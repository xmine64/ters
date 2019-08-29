#!/usr/bin/sh

# Simple script to build Ters

echo "Building Ters..."

gcc src/*.c -o ters -lutil -lncursesw -levent_core
