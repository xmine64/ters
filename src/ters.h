// Ters the Terminal scroller
// Copyright (C) 2019 By Mohammad Amin Mollazadeh

// requiered libraries
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// panic for unexpected error
void panic(const char *error, ...);

/* pty */

// initialize pty
int pty_init();
// send key to child through pty
void pty_send_keypress(long keycode);

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
void screen_handle_user_input(long keycode);
// refresh screen
void screen_refresh();
// clear buffer
void screen_clear();
// print buffer to screen
void screen_print(const char *str);
