#pragma once
#include "objects.h"
#include "camera.h"
#include <ncurses.h>

#define MAX_CHESTS 100
#define MAX_CHESTS_PER_ROOM 2

// Define the maximum number of items a player can carry
#define MAX_INVENTORY_SIZE 10

// Define the item type
typedef enum
{
    Item_ManaPotion,
    Item_HealthPotion,
    Item__Size,
    Item_Key,
    Item_Sword,
    Item_BlastGun,
} Item;

typedef enum {
    ItemType_Disposable,
    ItemType_Weapon
} ItemType;

typedef struct {
    ItemType type;
    int count;
    Item name;
    float cooldown;
} ItemSlot;

typedef struct
{
    ItemSlot items[MAX_INVENTORY_SIZE];
    int size;
    int selected_item;
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
    int attacking;
    Hotbar hotbar;
} Warrior;

#define MAX_MOBS 20

typedef enum
{
    MobType_Stupid,
    MobType_Coward,
    MobType_Intelligent,
    MobType_Archer,
    MobType__Size
} MobType;

typedef struct
{
    float speed;
    MobType type;
    Warrior warrior;
    int called;
    Vec2i wander_to;
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

typedef enum
{
    LightType_Vision,
    LightType_Torch
} LightType;

typedef struct
{
    Rect position;
    int radius;
} Torch;

typedef struct {
    Rect rect;
    int radius;
    Vec2f velocity;
} Star;

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
    Bitmap collision;
    Bitmap distance;
    Bitmap light;
    Torch *torches;
    Chest* chests;
    Rect portal;
    int chests_count;
    Mob *mobs;
    Bitmap illuminated;
    Inventory inventory;
    int minimap_maximized;
    int player_spike_damage_cooldown;
    int chestCount;
    Arrow* arrows;
    int arrow_count;

    int minimap_height;
    int sidebar_width;
    int player_stats_height;
    int inventory_height;
} GameState;

typedef struct
{
    WINDOW *win;
    int highlight;
    Star *stars;  // array of Star
    int num_stars; // size of the array
    Bitmap star_map; // bitmap for the stars
} StartMenuState;

typedef struct {
    WINDOW *win;
    int highlight;
} StartNiveisState;

typedef struct {
    WINDOW *win;
    int highlight;
} StartPauseState;
