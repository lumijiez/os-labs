//
// Created by lumijiez on 11/16/24.
//
#include "consumer.h"
#include <chrono>
#include <thread>

Consumer::Consumer(const int id, Buffer& buffer, std::counting_semaphore<5>& sem)
    : id(id), buffer(buffer), consumerSem(sem) {}

void Consumer::run() const {
    while (true) {
        consumerSem.acquire();
        int item = buffer.consume(id);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        consumerSem.release();
    }
}
