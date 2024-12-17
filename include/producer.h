//
// Created by lumijiez on 11/16/24.
//

#ifndef PRODUCER_H
#define PRODUCER_H

#include <semaphore>

class Producer {
public:
    Producer(int id, int write_fd, std::counting_semaphore<3>& producerSem);
    void run(std::atomic<int>& itemsProduced, const std::atomic<bool>& stopFlag) const;

private:
    int producerId;
    int writeFd;
    std::counting_semaphore<3>& producerSemaphore;
};

#endif
