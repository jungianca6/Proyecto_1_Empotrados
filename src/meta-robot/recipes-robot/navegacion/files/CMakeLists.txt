cmake_minimum_required(VERSION 3.10)
project(app_robot C)

add_executable(app_robot src/main.c)
target_link_libraries(app_robot robot)

install(TARGETS app_robot DESTINATION bin)
