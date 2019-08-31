// Ters the Terminal Scroller
// Copyright (C) 2019 By Mohammad Amin Mollazadeh

#include "ters.h"

#include <unistd.h>
#include <stdarg.h>

static int PtyFd = -1;

static int ResourceFreed = false;

void free_resources() {
	if (!ResourceFreed) {
	    screen_close();
	    close(PtyFd);
	    ResourceFreed = true;
    }
    printf("free_resources() called.\n");
}

int main() {
	printf("Terminal Scroller v0.1 alpha\n"
		   "Copyright (C) 2019 By Mohammad Amin Mollazadeh\n\n");

	// initialize pty
	PtyFd = pty_init();

    // initialize screen
    screen_init();

    // close resources on exit
    atexit(free_resources);

	// initialize event handler
	events_init(PtyFd);

	// start main loop
	events_loop_start();
}

// close resource and show error message when an unexpected error occurs.
void panic(const char *error, ...) {
	free_resources();

	va_list ap;

	va_start(ap, error);
	vfprintf(stderr, error, ap);
	va_end(ap);
	
	exit(1);
}
