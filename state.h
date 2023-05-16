#pragma once
#include "camera.h"
#include "objects.h"
#include "mobs.h"
#include "light.h"
#include <ncurses.h>

typedef enum {
    State_Game,
    State_Menu
} State;

typedef struct
{
    Camera camera;
    CameraMode cam_mode;
    Rect player;
    WINDOW *win_inventory;
    WINDOW *win_game;
    Bitmap pixmap;
    Torch *torches;
    Mob *mobs;
    Bitmap illuminated;
    Inventory inventory;
} GameState;

typedef struct
{
    WINDOW *win;
    int highlight;
} StartMenuState;
