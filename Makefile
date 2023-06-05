LIBS  = -lm -lncurses -lc
CFLAGS = -std=gnu11 -ggdb -Wall -Wextra -pedantic -O0
#-Wno-unused-variable -Wno-unused-parameter -Wno-unused-but-set-variable

.PHONY: all sdl run clean ncurses

# Should be equivalent to your list of C files, if you don't build selectively
SRC=$(wildcard *.c)

all: $(SRC)
	clang $^ $(CFLAGS) $(LIBS)

run:
	make && ./a.out

clean:
	rm -rf ./a.out
