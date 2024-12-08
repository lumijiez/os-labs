//
// Created by lumijiez on 12/8/24.
//

#include "bconsumer.h"
#include <chrono>
#include <thread>

BConsumer::BConsumer(const int id, Buffer& buffer, std::counting_semaphore<5>& sem)
    : id(id), buffer(buffer), consumerSem(sem) {}

void BConsumer::run() const {
    while (true) {
        consumerSem.acquire();
        int item = buffer.consume(id);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        consumerSem.release();
    }
}