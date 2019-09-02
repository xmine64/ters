# Ters Makefile
# By Mohammad Amin Mollazadeh

# C Compiler
CC = gcc
# Linker
LD = gcc
# Compiler Arguments
CFLAGS = 
# Linker Arguments
LDFLAGS =

# Install root
DESTDIR = /
# Install prefix
PREFIX = usr

# libraries to be linked
LIBRARIES = util ncurses event_core

SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, build/obj/%.o, $(SRC))

LDFLAGS2 = $(patsubst %, -l%, $(LIBRARIES)) $(LDFLAGS)

all: build/ters

# link
build/ters: $(OBJ)
	$(LD) -o $@ $(OBJ) $(LDFLAGS2)

# compile
build/obj/%.o: src/%.c
	$(CC) -g -c $< -o $@

clean:
	rm -rf build/ters $(OBJ)

install: ters
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f ters $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/ters

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/ters
