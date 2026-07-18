OUT_NAME := lockguy
VERSION := 1.0

CC := gcc

LIBS := x11 xft xext
CFLAGS := -Wall -Os -lm $(shell pkg-config --cflags --libs $(LIBS)) -DVERSION='"1.0"' $(CFLAGS)
SRC := src

FILES := $(SRC)/*.h $(SRC)/*.c
FLAGS_TXT_FILE := compile_flags.txt

.PHONY: build prepare test run test-notif cflags

build: prepare-ide
	$(CC) $(CFLAGS) $(FILES) -o $(OUT_PREFIX)$(OUT_NAME)

prepare: $(FLAGS_TXT_FILE)

test:
	CFLAGS="-DENABLE_TESTS" make build
	./$(OUT_NAME)

run: build
	./$(OUT_NAME)

test-notif: build
	notify-send "Pre notification"
	sh -c "(sleep 2 && notify-send aaaa)&"

	./$(OUT_NAME)

cflags:
	@echo $(CFLAGS)

clean:
	rm -f $(OUT_NAME)
	rm -f $(FLAGS_TXT_FILE)

$(FLAGS_TXT_FILE):
	@echo $(CFLAGS) | xargs -n1 > $(FLAGS_TXT_FILE)
	@echo Created "$(FLAGS_TXT_FILE)" for clangd. Restart your editor to fix include errors
