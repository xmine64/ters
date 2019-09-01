// Ters screen manager
// Copyright (C) 2019 By Mohammad Amin Mollazadeh

#include "ters.h"

#include <locale.h>
#include <curses.h>

// the scrollable window
static WINDOW *Pad;

static WINDOW *Status;
static WINDOW *Cursor;
static WINDOW *Popup = NULL;

void screen_init() {
	// it's required to show box drawings correctly
	setlocale(LC_ALL, "");
	
    initscr();
    raw();
    noecho();
    if (has_colors()) {
	    use_default_colors();
	    start_color();
	    // color pair for popup windows
	    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    }
    curs_set(0); // hide terminal emulator's cursor

	Status = newwin(1, COLS-1, LINES-1, 0);
	wbkgd(Status, COLOR_PAIR(1));
	box(Status, 0, 0);

   	Cursor = newwin(1, 1, 0, 0);
   	waddch(Cursor, ACS_BOARD);

   	Pad = newpad(LINES*PAGES, COLS);

    wprintw(Pad, "Ters Terminal Scroller v0.1 (EXPERIMENTAL)\n"
    			 "Press [Esc] then [h] for help.\n\n");
}

void screen_close() {
	if (screen_is_popup()) {
		delwin(Popup);
	}
	delwin(Pad);
    endwin();
}

void screen_refresh() {
		if (screen_get_mode()) {
		// reset status window (prevent from breaking on terminal resize)
		wclear(Status);
		mvwin(Status, LINES-1, 0);
		wresize(Status, 1, COLS-1);
		
		// draw scroll indicator
    	mvwprintw(Status, 0, 0, "[ SCROLL ] Line: %d   ", screen_get_pos());
    	mvwprintw(Status, 0, COLS - 19, "Press [h] for help");

    	wrefresh(Status);
	} else {
		// update scroller position
		int new_pos = getcury(Pad) - LINES + 1;
		screen_scroll_to(new_pos < 0? 0 : new_pos);
		// move cursor
		int cury, curx;
		getyx(Pad, cury, curx);
		cury = cury > LINES? LINES-1 : cury;
		mvwin(Cursor, cury, curx);
	}

	// refresh Pad
	prefresh(Pad, screen_get_pos(), 0, 0, 0,
			 screen_get_lines_in_page()-1, COLS - 1);
	wrefresh(Cursor);
	
	// refresh Popup Window
	if (screen_is_popup()) {
		wrefresh(Popup);
	}
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

// print new data received from pty
void screen_print_buffer(u_char *buffer, int count) {
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

//////////////////////////////////////////

void screen_popup(int width, int height, char** lines) {
	int cols = width+2;
	int rows = height+2;
	int x = (COLS-cols+1)/2;
	int y = (LINES-rows+1)/2;

	if (cols > COLS) {
		cols = COLS-1;
		x = 0;
	}
	if (rows > LINES) {
		rows = LINES-1;
		y = 0;
	}

	Popup = newwin(rows, cols, y, x);

	for (int i=0; i < height; i++) {
		wmove(Popup, i+1, 1);
		wprintw(Popup, lines[i]);
	}

	wbkgd(Popup, COLOR_PAIR(1));
	box(Popup, 0, 0);
	
	screen_refresh();
}

bool screen_is_popup() {
	return Popup != NULL;
}

void screen_close_popup() {
	delwin(Popup);
	Popup = NULL;
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
	if (Pos == line) return false;
	if (line < 0 ||
		line+screen_get_lines_in_page() - 1 > getcury(Pad))
		return false;
	
	Pos = line;
	return true;
}
