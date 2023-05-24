#include <string.h>
#include "combat.h"
#include "player.h"
#include <ncurses.h>

void render_player_stats(WINDOW* terminalWin, Player_Stats player, Warrior warrior, Vec2i window_size) {

    // Criar janela do mini-terminal
    box(terminalWin, 0, 0);

    // Imprimir a vida do jogador na janela do mini-terminal
    char* title = "STATS";
    mvwprintw(terminalWin, 0, (window_size.x - strlen(title)) / 2, title);
    mvwprintw(terminalWin, 1, 1, "Vida: %d/%d", warrior.hp, warrior.maxHP);
    mvwprintw(terminalWin, 2, 1, "Mana: %d/%d", player.mana, player.maxMana);
    mvwprintw(terminalWin, 3, 1, "Level: %d", player.level);
    mvwprintw(terminalWin, 4, 1, "RBP: %d", player.rbp);
}
