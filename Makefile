all:
	gcc main.c -ggdb -O0 -std=gnu11 -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter -Wno-unused-but-set-variable -lm -lncurses

run:
	make && ./a.out --setup && xterm -e ./a.out
