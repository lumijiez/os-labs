//
// Created by lumijiez on 11/16/24.
//
#include "signal_handler.h"

void handleSignal(const int signal) {
    if (signal == SIGUSR1) {
        std::cout << "SIGUSR1 received" << std::endl;
    } else if (signal == SIGUSR2) {
        std::random_device rd;
        std::default_random_engine generator(rd());
        std::uniform_int_distribution distribution(32, 126);

        for (int i = 0; i < 100; ++i) {
            const char randomChar = static_cast<char>(distribution(generator));
            std::cout << randomChar;
        }
        std::cout << std::endl;
        std::exit(0);
    }
}

void setupSignalHandlers() {
    std::signal(SIGUSR1, handleSignal);
    std::signal(SIGUSR2, handleSignal);
}

