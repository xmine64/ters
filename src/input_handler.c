// Ters input handler
// Copyright (C) 2019 Mohammad Amin Mollazadeh

#include "ters.h"

void handle_scroll_mode(long keycode) {
	switch (keycode) {
		// send [Esc] to running program, by double-pressing [Esc]
		case KC_ESCAPE:
			screen_set_mode(false);
			pty_send_keypress(KC_ESCAPE);
			break;
			
		case KC_ENTER:
			screen_set_mode(false);
			break;
			
		case KC_UP:
			actions_line_up();
			break;
		case KC_DOWN:
			actions_line_down();
			break;
			
		case KC_PAGEUP:
			actions_page_up();
			break;
		case KC_PAGEDOWN:
			actions_page_down();
			break;
			
		case KC_HOME:
			actions_scroll_top();
			break;
		case KC_END:
			actions_scroll_end();
			break;
			
		case 'h':
			actions_help();
			break;
			
		case 'r':
			screen_refresh();
			break;
			
		// quit
		case 'q':
			events_loop_stop();
			break;

		default:
			screen_printf("invalid key: %d\n"
						  "press [h] for help.\n",
						  keycode);
			break;
	}
}

void input_handler_handle(long keycode) {
	if (screen_get_mode()) {
		handle_scroll_mode(keycode);
	} else {
		if (keycode == KC_ESCAPE) {
			screen_set_mode(true);
		} else {
			pty_send_keypress(keycode);
		}
	}
}