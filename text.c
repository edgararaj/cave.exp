#include <ncurses.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

/*
 * Return 1 (true) if there are any unread characters, 0 (false) otherwise
 */
int kbhit(void) {
    struct timeval timeout;
    fd_set readfds;
    int how;

    /* look only at stdin (fd = 0) */
    FD_ZERO(&readfds);
    FD_SET(0, &readfds);

    /* poll: return immediately */
    timeout.tv_sec = 0L;
    timeout.tv_usec = 0L;

    how = select(1, &readfds, (fd_set *)NULL, (fd_set *)NULL, &timeout);
    /* Change "&timeout" above to "(struct timeval *)0" ^^^^^^^^
     * if you want to wait until a key is hit
     */

    if ((how > 0) && FD_ISSET(0, &readfds))
        return 1;
    else
        return 0;
}

int main(void) {
    initscr();

    cbreak();
    noecho();
    nodelay(stdscr, TRUE);

    scrollok(stdscr, TRUE);
    while (1) {
        if (kbhit()) {
            printw("Key pressed! It was: %d\n", getch());
            refresh();
        } else {
            printw("No key pressed yet...\n");
            refresh();
            sleep(1);
        }
    }
}
