// Ters screen manager
// Copyright (C) 2019 By Mohammad Amin Mollazadeh

#include "ters.h"

#include <curses.h>

// total pages
#define PAGES 500

// pad is curses' scrollable window
static WINDOW *Pad;

// position of scroller on screen
static int Pos = 0;

// 'Mode = true' is scroll mode that user can scroll
// 'Mode = false' is normal mode that input will send to child
bool Mode = false;

// update scroller pos after printing
void screen_update_pos() {
	int y, x;
	getyx(Pad, y, x);

	int new_pos = y - LINES + 1;
	
	if (!Mode && new_pos > 0) {
	  	Pos = new_pos;
	}
}

// print new data received from pty
void screen_print_buffer(char *buffer, int count) {
	int y, x;
	for (int i=0; i < count; i++) {
		switch (buffer[i]) {
			case '\a':
				beep();
				flash();
				break;
			case '\r':
				getyx(Pad, y, x);
				wmove(Pad, y, 0);
				break;
			case '\n':
				getyx(Pad, y, x);
				wmove(Pad, y + 1, x);
				break;
			default:
				waddch(Pad, buffer[i]);
				break;
		}
	}
	
	screen_update_pos();
	screen_refresh();
}

void screen_printf(const char *str, ...) {
	va_list ap;
	va_start(ap, str);
	vwprintw(Pad, str, ap);
	va_end(ap);

	screen_update_pos();
	screen_refresh();
}

void screen_clear() {
	Pos = 0;

	wclear(Pad);
	
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

    Pad = newpad(LINES*PAGES, COLS);

    screen_printf("Ters, The Terminal Scroller\n"
    			  "v0.1 alpha (EXPERIMENTAL)\n\n");
}

// close ncurses
void screen_close() {
	delwin(Pad);
    endwin();
}

/* render */

// render buffer to screen
void screen_refresh() {
	// refresh Pad
	int lines = Mode? LINES - 1 : LINES;
    prefresh(Pad, Pos, 0, 0, 0, lines - 1, COLS - 1);

    // draw scroll indicator
    if (Mode) {
    	mvprintw(LINES - 1, 0, "[ SCROLL ] Line: %d   ", Pos);
    	mvprintw(LINES - 1, COLS - 20, "Press [h] for help.");
    }

    // move pointer to currect place
    if (Mode) {
    	move(LINES - 1, COLS - 1);
    } else {
    	int y, x;
    	getyx(Pad, y, x);
    	move(y > LINES? LINES-1 : y, x);
    }
    
    refresh();
}


/* input handler */

/* key codes */
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

/* actions */
void screen_action_line_up() {
	if (Pos > 0) {
		Pos -= 1;
		screen_refresh();
	} else {
		beep();
		flash();
	}
}

void screen_action_line_down() {
	int lines = Pos + LINES;
	lines = Mode? lines-1 : lines;
	if (lines < LINES*PAGES) {
		Pos += 1;
		screen_refresh();
	} else {
		beep();
		flash();
	}
}

void screen_action_page_up() {
	if (Pos - LINES > 0) {
		Pos -= LINES;
		screen_refresh();
	} else {
		beep();
		flash();
	}
}

void screen_action_page_down() {
	int lines = Pos + LINES;
	lines = Mode? lines-1 : lines;
	if (lines < LINES*PAGES) {
		Pos += LINES;
		screen_refresh();
	} else {
		beep();
		flash();
	}
}

void screen_action_scroll_top() {
	Pos = 0;
	screen_refresh();
}

void screen_action_scroll_end() {
	Pos = LINES * (PAGES - 1);
	screen_refresh();
}

void screen_action_normal_mode() {
	Mode = false;
	screen_refresh();
}

void screen_action_scroll_mode() {
	Mode = true;
	screen_refresh();	
}

void screen_action_help() {
	screen_printf("[Esc]       send escape to child\n"
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
			screen_printf("invalid key: %d\n"
						 "press [h] for help.\n",
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
