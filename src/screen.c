// Ters screen manager
// Copyright (C) 2019 By Mohammad Amin Mollazadeh

#include "ters.h"

#include <curses.h>

// the scrollable window
static WINDOW *Pad;

void screen_init() {
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);

    Pad = newpad(LINES*PAGES, COLS);

    screen_printf("Ters, The Terminal Scroller\n"
    			  "v0.1 alpha (EXPERIMENTAL)\n\n");
}

void screen_close() {
	delwin(Pad);
    endwin();
}

void screen_refresh() {
	if (screen_get_mode()) {
		// draw scroll indicator
    	mvprintw(LINES - 1, 0, "[ SCROLL ] Line: %d   ", screen_get_pos());
    	mvprintw(LINES - 1, COLS - 20, "Press [h] for help.");

    	// move cursor to correct place
    	int y, x;
    	getyx(Pad, y, x);
    	move(y > LINES? LINES-1 : y, x);
    	
    	refresh();
	} else {
		// update scroller position
		int new_pos = getcury(Pad) - LINES + 1;
		screen_scroll_to(new_pos > 0? new_pos : 0);

		// move cursor to bottom-right of screen
		move(LINES - 1, COLS - 1);
	}

	// refresh Pad
	prefresh(Pad, screen_get_pos(), 0, 0, 0,
			 screen_get_lines_in_page()-1, COLS - 1);
}

void screen_clear() {
	screen_scroll_to(0);

	wclear(Pad);
	
    screen_refresh();
}

void screen_beep() {
	beep();
	flash();
}

////////////////////////////////////////

void screen_printf(const char *str, ...) {
	va_list ap;
	va_start(ap, str);
	vwprintw(Pad, str, ap);
	va_end(ap);

	screen_refresh();
}

// print new data received from pty
void screen_print_buffer(char *buffer, int count) {
	for (int i=0; i < count; i++) {
		switch (buffer[i]) {
			// beep
			case '\a':
				screen_beep();
				break;
			// move cursor to start of line
			case '\r':
				wmove(Pad, getcury(Pad), 0);
				break;
			// move cursor to next line
			case '\n':
				wmove(Pad, getcury(Pad) + 1, getcurx(Pad));
				break;
			
			default:
				waddch(Pad, buffer[i]);
				break;
		}
	}
	
	screen_refresh();
}

////////////////////////////////////////

// 'Mode = true' is scroll mode that user can scroll
// 'Mode = false' is normal mode that input will send to child
static bool Mode = false;

void screen_set_mode(bool mode) {
	Mode = mode;

	if (Mode) screen_scroll(1);
	
	screen_refresh();
}

bool screen_get_mode() {
	return Mode;
}

//////////////////////////////////////

int screen_get_lines_in_page() {
	return Mode? LINES-1 : LINES;
}

int screen_get_line() {
	return getcury(Pad);
}

//////////////////////////////////////

// position of scroller
static int Pos = 0;

int screen_get_pos() {
	return Pos;
}

bool screen_scroll(int lines) {
	int new_pos = Pos + lines;
	return screen_scroll_to(new_pos);
}

bool screen_scroll_to(int line) {
	if (line < 0 ||
		line+screen_get_lines_in_page() - 1 > getcury(Pad))
			return false;
			
	Pos = line;
	return true;
}
