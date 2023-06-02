LIBS  = -lm -lncurses -lc
CFLAGS = -std=gnu11 -ggdb -O2 -Wall -Wextra 
#-Wno-unused-variable -Wno-unused-parameter -Wno-unused-but-set-variable

# Should be equivalent to your list of C files, if you don't build selectively
SRC=$(wildcard *.c)

all: $(SRC)
	clang $^ $(CFLAGS) $(LIBS)

run:
	make && ./a.out

clean:
	rm -rf ./a.out
