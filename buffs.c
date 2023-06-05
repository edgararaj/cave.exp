#include <stdlib.h>

#include "state.h"
#include "term.h"

BuffType buffs[2];

void generate_random_buffs(void) {
    buffs[0] = rand() % 5;
    do {
        buffs[1] = rand() % 5;
    } while (buffs[0] == buffs[1]);  // garantir que os dois buffs sejam diferentes
}

void apply_buffs(GameState *gs) {
    for (int i = 0; i < 2; i++) {
        switch (buffs[i]) {
            case BUFF_INCREASE_MOBS_HEALTH:
                // Aumenta a saúde dos mobs em 10%
                add_term_line("Mobs Health Increased!\n");
                for (int j = 0; j < MAX_MOBS; j++) {
                    gs->mobs[j].warrior.maxHP = gs->mobs[j].warrior.maxHP * 1.1;
                    gs->mobs[j].warrior.hp = gs->mobs[j].warrior.hp * 1.1;
                }
                break;
            case BUFF_INCREASE_MOBS_DAMAGE:
                // Aumenta o dano dos mobs em 10%
                for (int j = 0; j < MAX_MOBS; j++) {
                    add_term_line("Mobs Damage Increase!\n");
                    gs->mobs[j].warrior.dmg = gs->mobs[j].warrior.dmg * 1.1;
                }
                break;
            case BUFF_INCREASE_MAX_MOBS:
                // Aumenta o número máximo de mobs 
                add_term_line("Max Mobs Increase!\n");
                break;
            case BUFF_INCREASE_MOBS_SPEED:
                // Aumenta a velocidade dos mobs em 10%
                for (int j = 0; j < MAX_MOBS; j++) {
                    add_term_line("Mobs Speed Increase!\n");
                    gs->mobs[j].speed = gs->mobs[j].speed * 1.1;
                }
                break;
            case BUFF_INCREASE_ROOMS:
                // Aumenta a quantidade de salas que podem ser geradas 
                add_term_line("Max Rooms Increase!\n");
                break;
        }
    }
}
