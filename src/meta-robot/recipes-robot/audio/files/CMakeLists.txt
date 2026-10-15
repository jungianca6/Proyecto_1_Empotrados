cmake_minimum_required(VERSION 3.16)
project(robot-audio VERSION 1.0.0 LANGUAGES C)

include(GNUInstallDirs)
include(CTest)

set(SYSTEMD_UNIT_DIR "${CMAKE_INSTALL_LIBDIR}/systemd/system" CACHE PATH
    "Directory for systemd unit files")
set(ROBOT_AUDIO_SOCKET_PATH "/run/robot-audio.sock" CACHE STRING
    "Unix socket used by the audio daemon")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(ALSA REQUIRED IMPORTED_TARGET alsa)
pkg_check_modules(MPG123 REQUIRED IMPORTED_TARGET libmpg123)
find_package(Threads REQUIRED)

add_library(robot-audio SHARED robot_audio.c)
target_include_directories(robot-audio
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(robot-audio PRIVATE PkgConfig::ALSA PkgConfig::MPG123 Threads::Threads)
target_compile_definitions(robot-audio PRIVATE _DEFAULT_SOURCE)
target_compile_options(robot-audio PRIVATE -Wall -Wextra -Wpedantic)
set_target_properties(robot-audio PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER robot_audio.h
)

add_executable(audio-test audio_test.c)
target_link_libraries(audio-test PRIVATE robot-audio)
target_compile_options(audio-test PRIVATE -Wall -Wextra -Wpedantic)

add_executable(audio-daemon audio_daemon.c)
target_link_libraries(audio-daemon PRIVATE robot-audio)
target_compile_options(audio-daemon PRIVATE -Wall -Wextra -Wpedantic)
target_compile_definitions(audio-daemon PRIVATE
    AUDIO_SOCKET_PATH="${ROBOT_AUDIO_SOCKET_PATH}")

add_executable(audioctl audioctl.c)
target_compile_options(audioctl PRIVATE -Wall -Wextra -Wpedantic)
target_compile_definitions(audioctl PRIVATE
    AUDIO_SOCKET_PATH="${ROBOT_AUDIO_SOCKET_PATH}")

install(TARGETS robot-audio
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(TARGETS audio-test RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS audio-daemon audioctl RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES robot-audio.service DESTINATION ${SYSTEMD_UNIT_DIR})
