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
	char *body[] = {
		"Ters, the Terminal Scroller",
		"",
		"Keymaps in scroll mode:",
		" [Enter]       back to normal mode",
		" [Esc]         send escape to child",
		" [Up]          scroll up",
		" [Down]        scroll down",
		" [Page up]     scroll one page up",
		" [Page down]   scroll one page down",
		" [r]           refresh screen",
		" [d]           show debug output",
		" [n]           show back to normal",
		" [q]           quit",
		"",
		"",
		"Press any key to close..."
	};
	screen_popup(50, 16, body);
}
