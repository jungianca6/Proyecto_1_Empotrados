cmake_minimum_required(VERSION 3.10)

project(robot-server C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")

option(USE_STUB "Enlazar contra librobot_stub (solo desarrollo, NO para la imagen final)" OFF)


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

find_package(PkgConfig REQUIRED)
pkg_check_modules(CJSON REQUIRED IMPORTED_TARGET libcjson)

# ============================================================
# Sesiones
# ============================================================

add_library(session STATIC
    lib/session/session.c
)

target_include_directories(session PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/session
)



# ============================================================
# API de librobot
# ============================================================

add_library(robot_api INTERFACE)

if(USE_STUB)

    message(WARNING
        "Compilando contra librobot_stub: NO usar en la imagen final"
    )

    target_include_directories(robot_api INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/include
    )

    add_library(robot_stub STATIC
        lib/stub/librobot_stub.c
    )

    target_include_directories(robot_stub PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/include
    )

    target_link_libraries(robot_api INTERFACE
        robot_stub
    )

else()

    # librobot.h y librobot.so son proporcionados por la receta
    # librobot mediante el sysroot de Yocto.

    find_path(ROBOT_INCLUDE_DIR
        NAMES librobot.h
        REQUIRED
    )

    find_library(ROBOT_LIB
        NAMES robot
        REQUIRED
    )

    target_include_directories(robot_api INTERFACE
        ${ROBOT_INCLUDE_DIR}
    )

    target_link_libraries(robot_api INTERFACE
        ${ROBOT_LIB}
    )

endif()

# ============================================================
# CGI
# ============================================================

foreach(CGI ${CGI_PROGRAMS})

    add_executable(${CGI}
        cgi/${CGI}.c
    )

    target_link_libraries(${CGI}
        PRIVATE
            session
            robot_api
            PkgConfig::CJSON
    )

    target_include_directories(${CGI}
        PRIVATE
            ${CJSON_INCLUDE_DIRS}
    )

    install(
        TARGETS ${CGI}
        RUNTIME DESTINATION /www/cgi-bin
    )

endforeach()

install(
    DIRECTORY www/
    DESTINATION /www
)