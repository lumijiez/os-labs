cmake_minimum_required(VERSION 3.29)
project(os-labs)

set(CMAKE_CXX_STANDARD 23)

add_executable(
        os-labs
        main.cpp
        include/signal_handler.h
        src/signal_handler.cpp
        include/buffer.h
        src/buffer.cpp
        include/producer.h
        src/consumer.cpp
        include/consumer.h
        src/producer.cpp
        include/reader_writer.h
        src/reader_writer.cpp
        include/p_producer_consumer.h
        src/p_producer_consumer.cpp
        include/bconsumer.h
        include/bproducer.h
        src/bconsumer.cpp
        src/bproducer.cpp
)
target_link_libraries(os-labs PRIVATE m)
target_include_directories(os-labs PRIVATE ${PROJECT_SOURCE_DIR}/include)