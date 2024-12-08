//
// Created by lumijiez on 12/8/24.
//

#include "reader_writer.h"

#include <iostream>
#include <sstream>
#include <utility>
#include <unistd.h>
#include <vector>

ReaderWriter::ReaderWriter(const int numReaders, const int numWriters, std::string filename)
    : numReaders(numReaders), numWriters(numWriters), filename(std::move(filename)), readCount(0), writeCount(0) {
    sem_init(&resourceAccess, 0, 1);
    sem_init(&readCountAccess, 0, 1);
}

ReaderWriter::~ReaderWriter() {
    sem_destroy(&resourceAccess);
    sem_destroy(&readCountAccess);
}

void* ReaderWriter::reader(void* arg) {
    auto* rw = static_cast<ReaderWriter*>(arg);

    for (int i = 0; i < 5; ++i) {
        sem_wait(&rw->readCountAccess);
        rw->readCount++;
        if (rw->readCount == 1) {
            sem_wait(&rw->resourceAccess);
        }
        sem_post(&rw->readCountAccess);

        if (std::ifstream file(rw->filename); file.is_open()) {
            std::ostringstream output;
            output << "Reader reading: ";
            std::string line;
            while (std::getline(file, line)) {
                output << line << " ";
            }
            output << std::endl;
            std::cout << output.str();
        } else {
            std::cerr << "Reader could not open file\n";
        }

        sem_wait(&rw->readCountAccess);
        rw->readCount--;
        if (rw->readCount == 0) {
            sem_post(&rw->resourceAccess);
        }
        sem_post(&rw->readCountAccess);

        sleep(1);
    }
    return nullptr;
}



void* ReaderWriter::writer(void* arg) {
    auto* rw = static_cast<ReaderWriter*>(arg);

    for (int i = 0; i < 5; ++i) {
        sem_wait(&rw->resourceAccess);

        std::ofstream file(rw->filename, std::ios::app);
        if (file.is_open()) {
            file << rw->writeCount << "\n";
            std::cout << "Writer writing: " << rw->writeCount << std::endl;
            rw->writeCount++;
        } else {
            std::cerr << "Writer could not open file\n";
        }
        file.close();

        sem_post(&rw->resourceAccess);

        sleep(1);
    }
    return nullptr;
}

void ReaderWriter::run() {
    std::vector<pthread_t> readerThreads(numReaders);
    std::vector<pthread_t> writerThreads(numWriters);

    for (int i = 0; i < numReaders; ++i) {
        pthread_create(&readerThreads[i], nullptr, reader, this);
    }
    for (int i = 0; i < numWriters; ++i) {
        pthread_create(&writerThreads[i], nullptr, writer, this);
    }

    for (const auto& t : readerThreads) {
        pthread_join(t, nullptr);
    }
    for (const auto& t : writerThreads) {
        pthread_join(t, nullptr);
    }
}