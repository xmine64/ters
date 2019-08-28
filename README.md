# TERS (EXPERIMENTAL)
Ters is a simple terminal multiplexer for linux that allows you to scroll back.

It would be a suckless, light and fast program that keeps previous messages
in a stack-based buffer to show them back whenever user wants.

I want to run it in [st](https://st.suckless.org) to add scroll support to it.

# Usage
just run Ters, you will be granted with a welcome message and /bin/sh.
currently shell is a hardcoded value but I'll add configuration support later.
when Ters is running, press <kbd>Esc</kbd> to enter scroll mode.
then you'll be able to scroll by pressing Up/Down arrow keys.
by pressing <kbd>q</kbd> in scroll mode, you'll quit.
pressing <kbd>Enter</kbd> will bring back you to normal mode.

# Implemented Futures
* Simple support for PTY and starting a shell from a hardcoded path
* Simple scroll support

# TODO
* Make scroller stable (it's a little buggy, specially with `dmesg`)
* Support for user configurations
* ANSI Escape Sequences and Terminfo
* Scroll with mouse
* add some visual effects for scroll mode to find out is it scroll mode or normal mode
