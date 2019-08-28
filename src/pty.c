// Pseudo Terminal utils
// Copyright (C) 2019 By Mohammad Amin Mollazadeh

#include "ters.h"

#include <sys/wait.h>
#include <sys/ioctl.h>

#include <ctype.h>
#include <errno.h>
#include <signal.h>

#include <termios.h>
#include <pty.h>

#include <curses.h>

static pid_t child_pid;
static int master_fd = -1;

void sigchld(int a) {
    int stat;
	pid_t p;

	if ((p = waitpid(child_pid, &stat, WNOHANG)) < 0)
		panic("[PANIC] waiting for pid %hd failed: %s\n", child_pid, strerror(errno));

	if (child_pid != p)
		return;

	if (WIFEXITED(stat) && WEXITSTATUS(stat))
		panic("[SIGCHLD] child exited with status %d\n", WEXITSTATUS(stat));
	else if (WIFSIGNALED(stat))
		panic("[SIGCHLD] child terminated due to signal %d\n", WTERMSIG(stat));

	events_stop();
}

int pty_init() {
	// create new pty
	int master, slave;

	if (openpty(&master, &slave, NULL, NULL, NULL) < 0)
		panic("[PANIC] openpty: %s\n", strerror(errno));

	// fork and run shell
    child_pid = fork();
    switch (child_pid) {
        case -1:
            panic("[PANIC] failed to fork: %s\n", strerror(errno));
            return -1;

        // child
        case 0:
    		close(STDOUT_FILENO);
		
    		// create new process group
    		setsid();
		
	    	dup2(slave, 0);
		    dup2(slave, 1);
		    dup2(slave, 2);
		
		    if (ioctl(slave, TIOCSCTTY, NULL) < 0)
			    panic("[PANIC] ioctl TIOCSCTTY: %s\n", strerror(errno));
		
		    close(slave);
		    close(master);

            signal(SIGCHLD, SIG_DFL);
            signal(SIGHUP, SIG_DFL);
            signal(SIGINT, SIG_DFL);
            signal(SIGQUIT, SIG_DFL);
            signal(SIGTERM, SIG_DFL);
            signal(SIGALRM, SIG_DFL);

            unsetenv("COLUMNS");
            unsetenv("LINES");
            unsetenv("TERMCAP");
            unsetenv("TERM");

            // TODO: Parse pw
            //setenv("LOGNAME", pw->pw_name, 1);
            //setenv("USER", pw->pw_name, 1);
            //setenv("SHELL", sh, 1);
            //setenv("HOME", pw->pw_dir, 1);

            // TODO: set TERM
            //setenv("TERM", "xterm", 1);

            signal(SIGCHLD, SIG_DFL);
            signal(SIGHUP, SIG_DFL);
            signal(SIGINT, SIG_DFL);
            signal(SIGQUIT, SIG_DFL);
            signal(SIGTERM, SIG_DFL);
            signal(SIGALRM, SIG_DFL);

            // print some helpful texts
            printf("The TERminal Scroller\n"
                   "Version 0.1 alpha EXPERIMENTAL\n"
                   "Copyright (c) 2019 Mohammad Amin Mollazadeh\n"
                   "Press [Esc] to enter scroll mode.\n"
                   "in scroll mode, use Up/Down arrow keys to scroll,"
                   "[q] to quit scroller, and [Enter] to back normal mode.\n\n");

            // start shell
		    char *const shell_args[] = { "/bin/sh", "-", NULL };
		    execv(shell_args[0], shell_args);

		    // reaching here means there was an error in starting shell
		    panic("[PANIC] failed to start shell.\n");
		    return -1;

		// parent
        default:
            printf("[pty_new] slave=%d, master=%d, child_pid=%d\n", slave, master, child_pid);

            signal(SIGCHLD, sigchld);

		    close(slave);

		    master_fd = master;
		    return master;
	}
}

void pty_send(char *buffer, int count) {
    write(master_fd, buffer, count);
}
