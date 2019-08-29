// Ters screen manager
// Copyright (C) 2019 By Mohammad Amin Mollazadeh

#include "ters.h"

#include <curses.h>

/* buffer */

// for ease of sizing buffer
#define BUFFER_COLS 80
#define BUFFER_LINES 25
#define BUFFER_PAGES 100

static u_char Buffer[BUFFER_COLS * BUFFER_LINES * BUFFER_PAGES];

// location of buffer
static int X = 0;
static int Y = 0;

// location of scroller (Y of line that prints on top of screen)
static int Top = 0;

// Mode = true is scroll mode that user can scroll
// Mode = false is normal mode that input will send to child
bool Mode = false;

// get a pointer that points to specified location of buffer
u_char *buffer_get_pointer_at(int x, int y) {
    return Buffer + (y*COLS) + x;
}

// get pointer that points to current location of buffer
u_char *buffer_get_pointer() {
    return buffer_get_pointer_at(X, Y);
}

// move Y one down
void update_y() {
    Y++;
    // clear buffer when Y reaches to end of buffer
    if (Y >= BUFFER_LINES * BUFFER_PAGES) {
        screen_clear();
    }
    // move scroller one down when first page is full
    if (!Mode && Y >= LINES) {
        Top = Y - LINES + 1;
    }
}

// move X one left
void update_x() {
    X++;
    // go to next line when line is full
    if (X >= COLS) {
        X = 0;
        update_y();
    }
}

// move pointer one back when '\b' received
void backspace() {
	// go previous line when current line is full
    if (X == 0) {
        Y -= 1;
        X = COLS - 1;
        buffer_get_pointer()[0] = '\0';
    } else {
        X -= 1;
        buffer_get_pointer()[0] = '\0';
    }
}

// append new data recieved from pty to buffer
void buffer_append(const char *buffer, int count) {
    for (int i = 0; i < count; i++) {
    	// go to new line when '\n' received
        if (buffer[i] == '\n') {
            update_y();
            continue;
        }
        // back to start of line when '\r' received
        if (buffer[i] == '\r') {
            X = 0;
            continue;
        }
        // do backspace when '\b' received
        if (buffer[i] == '\b') {
            backspace();
            continue;
        }
        // write character to buffer
        buffer_get_pointer()[0] = buffer[i];
        // move pointer
        update_x();
    }
}

void screen_clear() {
	// reset pointers
	Top = 0;
    X = 0;
    Y = 0;

    // fill buffer with '\0'
    memset(Buffer, 0, sizeof(Buffer));

    // refresh screen
    screen_refresh();
}

/* screen */

// initialize ncurses
void screen_init() {
    initscr();
    raw();
    noecho();
    //keypad(stdscr, TRUE);
    printw("loading...");
    refresh();
}

// close ncurses
void screen_close() {
    endwin();
}

// write a message to screen
void screen_message(const char *message, ...) {
    clear();

	va_list ap;

	va_start(ap, message);
	vwprintw(stdscr, message, ap);
	va_end(ap);

	refresh();
}

/* render */

// render buffer to screen
void screen_refresh() {
    clear();
    // for each LINE from Top
    for (int y = Top; y < LINES + Top; y++) {
    	// for each COL
        for (int x = 0; x < COLS; x++) {
            u_char c = buffer_get_pointer_at(x, y)[0];
            if (c != '\0') {
                mvaddch(y - Top, x, c);
            }
        }
    }
    refresh();
}

/* input handler */

// key code of [Esc] button
#define KEY_ESCAPE 27
#define KEY_ENTER 13

void screen_handle_user_input(char *buffer, int count) {
    if (Mode) {
    	// keypad keycodes starts with [0]=27, [1]=91
        if (buffer[0] == 27 && buffer[1] == 91) {
            switch (buffer[2]) {
            	// page up
            	case 53:
            		if (Top - LINES >= 0) {
            			Top -= LINES;
            			screen_refresh();
            		}
            		return;
            	// page down
            	case 54:
            		if ((Top + (2*LINES)) < (BUFFER_LINES * BUFFER_PAGES)) {
            		    Top += LINES;
            			screen_refresh();
            		}
            		return;
                // key up
                case 65:
                    if (Top > 0) {
                        Top -= 1;
                        screen_refresh();
                    } else {
                    	screen_message("top line");
                    }
                    return;
                case 66:
                    if ((Top + LINES) < (BUFFER_LINES * BUFFER_PAGES)) {
                        Top += 1;
                        screen_refresh();
                    } else {
                    	screen_message("last line");
                    }
                    return;
                default:
					screen_message("invalid key (keypad key): [%d] { %d , %d , %d }\n"
                			       "press [h] for help",
                			       count, buffer[0], buffer[1], buffer[2]);
                    return;
            }
        }
        switch (buffer[0]) {
        	// [h] quits
            case 'q':
                events_stop();
                break;
            // [h] shows a help message
            case 'h':
            	screen_message("[Esc]       send escape to child\n"
            		   		   "[Up]        scroll up\n"
            		           "[Down]      scroll down\n"
            		   		   "[Page up]   scroll one page up\n"
            		           "[Page down] scroll one page down\n"
            		           "[r]         refresh screen (to show terminal again)\n"
              		           "[q]         quit\n");
            	break;
            // [r] refreshes screen
            case 'r':
            	screen_refresh();
            	break;
            // send escpae key when [Esc] pressed in scroll mode
            case KEY_ESCAPE:
                if (count == 1) {
                    pty_send(buffer, count);
                    Mode = false;
                } else {
					screen_message("invalid key (escape key): [%d] { %d , %d , %d }\n"
                			       "press [h] for help",
                			       count, buffer[0], buffer[1], buffer[2]);
                }
                break;
            // [Enter] back to normal mode when
            case KEY_ENTER:
	            Mode = false;
                if (Y >= LINES) {
                	Top = Y - LINES + 1;
                }
            	screen_message("scroll mode disabled.");
            	sleep(1);
            	screen_refresh();
                break;
            // show an error for invalid key press
            default:
                screen_message("invalid key: [%d] { %d , %d , %d }\n"
                			   "press [h] for help",
                			    count, buffer[0], buffer[1], buffer[2]);
                break;
        }
        return;
    }

    // [Esc] is single byte 27.
    if (count == 1 && buffer[0] == KEY_ESCAPE) {
        // TODO: show a 'waiting for key press' message on bottom or top of screen
        Mode = true;
        screen_message("scroll mode enabled.");
        sleep(1);
        screen_refresh();
    } else {
    	// send keypress to child when not in scroll mode
        pty_send(buffer, count);
    }
}
