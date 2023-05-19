#include<ncurses.h>
#include "state.h"
#



WINDOW *gameOverWin = newwin(8, 70, (LINES - 10) / 2, (COLS - 75) / 2);
            box(gameOverWin, 1, 0);
            refresh();
            mvwprintw(gameOverWin, 1, 29, "GAME OVER!!");
            wrefresh(gameOverWin);
            //delwin(gameOverWin);

            keypad(gameOverWin, true);

            char choices[3][30] = {"Try Again", "Go to the menu", "Exit/You got tilted..."};//9,14,22 

            int choice, highlight = 0;

            while (true)
            {
                int e, tempx;
                tempx = getmaxx(gameOverWin);

                for(int i = 0; i < 3; i++)
                {
                    if(i == 0)
                    {
                         e = 5;
                    } else {
                        if(i == 1)
                        {
                         e = 7;
                        } else {
                            e = 11;
                        }
                    }
                    if(i == highlight){
                        wattron(gameOverWin, A_REVERSE);
                        mvwprintw(gameOverWin, i + 3, tempx/2 - e, choices[i]);
                        wattroff(gameOverWin, A_REVERSE);
                    } else {
                        mvwprintw(gameOverWin, i + 3, tempx/2 - e, choices[i]);
                    }
                    wrefresh(gameOverWin);
                }
                choice = wgetch(gameOverWin);
switch(choice)
                {
                case KEY_UP:
                    highlight--;
                    break;
                case KEY_DOWN:
                    highlight++;
                    break;
                default:
                    break;
                }
                if(choice == 10)
                {
                    break;
                }
            }

            while (true) {
                int gameOverKey = wgetch(gameWin);
                if (gameOverKey == 'q') 
                    gameOver = true;
                    break;

            }
        
