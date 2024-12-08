//
// Created by lumijiez on 11/16/24.
//

#ifndef CONSUMER_H
#define CONSUMER_H

#include "buffer.h"
#include <semaphore>

class Consumer {
public:
    Consumer(int id, Buffer& buffer, std::counting_semaphore<5>& sem);
    void run() const;

private:
    int id;
    Buffer& buffer;
    std::counting_semaphore<5>& consumerSem;
};

#endif //CONSUMER_H
