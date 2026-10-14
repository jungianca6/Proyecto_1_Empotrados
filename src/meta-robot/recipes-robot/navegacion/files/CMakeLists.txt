cmake_minimum_required(VERSION 3.10)
project(app_robot C)

add_executable(app_robot
    src/main.c
    src/mapper.c
    src/nav_events.c
)

target_include_directories(app_robot PRIVATE src)
target_link_libraries(app_robot robot m)

install(TARGETS
    app_robot
    DESTINATION bin
)
