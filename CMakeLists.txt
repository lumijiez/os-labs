cmake_minimum_required(VERSION 3.29)
project(os-labs)

set(CMAKE_CXX_STANDARD 23)

add_executable(
        os-labs
        main.cpp
        src/signal_handler.cpp
        include/signal_handler.h
        include/buffer.h
        src/buffer.cpp
        include/producer.h
        src/consumer.cpp
        include/consumer.h
        src/producer.cpp
)
target_link_libraries(os-labs PRIVATE m)
target_include_directories(os-labs PRIVATE ${PROJECT_SOURCE_DIR}/include)