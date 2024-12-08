//
// Created by lumijiez on 11/16/24.
//
#include "producer.h"
#include <random>
#include <chrono>
#include <thread>

Producer::Producer(const int id, Buffer& buffer, std::counting_semaphore<3>& sem)
    : id(id), buffer(buffer), producerSem(sem) {}

void Producer::run() const {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 100);

    while (true) {
        producerSem.acquire();
        const int item = dis(gen);
        buffer.produce(id, item);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        producerSem.release();
    }
}
