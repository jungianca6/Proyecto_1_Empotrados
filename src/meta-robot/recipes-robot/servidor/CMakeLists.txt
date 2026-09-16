cmake_minimum_required(VERSION 3.10)
project(robot-server C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(robot-server robot-server.c)
target_compile_options(robot-server PRIVATE -Wall -Wextra -Wpedantic)

install(TARGETS robot-server RUNTIME DESTINATION bin)
install(FILES robot-server.service DESTINATION lib/systemd/system)