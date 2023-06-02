#pragma once
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "state.h"
#include "utils.h"
#include "colors.h"
#include "player.h"

void draw_game_over(GameOverState *gos, State *state, int choice, Vec2i window_size);
