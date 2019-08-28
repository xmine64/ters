#!/usr/bin/fish

echo "Building TERS..."

gcc src/*.c -o ters -lutil -lncursesw -levent_core
