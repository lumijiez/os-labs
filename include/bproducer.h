//
// Created by lumijiez on 12/8/24.
//

#ifndef BPRODUCER_H
#define BPRODUCER_H

#include "buffer.h"
#include <semaphore>

class BProducer {
public:
    BProducer(int id, Buffer& buffer, std::counting_semaphore<3>& sem);
    void run() const;

private:
    int id;
    Buffer& buffer;
    std::counting_semaphore<3>& producerSem;
};

#endif //BPRODUCER_H
