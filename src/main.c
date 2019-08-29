// Ters the Terminal Scroller
// Copyright (C) 2019 By Mohammad Amin Mollazadeh

#include "ters.h"

#include <stdarg.h>

static int PtyFd = -1;

int main() {
	printf("Terminal Scroller v0.1 alpha\n"
		   "Copyright (C) 2019 By Mohammad Amin Mollazadeh\n\n");

	// initialize pty
	PtyFd = pty_init();

    // initialize screen
    screen_init();

	// initialize event handler
	events_init(PtyFd);

	// start main loop
	events_loop();

	// exiting
	screen_close();
	close(PtyFd);
}

// close resource and show error message when an unexpected error occurs.
void panic(const char *error, ...) {
    screen_close();
    close(PtyFd);

	va_list ap;

	va_start(ap, error);
	vfprintf(stderr, error, ap);
	va_end(ap);
	
	exit(1);
}
