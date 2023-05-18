#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TERM_BUFFER 40
int term_line_index = 0;
char term_lines[TERM_BUFFER][50] = {};
void add_term_line(const char *format, ...) {
    if (term_line_index == TERM_BUFFER - 1) {
        for (int i = 0; i < TERM_BUFFER - 1; i++) {
            strcpy(term_lines[i], term_lines[i + 1]);
        }
        term_line_index -= 1;
    }
    va_list va;
    va_start(va, format);
    const int ret = vsnprintf(term_lines[term_line_index++], 50, format, va);
    va_end(va);
}

void render_term(WINDOW *win) {
    for (int i = 0; i < TERM_BUFFER && *term_lines[i]; i++) {
        mvwprintw(win, i + 1, 1, term_lines[i]);
    }
}
