//
// Created by lumijiez on 12/8/24.
//

#include "p_producer_consumer.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <cstring>

void pproducer(const int producer_id, const int write_fd) {
    srand(time(nullptr) * getpid());

    while (true) {
        const int item = rand() % 100;
        char item_str[32];
        snprintf(item_str, sizeof(item_str), "%d", item);

        write(write_fd, item_str, strlen(item_str) + 1);

        printf("Producer %d produced: %d\n", producer_id, item);

        sleep(1);
    }
}

void pconsumer(const int consumer_id, const int read_fd) {
    while (true) {
        char item_str[32];

        if (const ssize_t bytesRead = read(read_fd, item_str, sizeof(item_str)); bytesRead > 0) {
            printf("Consumer %d consumed: %s\n", consumer_id, item_str);
        } else {
            if (bytesRead == 0) {
                break;
            }
            perror("Error reading from pipe");
        }

        sleep(2);
    }
}