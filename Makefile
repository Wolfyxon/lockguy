OUT_NAME := lockguy
VERSION := 1.0

CC := gcc
CFLAGS := -Wall -Os -lX11 -lcrypt -DVERSION='"1.0"' $(CFLAGS)
SRC := src

.PHONY: build clean run test-notif

build:
	$(CC) $(CFLAGS) $(SRC)/*.h $(SRC)/*.c -o $(OUT_PREFIX)$(OUT_NAME)

run: build
	./$(OUT_NAME)

test-notif: build
	notify-send "Pre notification"
	sh -c "(sleep 2 && notify-send aaaa)&"

	./$(OUT_NAME)

clean:
	rm -f $(OUT_NAME)
