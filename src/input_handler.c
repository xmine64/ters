// Ters input handler
// Copyright (C) 2019 Mohammad Amin Mollazadeh

#include "ters.h"

void invalid_key_error(long keycode) {
	char error[30];
	sprintf(error, "invalid key: %ld", keycode);
	char *body[] = {
		error,
		"",
		"press [h] for help."
	};
	screen_popup(30, 3, body);
}

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
		case KC_UP_1:
			actions_line_up();
			break;
		case KC_DOWN:
		case KC_DOWN_1:
			actions_line_down();
			break;
			
		case KC_PAGEUP:
			actions_page_up();
			break;
		case KC_PAGEDOWN:
			actions_page_down();
			break;

		case KC_HOME:
		case KC_HOME_1:
			actions_scroll_top();
			break;
		case KC_END:
		case KC_END_1:
			actions_scroll_end();
			break;
			
		case 'h':
			actions_help();
			break;
			
		case 'r':
			screen_refresh();
			break;

		case 'd':
			screen_debug(true);
			screen_refresh();
			break;

		case 'n':
			screen_debug(false);
			screen_refresh();
			break;
			
		// quit
		case 'q':
			events_loop_stop();
			break;

		default:
			debug_printf("[input handler] invalid key press: %d\n", keycode);
			invalid_key_error(keycode);
			break;
	}
}

void input_handler_handle(long keycode) {
	if (screen_is_popup()) {
		screen_close_popup();
		return;
	}
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
