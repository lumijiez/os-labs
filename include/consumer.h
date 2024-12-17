//
// Created by lumijiez on 11/16/24.
//

#ifndef CONSUMER_H
#define CONSUMER_H

#include <semaphore>

class Consumer {
public:
    Consumer(int id, int read_fd, std::counting_semaphore<5>& consumerSem);
    void run(std::atomic<int>& itemsConsumed, const std::atomic<bool>& stopFlag) const;

private:
    int consumerId;
    int readFd;
    std::counting_semaphore<5>& consumerSemaphore;
};

#endif

