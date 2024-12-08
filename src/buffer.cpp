//
// Created by lumijiez on 11/16/24.
//
#include "buffer.h"

#include <iostream>

Buffer::Buffer(const int size) : maxSize(size) {}

void Buffer::produce(const int producerId, const int item) {
    std::unique_lock lock(mtx);
    notFull.wait(lock, [this] { return !isFull(); });

    items.push(item);
    std::cout << "Producer " << producerId << " produced: " << item << std::endl;

    lock.unlock();
    notEmpty.notify_one();
}

int Buffer::consume(const int consumerId) {
    std::unique_lock lock(mtx);
    notEmpty.wait(lock, [this] { return !isEmpty(); });

    const int item = items.front();
    items.pop();
    std::cout << "Consumer " << consumerId << " consumed: " << item << std::endl;

    lock.unlock();
    notFull.notify_one();
    return item;
}

bool Buffer::isEmpty() const {
    return items.empty();
}

bool Buffer::isFull() const {
    return items.size() >= maxSize;
}