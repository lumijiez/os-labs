#include <iostream>
#include <thread>
#include <vector>
#include <unistd.h>
#include "signal_handler.h"
#include "buffer.h"
#include "producer.h"
#include "consumer.h"
#include "reader_writer.h"
#include <fcntl.h>

std::atomic stopProduction(false);

[[noreturn]] void runSignalHandler() {
    setupSignalHandlers();
    std::cout << "PID: " << getpid() << std::endl;
    while (true) {
        pause();
    }
}

void runProducerConsumer() {
    const int NUM_PRODUCERS = 3;
    const int NUM_CONSUMERS = 2;

    // Create pipe
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        std::cerr << "Pipe creation failed" << std::endl;
        return;
    }

    // Set pipe to non-blocking mode
    int flags = fcntl(pipe_fd[1], F_GETFL, 0);
    fcntl(pipe_fd[1], F_SETFL, flags | O_NONBLOCK);

    // Create semaphores
    std::counting_semaphore<3> producerSemaphore(3);
    std::counting_semaphore<5> consumerSemaphore(5);

    // Atomic flags for synchronization
    std::atomic<bool> stopProduction(false);
    std::atomic<int> itemsProduced(0);
    std::atomic<int> itemsConsumed(0);

    // Create threads
    std::vector<std::thread> producerThreads;
    std::vector<std::thread> consumerThreads;

    // Create producer threads
    for (int i = 0; i < NUM_PRODUCERS; ++i) {
        producerThreads.emplace_back([&, i]() {
            Producer producer(i, pipe_fd[1], producerSemaphore);
            producer.run(itemsProduced, stopProduction);
        });
    }

    // Create consumer threads
    for (int i = 0; i < NUM_CONSUMERS; ++i) {
        consumerThreads.emplace_back([&, i]() {
            Consumer consumer(i, pipe_fd[0], consumerSemaphore);
            consumer.run(itemsConsumed, stopProduction);
        });
    }

    // Wait for producers to finish
    for (auto& thread : producerThreads) {
        thread.join();
    }

    // Signal stop and wait for consumers
    stopProduction = true;
    for (auto& thread : consumerThreads) {
        thread.join();
    }

    // Close pipe
    close(pipe_fd[0]);
    close(pipe_fd[1]);

    // Print final statistics
    std::cout << "Total items produced: " << itemsProduced
              << "\nTotal items consumed: " << itemsConsumed << std::endl;
}

void runReaderWriter() {
    int numReaders = 5;
    int numWriters = 2;
    std::string filename = "shared_resource.txt";

    std::ofstream file(filename, std::ios::trunc);
    file.close();

    ReaderWriter rw(numReaders, numWriters, filename);
    rw.run();
}

int main() {
    while (true) {
        std::cout << "=== Main Menu ===\n";
        std::cout << "1. Run Signal Handler\n";
        std::cout << "2. Run Producer/Consumer\n";
        std::cout << "3. Run Reader/Writer\n";
        std::cout << "0. Exit\n";
        std::cout << "Enter your choice: ";

        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1:
                std::cout << "Running Signal Handler...\n";
            runSignalHandler();
            case 2:
                std::cout << "Running Producer/Consumer...\n";
            runProducerConsumer();
            break;
            case 3:
                std::cout << "Running Reader-Writer...\n";
            runReaderWriter();
            break;
            case 0:
                std::cout << "Exiting program. Goodbye!\n";
            return 0;
            default:
                std::cout << "Invalid choice. Please try again.\n";
            break;
        }
    }
}
