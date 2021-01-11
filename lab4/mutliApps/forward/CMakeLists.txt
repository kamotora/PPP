cmake_minimum_required(VERSION 3.17)
project(forward)

set(CMAKE_CXX_STANDARD 14)

add_executable(
        forward
        ../lib/ArrayIntegerChannel.cpp
        ../lib/ArrayIntegerChannel.h
        ../lib/BinarySemaphore.cpp
        ../lib/BinarySemaphore.h
        ../lib/Constants.h
        ../lib/helper.cpp
        ../lib/helper.h
        ../lib/IntegerChannel.cpp
        ../lib/IntegerChannel.h
        ../lib/IntegerSemaphore.cpp
        ../lib/IntegerSemaphore.h
        ../lib/Message.cpp
        ../lib/Message.h
        ../lib/myrandom.h
        main.cpp
)