// the TERminal Scroller
// Copyright (C) 2019 By Mohammad Amin Mollazadeh


#include "ters.h"

#include <stdarg.h>
#include <curses.h>

int main() {
	printf("Terminal Scroller v0.1 alpha\n");
	printf("Copyright (C) 2019 By Mohammad Amin Mollazadeh\n\n");

	// initialize pty
	int pty_fd = pty_init();

    // initialize screen
    screen_init();

	// initialize event handler
	events_init(pty_fd);

	// start main loop
	events_loop();

	// exiting
	screen_close();
	close(pty_fd);
}

void panic(const char *error, ...) {
    screen_close();

	va_list ap;

	va_start(ap, error);
	vfprintf(stderr, error, ap);
	va_end(ap);
	exit(1);
}
