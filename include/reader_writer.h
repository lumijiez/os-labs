//
// Created by lumijiez on 12/8/24.
//

#ifndef READER_WRITER_H
#define READER_WRITER_H

#include <semaphore.h>
#include <fstream>

class ReaderWriter {
public:
    ReaderWriter(int numReaders, int numWriters, std::string  filename);
    ~ReaderWriter();
    void run();

private:
    int numReaders;
    int numWriters;
    std::string filename;

    sem_t resourceAccess{};
    sem_t readCountAccess{};
    int readCount;
    int writeCount;

    static void* reader(void* arg);
    static void* writer(void* arg);
};
#endif //READER_WRITER_H
