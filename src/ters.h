// the TERminal Scroller
// Copyright (C) 2019 By Mohammad Amin Mollazadeh

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

void panic(const char *error, ...);

int pty_init();
void pty_send(char *buffer, int count);

void events_init(int pty_fd);
void events_loop();
void events_stop();

void screen_init();
void screen_close();
void screen_handle_user_input(char *buf, int count);
void screen_refresh();
void screen_clear();
void buffer_append(const char *buffer, int count);
