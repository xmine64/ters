// Pseudo Terminal utils
// Copyright (C) 2019 By Mohammad Amin Mollazadeh

#include "ters.h"

#include <errno.h>

#include <pty.h>
#include <sys/wait.h>

//#include <sys/ioctl.h>
//#include <ctype.h>
//#include <signal.h>
//#include <termios.h>
//#include <curses.h>

static pid_t ChildPid;
static int MasterFd = -1;
static int SlaveFd = -1;

// gets execute when child gets terminated
void sigchld(int a) {
    int stat;
	pid_t p;

	if ((p = waitpid(ChildPid, &stat, WNOHANG)) < 0)
		panic("[PANIC] waiting for pid %hd failed: %s\n", ChildPid, strerror(errno));

	if (ChildPid != p)
		return;

	if (WIFEXITED(stat) && WEXITSTATUS(stat))
		panic("[SIGCHLD] child exited with status %d\n", WEXITSTATUS(stat));
	else if (WIFSIGNALED(stat))
		panic("[SIGCHLD] child terminated due to signal %d\n", WTERMSIG(stat));

	events_stop();
}

// the forked process that starts shell
void child_process() {
	// close standard input/output
	close(STDOUT_FILENO);
			
	// create new process group
	setsid();

	// duplicate PTY fd to standard I/O fds
  	dup2(SlaveFd, STDIN_FILENO);
    dup2(SlaveFd, STDOUT_FILENO);
    dup2(SlaveFd, STDERR_FILENO);

	// I don't know what does it do but it's required to /bin/sh work well.
    if (ioctl(SlaveFd, TIOCSCTTY, NULL) < 0)
	    panic("[PANIC] ioctl TIOCSCTTY: %s\n", strerror(errno));

	// after duplicating them we don't need them anymore.
    close(SlaveFd);
    close(MasterFd);

	// setting up signals
    signal(SIGCHLD, SIG_DFL);
    signal(SIGHUP, SIG_DFL);
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
    signal(SIGALRM, SIG_DFL);

	// setting up environmental variables
    unsetenv("COLUMNS");
    unsetenv("LINES");
    unsetenv("TERMCAP");
    unsetenv("TERM");
	
    // TODO: Parse passwd
    //setenv("LOGNAME", pw->pw_name, 1);
    //setenv("USER", pw->pw_name, 1);
    //setenv("SHELL", sh, 1);
    //setenv("HOME", pw->pw_dir, 1);
	
    // TODO: set TERM
    //setenv("TERM", "xterm", 1);
	
    // print some helpful texts
    printf("The TERminal Scroller\n"
           "version 0.1 alpha EXPERIMENTAL\n"
           "Copyright (c) 2019 Mohammad Amin Mollazadeh\n"
           "Press [Esc] then [h] for help.\n\n");
	
    // start shell
    char *const shell_args[] = { "/bin/sh", "-", NULL };
    execv(shell_args[0], shell_args);
	
    // reaching here means there was an error in starting shell
    fprintf(stderr, "[PANIC] failed to start shell.\n");
    exit(1);
}

// initialize PTY
int pty_init() {
	// create a pty
	if (openpty(&MasterFd, &SlaveFd, NULL, NULL, NULL) < 0)
		panic("[PANIC] openpty: %s\n", strerror(errno));

	// fork and run shell
    ChildPid = fork();
    switch (ChildPid) {
        case -1:
            panic("[PANIC] failed to fork: %s\n", strerror(errno));
            return -1;

        // child
        case 0:
    		child_process();
		    return -1;

		// parent
        default:
            printf("[pty_new] slave=%d, master=%d, child_pid=%d\n", SlaveFd, MasterFd, ChildPid);

			// signal when child process terminated
            signal(SIGCHLD, sigchld);

			// close slave, it's for child
		    close(SlaveFd);
		    
		    return MasterFd;
	}
}

void pty_send(char *buffer, int count) {
	// write buffer to be send to master side of pty
    write(MasterFd, buffer, count);
}
