cmake_minimum_required(VERSION 3.14)
project(handout C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(handout
    clock.h
    constants.h
    context.c
    scheduler.c
    scheduler.h
    structs.h
    sys.h)
