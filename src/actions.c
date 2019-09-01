// Ters actions
// Copyright (C) 2019 Mohammad Amin Mollazadeh

#include "ters.h"

/* actions */
void actions_line_up() {
	if (screen_scroll(-1)) {
		screen_refresh();
	} else {
		screen_beep();
	}
}

void actions_line_down() {
	if (screen_scroll(+1)) {
		screen_refresh();
	} else {
		screen_beep();
	}
}

void actions_page_up() {
	if (screen_scroll(0-screen_get_lines_in_page())) {
		screen_refresh();
	} else {
		actions_scroll_top();
	}
}

void actions_page_down() {
	if (screen_scroll(screen_get_lines_in_page()-1)) {
		screen_refresh();
	} else {
		actions_scroll_end();
	}
}

void actions_scroll_top() {
	if (screen_scroll_to(0)) {
		screen_refresh();
	} else {
		screen_beep();
	}
}

void actions_scroll_end() {
	if (screen_scroll_to(screen_get_line() - screen_get_lines_in_page() + 1)) {
		screen_refresh();
	} else {
		screen_beep();
	}
}

void actions_help() {
	screen_printf("[Esc]       send escape to child\n"
				  "[Up]        scroll up\n"
				  "[Down]      scroll down\n"
				  "[Page up]   scroll one page up\n"
				  "[Page down] scroll one page down\n"
				  "[r]         refresh screen (to show terminal again)\n"
				  "[q]         quit\n"
				  );
}