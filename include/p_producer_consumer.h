//
// Created by lumijiez on 12/8/24.
//

#ifndef P_PRODUCER_CONSUMER_H
#define P_PRODUCER_CONSUMER_H

#define PNUM_PRODUCERS 3
#define PNUM_CONSUMERS 2
#define PBUFFER_SIZE 10

#define PSEM_MUTEX_NAME "/buffer_mutex"
#define PSEM_FULL_NAME "/buffer_full"
#define PSEM_EMPTY_NAME "/buffer_empty"

void pproducer(int producer_id, int write_fd);

void pconsumer(int consumer_id, int read_fd);

#endif //P_PRODUCER_CONSUMER_H
