// Ters event manager
// Copyright (C) 2019 By Mohammad Amin Mollazadeh

#include "ters.h"

/* including libevent2 */
#include <event2/event.h>
#include <event2/buffer.h>

// event base keeps track of events
static struct event_base *ev_base;
// the event of receiving data through pty
static struct event *ev_pty;
// the event of receiving data from standard input
static struct event *ev_stdin;

// callback function for ev_pty
void events_callback_pty(evutil_socket_t fd, short ev, void * arg) {
    char buf[300];
    int count = read(fd, buf, 300);
    if (count > 0) {
        buffer_append(buf, count);
        screen_refresh();
    }
}

// callback function for ev_stdin
void events_callback_stdin(evutil_socket_t fd, short ev, void * arg) {
    char buf[4];
    int count = read(fd, buf, 4);
    if (count > 0)
        screen_handle_user_input(buf, count);
}

// initialize event handler and create an event for pty_fd
void events_init(int pty_fd) {
    ev_base = event_base_new();
    
    ev_pty = event_new(ev_base, pty_fd, EV_READ | EV_PERSIST, events_callback_pty, NULL);
    if (event_add(ev_pty, NULL) != 0)
            panic("[PANIC] event_add(ev_pty, NULL) failed.\n");
            
    ev_stdin = event_new(ev_base, STDIN_FILENO, EV_READ | EV_PERSIST, events_callback_stdin, NULL);
    if (event_add(ev_stdin, NULL) != 0)
        panic("[PANIC] event_add(ev_stdin, NULL) failed.\n");
}

void events_loop() {
    event_base_loop(ev_base, EVLOOP_NO_EXIT_ON_EMPTY);
}

void events_stop() {
    event_base_loopexit(ev_base, NULL);
}
