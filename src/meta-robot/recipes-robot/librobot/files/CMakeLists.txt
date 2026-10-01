cmake_minimum_required(VERSION 3.10)
project(librobot C)

include_directories(include)

add_library(robot SHARED
    src/motors.c
    src/sensors.c
    src/fsm.c
    src/gpio.c
)

install(TARGETS robot DESTINATION lib)
install(FILES 
    include/motors.h 
    include/sensors.h 
    include/fsm.h 
    include/gpio.h 
    DESTINATION include
)
