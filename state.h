#pragma once
#include "objects.h"
#include "camera.h"
#define MAX_CHESTS 100
#define MAX_CHESTS_PER_ROOM 2

#define ITEM_GOLD 1
#define ITEM_SWORD 2
#define ITEM_POTION 3

// Define the maximum number of items a player can carry
#define MAX_INVENTORY_SIZE 10

// Define the item type
typedef enum
{
    ItemType_HealthPotion,
    ItemType__Size
} ItemType;

typedef struct
{
    ItemType items[MAX_INVENTORY_SIZE];
    int items_counts[MAX_INVENTORY_SIZE];
    int size;
} Inventory;

typedef struct {
    Inventory inventory;
    Rect rect;
    int is_open;
} Chest;

typedef enum
{
    State_Game,
    State_Menu,
    State_Controlos,
    State_Niveis,
    State_Info,
    State_Pause,
    State_New_Game
} State;


typedef struct
{
    int type;
    int count;
} HotbarItem;


#define HOTBAR_SIZE 9

typedef struct
{
    HotbarItem items[HOTBAR_SIZE]; // The items in the hotbar
    int selected;                  // The currently selected item
} Hotbar;

typedef struct
{
    RectFloat rect;
    Vec2f velocity;
    int hp;
    int maxHP;
    int dmg;
    int weight;
    int kills;
    int dmg_cooldown;
    Hotbar hotbar;
} Warrior;

#define MAX_MOBS 10

typedef enum
{
    MobType_Stupid,
    MobType_Coward,
    MobType_Intelligent,
    MobType__Size
} MobType;

typedef struct
{
    float speed;
    MobType type;
    Warrior warrior;
    int called;
} Mob;

typedef struct
{
    int hp;
    int mana;
    int maxMana;
    int level;
    int defense;
    int rbp;
} Player_Stats;

typedef struct
{
    Rect position;
    int radius;
} Torch;

typedef struct
{
    Camera camera;
    CameraMode cam_mode;
    Warrior player;
    Player_Stats player_stats;
    WINDOW *win_inventory;
    WINDOW *win_game;
    WINDOW *win_log;
    WINDOW *win_minimap;
    WINDOW *win_stats;
    Bitmap pixmap;
    Torch *torches;
    Chest* chests;
    int chests_count;
    Mob *mobs;
    Bitmap illuminated;
    Inventory inventory;
    int player_attacking;
    int minimap_maximized;
    int player_spike_damage_cooldown;
    int chestCount;
} GameState;

typedef struct
{
    WINDOW *win;
    int highlight;
} StartMenuState;

typedef struct {
    WINDOW *win;
    int highlight;
} StartNiveisState;

typedef struct {
    WINDOW *win;
    int highlight;
} StartPauseState;