#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

#define MAP_WIDTH 80
#define MAP_HEIGHT 24
#define SPAWN_COUNT 14

void spawnCharacter(char map[MAP_HEIGHT][MAP_WIDTH], int x, int y, char character)
{
    map[y][x] = character;
}

void printMap(const char map[MAP_HEIGHT][MAP_WIDTH])
{
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            mvaddch(y, x, map[y][x]);
        }
    }

    refresh();
}

int main()
{
    srand(time(NULL));

    char map[MAP_HEIGHT][MAP_WIDTH];

    initscr(); // Inicializar a biblioteca ncurses
    curs_set(0); // Ocultar o cursor

    // Inicializar o mapa com espaços em branco
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            map[y][x] = ' ';
        }
    }

    // Gerar e fazer o spawn aleatório do caractere 'C' em 10 posições diferentes
    for (int i = 0; i < SPAWN_COUNT; i++)
    {
        int characterX = rand() % MAP_WIDTH;
        int characterY = rand() % MAP_HEIGHT;

        spawnCharacter(map, characterX, characterY, 'C');
    }

    // Exibir o mapa
    printMap(map);

    getch(); // Aguardar entrada do teclado antes de finalizar

    endwin(); // Encerrar a biblioteca ncurses

    return 0;
}
