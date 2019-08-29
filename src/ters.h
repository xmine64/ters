// Ters the Terminal scroller
// Copyright (C) 2019 By Mohammad Amin Mollazadeh

// requiered libraries
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

// panic for unexpected error
void panic(const char *error, ...);

/* pty */

// initialize pty
int pty_init();
// send data to child through pty
void pty_send(char *buffer, int count);

/* events */

// initialize libevent
void events_init(int pty_fd);
// start event loop
void events_loop();
// stop event loop
void events_stop();

/* screen */

// initialize ncurses
void screen_init();
// stop ncurses
void screen_close();
// handle user input
void screen_handle_user_input(char *buf, int count);
// refresh screen
void screen_refresh();
// clear buffer
void screen_clear();
// append data to buffer
void buffer_append(const char *buffer, int count);
