// ters screen manager
// Copyright (C) 2019 By Mohammad Amin Mollazadeh

#include "ters.h"

#include <curses.h>

/* buffer */

#define BUFFER_COLS 80
#define BUFFER_LINES 25
#define BUFFER_PAGES 10

static u_char Buffer[BUFFER_COLS * BUFFER_LINES * BUFFER_PAGES];

static int X = 0;
static int Y = 0;

// index of top line of screen
static int Top = 0;

// input mode, if true, keys will treated as command, else will send to child
// pressing ESC will enable it, pressing Enter will disable.
bool Mode = false;

u_char *buffer_get_pointer_at(int x, int y) {
    return Buffer + (y*COLS) + x;
}

u_char *buffer_get_pointer() {
    return buffer_get_pointer_at(X, Y);
}

void update_y() {
    Y++;
    if (Y >= BUFFER_LINES * BUFFER_PAGES) {
        screen_clear();
    }
    if (!Mode && Y >= LINES) {
        Top = Y - LINES + 1;
    }
}

void update_x() {
    X++;
    if (X >= COLS) {
        X = 0;
        update_y();
    }
}

void backspace() {
    if (X == 0) {
        Y -= 1;
        X = COLS - 1;
        buffer_get_pointer()[0] = '\0';
    } else {
        X -= 1;
        buffer_get_pointer()[0] = '\0';
    }
}

void buffer_append(const char *buffer, int count) {
    for (int i = 0; i < count; i++) {
        if (buffer[i] == '\n') {
            update_y();
            continue;
        }
        if (buffer[i] == '\r') {
            X = 0;
            continue;
        }
        if (buffer[i] == '\b') {
            backspace();
            continue;
        }
        buffer_get_pointer()[0] = buffer[i];
        update_x();
    }
}

void screen_clear() {
    X = 0;
    Y = 0;
    memset(Buffer, 0, sizeof(Buffer));
}

/* screen */

void screen_init() {
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    refresh();
}

void screen_close() {
    endwin();
}

/* render */
void screen_refresh() {
    clear();
    for (int x = 0; x < COLS; x++) {
        for (int y = Top; y < LINES + Top; y++) {
            u_char c = buffer_get_pointer_at(x, y)[0];
            if (c != '\0') {
                mvaddch(y - Top, x, c);
            }
        }
    }
    refresh();
}

/* input handler */
#define KEY_ESCAPE 27

void screen_handle_user_input(char *buffer, int count) {
    if (Mode) {
        if (buffer[0] == 27 && buffer[1] == 79) {
            switch (buffer[2]) {
                // key up
                case 65:
                    if (Top > 0)
                        Top -= 1;
                    // TODO else show error message
                    screen_refresh();
                    return;
                case 66:
                    if ((Top + LINES) < (BUFFER_LINES * BUFFER_PAGES))
                        Top += 1;
                    // TODO else show error message;
                    screen_refresh();
                    return;
                default:
                    clear();
                    printw("invalid key: %d %d %d\n", buffer[0], buffer[1], buffer[2]);
                    refresh();
                    sleep(3);
                    screen_refresh();
                    return;
            }
        }
        switch (buffer[0]) {
            case 'q':
                events_stop();
                break;
            case KEY_ESCAPE:
                if (count == 0) {
                    pty_send(buffer, count);
                    Mode = false;
                } else {
                    clear();
                    printw("invalid key: %d %d %d\n", buffer[0], buffer[1], buffer[2]);
                    refresh();
                    sleep(3);
                    screen_refresh();
                }
                break;
            case '\r':
                Mode = false;
                break;
            default:
                clear();
                printw("invalid key: %d %d %d\n", buffer[0], buffer[1], buffer[2]);
                refresh();
                sleep(3);
                screen_refresh();
                break;
        }
        return;
    }

    if (count == 1 && buffer[0] == KEY_ESCAPE) {
        // TODO: show a 'waiting for key press' message
        Mode = true;
    } else {
        pty_send(buffer, count);
    }
}
