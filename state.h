#pragma once
#include "camera.h"
#include "inventory.h"
#include "light.h"
#include "mobs.h"
#include "objects.h"

#define MAX_CHESTS 100

typedef struct
{
    Vec2i position;
    int isOpened;
    int item;
} Chest;

typedef enum
{
    State_Game,
    State_Menu,
    State_Controlos,
    State_Niveis,
    State_Info,
} State;

typedef struct
{
    Camera camera;
    CameraMode cam_mode;
    Warrior player;
    Player_Stats player_stats;
    WINDOW *win_inventory;
    WINDOW *win_game;
    WINDOW *vida;
    Bitmap pixmap;
    Torch *torches;
    Mob *mobs;
    Bitmap illuminated;
    Inventory inventory;
    int player_attacking;
    int minimap_maximized;
    int player_spike_damage_cooldown;
    Chest chests[MAX_CHESTS];
    int chestCount;
} GameState;

typedef struct
{
    WINDOW *win;
    int highlight;
} StartMenuState;

typedef struct
{
    WINDOW *win;
    int highlight;
} StartNiveisState;
