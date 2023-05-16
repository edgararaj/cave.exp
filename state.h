#pragma once
#include "camera.h"
#include "objects.h"
#include <ncurses.h>

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
    Rect player;
    WINDOW *win_inventory;
    WINDOW *win_game;
    Bitmap pixmap;
    Rect *torches;
} GameState;

typedef struct
{
    WINDOW *win;
    int highlight;
} StartMenuState;


