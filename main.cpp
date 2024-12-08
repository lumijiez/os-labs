#include <iostream>
#include <thread>
#include <vector>
#include <unistd.h>
#include "signal_handler.h"
#include "buffer.h"
#include "producer.h"
#include "consumer.h"
#include "reader_writer.h"

[[noreturn]] void runSignalHandler() {
    setupSignalHandlers();
    std::cout << "PID: " << getpid() << std::endl;
    while (true) {
        pause();
    }
}

void runProducerConsumer() {
    Buffer buffer(10);
    std::counting_semaphore<3> producerSem(3);
    std::counting_semaphore<5> consumerSem(5);

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    for (int i = 0; i < 3; ++i) {
        producers.emplace_back([i, &buffer, &producerSem]() {
            const Producer producer(i, buffer, producerSem);
            producer.run();
        });
    }

    for (int i = 0; i < 3; ++i) {
        consumers.emplace_back([i, &buffer, &consumerSem]() {
            const Consumer consumer(i, buffer, consumerSem);
            consumer.run();
        });
    }

    for (auto& p : producers) {
        p.join();
    }
    for (auto& c : consumers) {
        c.join();
    }
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
            break;
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
