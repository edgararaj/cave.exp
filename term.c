#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "colors.h"
#include "utils.h"

#define TERM_BUFFER 5
int term_line_index = 0;
char term_lines[TERM_BUFFER][50] = {};
static int last_line_hightlight = 200;
static int hightlighting = 0;
void add_term_line(const char *format, ...)
{
    if (term_line_index == TERM_BUFFER - 1)
    {
        for (int i = 0; i < TERM_BUFFER - 1; i++)
        {
            strcpy(term_lines[i], term_lines[i + 1]);
        }
        term_line_index -= 1;
    }
    va_list va;
    va_start(va, format);
    vsnprintf(term_lines[term_line_index++], 50, format, va);
    va_end(va);
}

int is_last(int i)
{
    return (i == TERM_BUFFER - 1 || !*term_lines[i + 1]);
}

void render_term(WINDOW *win)
{
    for (int i = 0; i < TERM_BUFFER && *term_lines[i]; i++)
    {
        if (is_last(i) && i != hightlighting)
        {
            last_line_hightlight = 200;
            hightlighting = i;
        }

        if (is_last(i) && last_line_hightlight)
        {
            timer_update(&last_line_hightlight, 1);
            wattrset(win, COLOR_PAIR(Culur_Default_Green));
        }
        else
            wattrset(win, COLOR_PAIR(Culur_Default));
        mvwprintw(win, i, 0, "%s", term_lines[i]);
    }
}
