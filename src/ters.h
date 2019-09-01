// Ters the Terminal scroller
// Copyright (C) 2019 By Mohammad Amin Mollazadeh

// requiered libraries
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

void panic(const char *error, ...);

/* pty */
int pty_init();
void pty_send_keypress(long keycode);

/* events */
void events_init(int pty_fd);
void events_loop_start();
void events_loop_stop();

/* screen */

// total pages to be allocated
#define PAGES 500

void screen_init();
void screen_close();

void screen_refresh();
void screen_clear();
void screen_beep();
void screen_print_buffer(char *buffer, int count);

void screen_popup(int width, int height, char **lines);
bool screen_is_popup();
void screen_close_popup();

void screen_set_mode(bool mode);
bool screen_get_mode();

bool screen_scroll(int lines);
bool screen_scroll_to(int line);

int screen_get_pos();           // get scroller position
int screen_get_line();          // get current line number in buffer
int screen_get_lines_in_page(); // get total lines in each page

/* input handler */

// keycodes
#define KC_ESCAPE 27
#define KC_ENTER 13
// #define KC_UP 4283163
// #define KC_DOWN 4348699
#define KC_UP 4280091
#define KC_DOWN 4345627
#define KC_PAGEUP 2117425947
#define KC_PAGEDOWN 2117491483
#define KC_HOME 2117163803
#define KC_END 2117360411

void input_handler_handle(long keycode);

/* actions */
void actions_line_up();
void actions_line_down();
void actions_page_up();
void actions_page_down();
void actions_scroll_top();
void actions_scroll_end();
void actions_help();
