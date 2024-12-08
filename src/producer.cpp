//
// Created by lumijiez on 11/16/24.
//
#include "producer.h"
#include <random>
#include <chrono>
#include <csignal>
#include <iostream>
#include <thread>

Producer::Producer(const int id, const int write_fd, std::counting_semaphore<3>& producerSem)
    : producerId(id), writeFd(write_fd), producerSemaphore(producerSem) {}

void Producer::run(std::atomic<int>& itemsProduced, const std::atomic<bool>& stopFlag) const {
    signal(SIGPIPE, SIG_IGN);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 1000);

    while (!stopFlag && itemsProduced < 60) {
        producerSemaphore.acquire();

        int item = dis(gen);

        if (const ssize_t bytes_written = write(writeFd, &item, sizeof(item)); bytes_written > 0) {
            std::ostringstream output;
            output << "Producer " << producerId << " produced: " << item << std::endl;
            std::cout << output.str();

            ++itemsProduced;
        } else {
            std::ostringstream output;
            output << "Producer " << producerId << "failed to write to pipe." << std::endl;
            std::cerr << output.str();
            if (errno != EAGAIN && errno != EINTR) {
                break;
            }
        }
        producerSemaphore.release();

        usleep(100000);
    }
}
