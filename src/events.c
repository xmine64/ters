// ters event manager
// Copyright (C) 2019 By Mohammad Amin Mollazadeh

#include "ters.h"

#include <event2/event.h>
#include <event2/buffer.h>

static struct event_base *ev_base;
static struct event *ev_pty;
static struct event *ev_stdin;

void events_callback_pty(evutil_socket_t fd, short ev, void * arg) {
    char buf[300];
    int count = read(fd, buf, 300);
    if (count > 0) {
        buffer_append(buf, count);
        screen_refresh();
    }
}

void events_callback_stdin(evutil_socket_t fd, short ev, void * arg) {
    char buf[10];
    int count = read(fd, buf, 10);
    if (count > 0)
        screen_handle_user_input(buf, count);
}

void events_init(int pty_fd) {
    ev_base = event_base_new();
    ev_pty = event_new(ev_base, pty_fd, EV_READ | EV_PERSIST, events_callback_pty, NULL);
    ev_stdin = event_new(ev_base, STDIN_FILENO, EV_READ | EV_PERSIST, events_callback_stdin, NULL);
    if (event_add(ev_stdin, NULL) != 0)
        panic("[PANIC] event_add(ev_stdin, NULL) failed.\n");
    if (event_add(ev_pty, NULL) != 0)
        panic("[PANIC] event_add(ev_pty, NULL) failed.\n");
}

void events_loop() {
    event_base_loop(ev_base, EVLOOP_NO_EXIT_ON_EMPTY);
}

void events_stop() {
    event_base_loopexit(ev_base, NULL);
}
