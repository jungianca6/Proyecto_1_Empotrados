cmake_minimum_required(VERSION 3.16)
project(robot-audio VERSION 1.0.0 LANGUAGES C)

include(GNUInstallDirs)
include(CTest)

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
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(robot-audio
    PRIVATE
        PkgConfig::ALSA
        PkgConfig::MPG123
        Threads::Threads
)
target_compile_definitions(robot-audio PRIVATE _DEFAULT_SOURCE)
target_compile_options(robot-audio PRIVATE -Wall -Wextra -Wpedantic)
set_target_properties(robot-audio PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER include/robot_audio.h
)

add_executable(audio-test audio_test.c)
target_link_libraries(audio-test PRIVATE robot-audio)
target_compile_options(audio-test PRIVATE -Wall -Wextra -Wpedantic)

install(TARGETS robot-audio
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(TARGETS audio-test RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if(BUILD_TESTING)
    add_test(NAME audio-test-help COMMAND audio-test --help)
endif()
