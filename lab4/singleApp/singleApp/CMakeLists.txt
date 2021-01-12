cmake_minimum_required(VERSION 3.17)
project(singleApp)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(singleApp
        ../../lib/ArrayIntegerChannel.cpp
        ../../lib/ArrayIntegerChannel.h
        ../../lib/BinarySemaphore.cpp
        ../../lib/BinarySemaphore.h
        ../../lib/Constants.h
        ../../lib/helper.cpp
        ../../lib/helper.h
        ../../lib/IntegerChannel.cpp
        ../../lib/IntegerChannel.h
        ../../lib/Message.cpp
        ../../lib/Message.h
        ../../lib/myrandom.h
        coach.h
        defender.h
        doctor.h
        footballField.h
        forward.h
        goalkeeper.h
        singleApp.cpp
        ../../lib/Signal.cpp ../../lib/Signal.h)
