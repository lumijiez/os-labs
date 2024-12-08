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
#include <atomic>
#include <sys/wait.h>

#include "bconsumer.h"
#include "bproducer.h"
#include "p_producer_consumer.h"

std::atomic stopProduction(false);

[[noreturn]] void runSignalHandler() {
    setupSignalHandlers();
    std::cout << "PID: " << getpid() << std::endl;
    while (true) {
        pause();
    }
}

void runProducerConsumer() {
    constexpr int NUM_PRODUCERS = 3;
    constexpr int NUM_CONSUMERS = 2;

    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        std::cerr << "Pipe creation failed" << std::endl;
        return;
    }

    const int flags = fcntl(pipe_fd[1], F_GETFL, 0);
    fcntl(pipe_fd[1], F_SETFL, flags | O_NONBLOCK);

    std::counting_semaphore<3> producerSemaphore(3);
    std::counting_semaphore<5> consumerSemaphore(5);

    std::atomic stopProduction(false);
    std::atomic itemsProduced(0);
    std::atomic itemsConsumed(0);

    std::vector<std::thread> producerThreads;
    std::vector<std::thread> consumerThreads;

    for (int i = 0; i < NUM_PRODUCERS; ++i) {
        producerThreads.emplace_back([&, i]() {
            const Producer producer(i, pipe_fd[1], producerSemaphore);
            producer.run(itemsProduced, stopProduction);
        });
    }

    for (int i = 0; i < NUM_CONSUMERS; ++i) {
        consumerThreads.emplace_back([&, i]() {
            const Consumer consumer(i, pipe_fd[0], consumerSemaphore);
            consumer.run(itemsConsumed, stopProduction);
        });
    }

    for (auto& thread : producerThreads) {
        thread.join();
    }

    stopProduction = true;
    for (auto& thread : consumerThreads) {
        thread.join();
    }

    close(pipe_fd[0]);
    close(pipe_fd[1]);

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

void runProcessProducerConsumer() {
    const sem_t *mutex = sem_open(PSEM_MUTEX_NAME, O_CREAT, 0644, 1);
    const sem_t *full = sem_open(PSEM_FULL_NAME, O_CREAT, 0644, PBUFFER_SIZE);

    if (const sem_t *empty = sem_open(PSEM_EMPTY_NAME, O_CREAT, 0644, 0); mutex == SEM_FAILED || full == SEM_FAILED || empty == SEM_FAILED) {
        perror("Failed to create semaphores");
        return;
    }

    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        perror("Failed to create pipe");
        return;
    }

    for (int i = 0; i < PNUM_PRODUCERS; i++) {
        if (const pid_t pid = fork(); pid == 0) {
            close(pipe_fd[0]);
            pproducer(i, pipe_fd[1]);
            exit(0);
        }
    }

    for (int i = 0; i < PNUM_CONSUMERS; i++) {
        if (const pid_t pid = fork(); pid == 0) {
            close(pipe_fd[1]);
            pconsumer(i, pipe_fd[0]);
            exit(0);
        }
    }

    for (int i = 0; i < PNUM_PRODUCERS + PNUM_CONSUMERS; i++) {
        wait(nullptr);
    }

    sem_unlink(PSEM_MUTEX_NAME);
    sem_unlink(PSEM_FULL_NAME);
    sem_unlink(PSEM_EMPTY_NAME);
}

void runProducerConsumerBuffer() {
    Buffer buffer(10);
    std::counting_semaphore<3> producerSem(3);
    std::counting_semaphore<5> consumerSem(5);

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    for (int i = 0; i < 3; ++i) {
        producers.emplace_back([i, &buffer, &producerSem]() {
            const BProducer bproducer(i, buffer, producerSem);
            bproducer.run();
        });
    }

    for (int i = 0; i < 3; ++i) {
        consumers.emplace_back([i, &buffer, &consumerSem]() {
            const BConsumer bconsumer(i, buffer, consumerSem);
            bconsumer.run();
        });
    }

    for (auto& p : producers) {
        p.join();
    }
    for (auto& c : consumers) {
        c.join();
    }
}

int main() {
    while (true) {
        std::cout << "=== Main Menu ===\n";
        std::cout << "1. Run Signal Handler\n";
        std::cout << "2. Run Threaded Producer/Consumer\n";
        std::cout << "3. Run Process Producer/Consumer\n";
        std::cout << "4. Run Buffered Producer/Consumer\n";
        std::cout << "5. Run Reader/Writer\n";
        std::cout << "0. Exit\n";
        std::cout << "Enter your choice: ";

        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1:
                std::cout << "Running Signal Handler...\n";
            runSignalHandler();
            case 2:
                std::cout << "Running Threaded Producer/Consumer...\n";
            runProducerConsumer();
            break;
            case 3:
                std::cout << "Running Process Producer/Consumer...\n";
            runProcessProducerConsumer();
            break;
            case 4:
                std::cout << "Running Buffered Producer/Consumer...\n";
            runProducerConsumerBuffer();
            break;
            case 5:
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
