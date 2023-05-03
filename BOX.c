#include <assert.h>
#include <ncurses.h>
#include <unistd.h>

int main()
{
    WINDOW *win, *swin;
    int lines, cols, y, x;

    initscr();
    keypad(stdscr, TRUE);
    noecho();

    // Create window
    lines = 10; cols  = 40;
    y = 5; x = 5;
    win = newwin(lines, cols, y, x);
    assert(win != NULL);

    // Create window border
    box(win, 0, 0);
    mvwprintw(win, 0, 2, " Window ");

    // Create subwindow
    swin = subwin(win, lines-2, cols-2, y+1, x+1);
    assert(swin != NULL);
    // Print window and subwindow y,x
    mvwprintw(swin, 0, 0, "win y,x=%d,%d  swin y,x=%d,%d\n",
        getbegy(win), getbegx(win), getbegy(swin), getbegx(swin));

    // Refresh
    wnoutrefresh(stdscr);
    wnoutrefresh(win);
    wnoutrefresh(swin);
    doupdate();

    sleep(2);

    // Move window
    y = 20; x = 40;
    mvwin(win, y, x);
    mvwprintw(swin, 0, 0, "win y,x=%d,%d  swin y,x=%d,%d\n",
        getbegy(win), getbegx(win), getbegy(swin), getbegx(swin));

    // Refresh
    wnoutrefresh(stdscr);
    wnoutrefresh(win);
    wnoutrefresh(swin);
    doupdate();

    wgetch(swin);

    endwin();
    return 0;
}