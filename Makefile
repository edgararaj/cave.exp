all:
	gcc -ggdb -O0 -std=gnu11 -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter -Wno-unused-but-set-variable -lm -lncurses main.c

run:
	make && ./a.out --setup && xterm -e ./a.out
