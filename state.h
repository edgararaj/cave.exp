#pragma once
#include "camera.h"
#include "inventory.h"
#include "light.h"
#include "mobs.h"
#include "objects.h"

typedef enum
{
    State_Game,
    State_Menu,
    State_Info,
} State;

typedef struct
{
    Camera camera;
    CameraMode cam_mode;
    Warrior player;
    WINDOW *win_inventory;
    WINDOW *win_game;
    Bitmap pixmap;
    Player_Stats player_stats;
    Torch *torches;
    Mob *mobs;
    Bitmap illuminated;
    Inventory inventory;
    int player_attacking;
    int minimap_maximized;
} GameState;

typedef struct
{
    WINDOW *win;
    int highlight;
} StartMenuState;
