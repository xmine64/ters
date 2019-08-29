// Ters screen manager
// Copyright (C) 2019 By Mohammad Amin Mollazadeh

#include "ters.h"

#include <curses.h>

/* buffer */

// for ease of sizing buffer
#define BUFFER_COLS 180   // columns of my console
#define BUFFER_LINES 64   // lines of my console
#define BUFFER_PAGES 1000 // selected randomly

static u_char Buffer[BUFFER_COLS * BUFFER_LINES * BUFFER_PAGES];

// location of buffer
static int X = 0;
static int Y = 0;

// location of scroller (Y of line that prints on top of screen)
static int Top = 0;

// 'Mode = true' is scroll mode that user can scroll
// 'Mode = false' is normal mode that input will send to child
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

    // instead of here, it'll checked in buffer_append
    /*
    // clear buffer when Y reaches to end of buffer
    if (Y >= BUFFER_LINES * BUFFER_PAGES) {
        screen_clear();
    }
    */

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

// append new data received from pty to buffer
void buffer_append(const char *buffer, int count) {
    // clear screen when it doesn't fits in buffer.
    if ((Y*COLS) + X + count > sizeof(Buffer)) {
        screen_clear();
    }

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
        // beep when '\a' received
        if (buffer[i] == '\a') {
        	beep();
        	flash();
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
    keypad(stdscr, TRUE);
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
    // check if screen is not out of buffer
    if ((Top + LINES) * COLS > sizeof(Buffer)) {
    	printw( "Top over buffer error\n"
    			"Top: %d, Buffer size: %d\n",
    			Top, BUFFER_LINES*BUFFER_PAGES);
    	sleep(1);
    	Top = 0;
    	screen_refresh();
    }
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

/* key codes */
#define KC_ESCAPE 27
#define KC_ENTER 13
// without keypad() #define KC_UP 4283163
// without keypad() #define KC_DOWN 4348699
#define KC_UP 4280091
#define KC_DOWN 4348699
#define KC_PAGEUP 2117425947
#define KC_PAGEDOWN 2117491483
#define KC_HOME 2117163803
#define KC_END 2117360411

/* actions */
void screen_action_line_up() {
	if (Top > 0) {
		Top -= 1;
		screen_refresh();
	} else {
		beep();
		flash();
	}
}

void screen_action_line_down() {
	if ((Top + LINES) < (BUFFER_LINES * BUFFER_PAGES)) {
		Top += 1;
		screen_refresh();
	} else {
		beep();
		flash();
	}
}

void screen_action_page_up() {
	if (Top - LINES >= 0) {
		Top -= LINES;
		screen_refresh();
	} else {
		beep();
		flash();
	}
}

void screen_action_page_down() {
	if ((Top + (2*LINES)) < (BUFFER_LINES * BUFFER_PAGES)) {
		Top += LINES;
		screen_refresh();
	} else {
		beep();
		flash();
	}
}

void screen_action_scroll_top() {
	Top = 0;
	screen_refresh();
}

void screen_action_scroll_end() {
	Top = (BUFFER_LINES*BUFFER_PAGES) - LINES - 1;
	screen_refresh();
}

void screen_action_scroll_mode() {
	Mode = true;

	// TODO: show a 'waiting for key press' message on bottom or top of screen

	screen_message("scroll mode enabled.");
	sleep(1);
	screen_refresh();	
}

void screen_action_normal_mode() {
	Mode = false;
	if (Y >= LINES) {
		Top = Y - LINES + 1;
	}
	
	screen_message("scroll mode disabled.");
	sleep(1);
	screen_refresh();
}

void screen_action_help() {
	screen_message( "[Esc]       send escape to child\n"
					"[Up]        scroll up\n"
					"[Down]      scroll down\n"
					"[Page up]   scroll one page up\n"
					"[Page down] scroll one page down\n"
					"[r]         refresh screen (to show terminal again)\n"
					"[q]         quit\n"
					);
}

/* handler */

void screen_handle_scroll_mode(long keycode) {
	switch (keycode) {
		// send [Esc] to running program
		case KC_ESCAPE:
			screen_action_normal_mode();
			pty_send_keypress(KC_ESCAPE);
			break;
		// back to normal mode
		case KC_ENTER:
			screen_action_normal_mode();
			break;
			
		// scroll one line up
		case KC_UP:
			screen_action_line_up();
			break;
		// scroll one line down
		case KC_DOWN:
			screen_action_line_down();
			break;
		// scroll one page up
		case KC_PAGEUP:
			screen_action_page_up();
			break;
		// scroll one page down
		case KC_PAGEDOWN:
			screen_action_page_down();
			break;
		// scroll to top
		case KC_HOME:
			screen_action_scroll_top();
			break;
		// scroll to end
		case KC_END:
			screen_action_scroll_end();
			break;
			
		// help
		case 'h':
			screen_action_help();
			break;
		// refresh screen
		case 'r':
			screen_refresh();
			break;
		// quit
		case 'q':
			events_stop();
			break;

		default:
			screen_message( "invalid key: %d\n"
							"press [h] for help.",
							keycode);
			break;
	}
}

void screen_handle_user_input(long keycode) {
	if (Mode) {
		screen_handle_scroll_mode(keycode);
	} else {
		if (keycode == KC_ESCAPE) {
			screen_action_scroll_mode();
		} else {
			pty_send_keypress(keycode);
		}
	}
}
