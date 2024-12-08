//
// Created by lumijiez on 12/8/24.
//

#ifndef BCONSUMER_H
#define BCONSUMER_H

#include "buffer.h"
#include <semaphore>

class BConsumer {
public:
    BConsumer(int id, Buffer& buffer, std::counting_semaphore<5>& sem);
    void run() const;

private:
    int id;
    Buffer& buffer;
    std::counting_semaphore<5>& consumerSem;
};

#endif //BCONSUMER_H
