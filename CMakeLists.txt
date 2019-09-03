
cmake_minimum_required(VERSION 3.13)
project(ters)

add_executable(ters
        src/main.c
        src/pty.c src/screen.c src/events.c src/vt.c src/vt.h)

target_link_libraries(ters util event_core ncursesw)