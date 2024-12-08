#include <iostream>
#include <thread>
#include <vector>
#include <unistd.h>
#include "signal_handler.h"
#include "buffer.h"
#include "producer.h"
#include "consumer.h"

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

int main() {
    while (true) {
        std::cout << "=== Main Menu ===\n";
        std::cout << "1. Run Signal Handler Program\n";
        std::cout << "2. Run Producer/Consumer Program\n";
        std::cout << "0. Exit\n";
        std::cout << "Enter your choice: ";

        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1:
                std::cout << "Running Signal Handler Program...\n";
            runSignalHandler();
            break;
            case 2:
                std::cout << "Running Producer/Consumer Program...\n";
            runProducerConsumer();
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
