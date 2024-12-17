//
// Created by lumijiez on 11/16/24.
//

#ifndef BUFFER_H
#define BUFFER_H

#include <queue>
#include <mutex>
#include <condition_variable>

class Buffer {
public:
    explicit Buffer(int size);
    void produce(int producerId, int item);
    int consume(int consumerId);
    bool isEmpty() const;
    bool isFull() const;

private:
    std::queue<int> items;
    const int maxSize;
    mutable std::mutex mtx;
    std::condition_variable notFull;
    std::condition_variable notEmpty;
};

#endif //BUFFER_H
