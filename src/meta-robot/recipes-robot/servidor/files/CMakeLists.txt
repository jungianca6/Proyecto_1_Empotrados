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

#add_library(session STATIC cgi/session.c)
#target_include_directories(session PUBLIC cgi)

find_package(PkgConfig REQUIRED)
pkg_check_modules(CJSON REQUIRED IMPORTED_TARGET libcjson)

foreach(CGI ${CGI_PROGRAMS})
    add_executable(${CGI} cgi/${CGI}.c)

    target_link_libraries(${CGI} PRIVATE PkgConfig::CJSON)
    target_include_directories(${CGI} PRIVATE ${CJSON_INCLUDE_DIRS})

    install(
        TARGETS ${CGI}
        RUNTIME DESTINATION /www/cgi-bin
    )
endforeach()

install(
    DIRECTORY www/
    DESTINATION /www
)