cmake_minimum_required(VERSION 3.10)

project(robot-server C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")

set(CGI_PROGRAMS
    login
    status
    mode
    motors
    sensors
    leds
    audiolist
    audioplay
    audiopause
    audiostop
    audiovolume
    map
)

foreach(CGI ${CGI_PROGRAMS})
    add_executable(${CGI} cgi/${CGI}.c)

    install(
        TARGETS ${CGI}
        RUNTIME DESTINATION /www/cgi-bin
    )
endforeach()

install(
    DIRECTORY www/
    DESTINATION /www
)