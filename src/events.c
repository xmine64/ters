// Ters event manager
// Copyright (C) 2019 By Mohammad Amin Mollazadeh

#include "ters.h"

#include <unistd.h>
#include <sys/ioctl.h>

/* including libevent2 */
#include <event2/event.h>
#include <event2/buffer.h>

// event base keeps track of events
static struct event_base *ev_base;
// the event of receiving data through pty
static struct event *ev_pty;
// the event of receiving data from standard input
static struct event *ev_stdin;

void events_callback_pty(evutil_socket_t fd, short ev, void * arg) {
	// count pending data on pty
	int count;
	ioctl(fd, FIONREAD, &count);
	debug_printf("[event] PTY: %d bytes received.\n", count);
	if (count > 0) {
		// allocate memory for it and read data
		u_char *buf = malloc(count);
	    read(fd, buf, count);
    	vt_print_buffer(buf, count);
	    free(buf);
    }
}

// convert array of 8 chars (64-bit) to long (64-bit)
// for converting key buffer to key code
long kb_to_kc(u_char *buffer, int count) {
	u_char buf[] = {0, 0, 0, 0, 0, 0, 0, 0};
	memcpy(buf, buffer, count);
	long keycode = *(long*)buf;
	return keycode;
}

void events_callback_stdin(evutil_socket_t fd, short ev, void * arg) {
    u_char buf[8];
    int count = read(fd, buf, 8);
    int key = kb_to_kc(buf, count);
    if (count > 0)
        input_handler_handle(key);
}

void events_init(int pty_fd) {
    ev_base = event_base_new();

    ev_pty = event_new(ev_base, pty_fd, EV_READ | EV_PERSIST, events_callback_pty, NULL);
    if (event_add(ev_pty, NULL) != 0)
            panic("[PANIC] event_add(ev_pty, NULL) failed.\n");
            
    ev_stdin = event_new(ev_base, STDIN_FILENO, EV_READ | EV_PERSIST, events_callback_stdin, NULL);
    if (event_add(ev_stdin, NULL) != 0)
        panic("[PANIC] event_add(ev_stdin, NULL) failed.\n");
}

void events_loop_start() {
    event_base_loop(ev_base, EVLOOP_NO_EXIT_ON_EMPTY);
}

void events_loop_stop() {
    event_base_loopexit(ev_base, NULL);
}
