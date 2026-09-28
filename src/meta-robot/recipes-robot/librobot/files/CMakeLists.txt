cmake_minimum_required(VERSION 3.16)
project(robot-hardware VERSION 1.0.0 LANGUAGES C)

include(GNUInstallDirs)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(robot SHARED gpio.c motors.c sensors.c)
target_include_directories(robot PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(robot PRIVATE -Wall -Wextra -Wpedantic)
set_target_properties(robot PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION 1)

install(TARGETS robot LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES gpio.h motors.h sensors.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/robot)
