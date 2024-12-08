//
// Created by lumijiez on 11/16/24.
//
#include "consumer.h"

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <atomic>
#include <fcntl.h>
#include <sstream>

Consumer::Consumer(const int id, const int read_fd, std::counting_semaphore<5>& consumerSem)
    : consumerId(id), readFd(read_fd), consumerSemaphore(consumerSem) {}

void Consumer::run(std::atomic<int>& itemsConsumed, const std::atomic<bool>& stopFlag) const {
    const int flags = fcntl(readFd, F_GETFL, 0);
    fcntl(readFd, F_SETFL, flags | O_NONBLOCK);

    while (!stopFlag || itemsConsumed < 60) {
        consumerSemaphore.acquire();

        int item;

        if (const ssize_t bytes_read = read(readFd, &item, sizeof(item)); bytes_read > 0) {
            std::ostringstream output;
            output << "Consumer " << consumerId << " consumed: " << item << std::endl;
            std::cout << output.str();

            ++itemsConsumed;
        }
        else if (bytes_read == 0) {
            break;
        }
        else {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                std::ostringstream output;
                output << "Consumer " << consumerId << "read error: " << strerror(errno) << std::endl;
                std::cerr << output.str();
                break;
            }
        }

        consumerSemaphore.release();

        usleep(150000);  // 150ms
    }
}
