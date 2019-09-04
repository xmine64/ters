// Ters screen manager
// Copyright (C) 2019 By Mohammad Amin Mollazadeh

#include "ters.h"
#include "vt.h"

#include <locale.h>
#include <curses.h>

static WINDOW *Pad;

static WINDOW *Stdout;
static WINDOW *Debug;

static WINDOW *Status;
static WINDOW *Cursor;
static WINDOW *Popup = NULL;

static int PadCols = 0;
static int PadLines = 0;

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
   	waddch(Cursor, ACS_CKBOARD);

	PadLines = LINES*PAGES;
	PadCols = COLS;
   	Stdout = newpad(PadLines, PadCols);

   	Debug = newpad(PadLines, PadCols);

   	Pad = Stdout;
}

void screen_close() {
	if (screen_is_popup()) {
		delwin(Popup);
	}
	delwin(Stdout);
	delwin(Debug);
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

	wclear(Stdout);
	
    screen_refresh();
}

void screen_beep() {
	beep();
	flash();
}

void screen_printf(const char *message, ...) {
    va_list ap;

    va_start(ap, message);
    vwprintw(Stdout, message, ap);
    va_end(ap);
}

void debug_printf(const char *message, ...) {
    va_list ap;

    va_start(ap, message);
    vwprintw(Debug, message, ap);
    va_end(ap);
}

void screen_debug(bool enabled) {
	if (enabled) {
		Pad = Debug;
		screen_scroll_to(0);
	} else {
		Pad = Stdout;
		screen_scroll_to(0);
	}
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
	return getcury(Stdout);
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


/////////////////////////////////////
#define CURRENT_PAGE ((getcury(Pad)/LINES)*LINES)

void screen_addch(u_char c) {
	waddch(Stdout, c);
}

int screen_get_y() {
	int result = getcury(Stdout)%LINES;
	debug_printf("[screen] screen_get_y(): %d \n", result);
	return result;
}

int screen_get_x() {
	int result = getcurx(Stdout);
	debug_printf("[screen] screen_get_x(): %d \n", result);
	return result;
}

void screen_set_y(int y) {
	int pady = ((getcury(Stdout)/LINES)*LINES)+y;
	int padx = getcurx(Stdout);
	debug_printf("[screen] screen_set_y(%d) -> move(%d, %d) \n", y, pady, padx);
	wmove(Stdout, pady, padx);
}

void screen_set_x(int x) {
	int y = getcury(Stdout);
	debug_printf("[screen] screen_set_x(%d) -> move(%d, %d) \n", y, y, x);
	wmove(Stdout, y, x);
}

void screen_vt_H(int y, int x) {
	debug_printf("[screen] screen_vt_H(%d, %d), CURRENT_PAGE=%d\n", y, x, CURRENT_PAGE);
	if (y < 0 && x < 0) return;
	if (y*COLS >= PadLines*PadCols) return;
	if (x >= COLS) return;
	wmove(Stdout, CURRENT_PAGE+y, x);
}

void screen_vt_cr() {
    wmove(Stdout, getcury(Stdout), 0);
}

void screen_vt_lf() {
    wmove(Stdout, getcury(Stdout)+1, getcurx(Stdout));
}

void screen_vt_bs() {
	screen_addch(BS);
}

void screen_vt_vtab() {
	screen_addch(VT);
}

void screen_vt_htab() {
	screen_addch(HT);
}

void screen_vt_sp() {
	screen_addch(SP);
}

void screen_vt_del() {
	screen_addch(DEL);
}

void screen_vt_B(int lines) {
	screen_vt_H(screen_get_y() + lines, screen_get_x());
}

void screen_vt_A(int lines) {
	screen_vt_B(0-lines);
}

void screen_vt_C(int cols) {
	screen_vt_H(screen_get_y(), screen_get_x() + cols);
}

void screen_vt_D(int cols) {
	screen_vt_C(0-cols);
}

void screen_vt_K() {
	int y = getcury(Stdout);
	int x0 = getcurx(Stdout);
	for (int x = x0; x < COLS; x++) {
		mvwaddch(Stdout, y, x, ' ');
	}
	wmove(Stdout, y, x0);
}

void screen_vt_J() {
	for (int i=0; i < COLS*LINES; i++) {
		screen_addch(' ');
	}
}

