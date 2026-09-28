cmake_minimum_required(VERSION 3.16)
project(robot-navigation VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_library(ROBOT_LIBRARY robot REQUIRED)

add_executable(navegacion main.c fsm.c)
target_include_directories(navegacion PRIVATE ${CMAKE_SYSROOT}/usr/include/robot)
target_link_libraries(navegacion PRIVATE ${ROBOT_LIBRARY})
target_compile_options(navegacion PRIVATE -Wall -Wextra -Wpedantic)

install(TARGETS navegacion RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
