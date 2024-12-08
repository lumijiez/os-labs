//
// Created by lumijiez on 11/16/24.
//

#ifndef PRODUCER_H
#define PRODUCER_H

#include "buffer.h"
#include <semaphore>

class Producer {
public:
    Producer(int id, Buffer& buffer, std::counting_semaphore<3>& sem);
    void run() const;

private:
    int id;
    Buffer& buffer;
    std::counting_semaphore<3>& producerSem;
};

#endif //PRODUCER_H
