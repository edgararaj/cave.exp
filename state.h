#pragma once
#include "objects.h"
#include "camera.h"
#define MAX_CHESTS 100
#define ITEM_GOLD 1
#define ITEM_SWORD 2
#define ITEM_POTION 3


typedef struct
{
    Vec2i position;
    bool isOpen;
    int item;
} Chest;

typedef enum
{
    State_Game,
    State_Menu,
    State_Info,
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
} Mob;

typedef struct
{
    int hp;
    int maxHP;
    int mana;
    int maxMana;
    int level;
    int experience;
    int attackPower;
    int defense;
    float speed;
    int gold;
} Player_Stats;

typedef struct
{
    Rect position;
    int radius;
} Torch;

// Define the maximum number of items a player can carry
#define MAX_INVENTORY_SIZE 10

// Define the item type
typedef enum
{
    ITEM_TYPE_SWORD,
    ITEM_TYPE_BLASTGUN,
    ITEM_TYPE_HEALTH_POTION,
    ITEM_TYPE_MANA_POTION,
    ITEM_TYPE_COINS,
    ITEM_TYPE_KEY,
} ItemType;

// Define the item structure
typedef struct
{
    ItemType type;
    char *name;
    char symbol;
    int color;
} Item;

typedef struct Inventory
{
    Item *items;
    int *item_counts; // Array to store the count of each item
    int size;
    int capacity;
    int gold;
} Inventory;

typedef struct
{
    Camera camera;
    CameraMode cam_mode;
    Warrior player;
    Player_Stats player_stats;
    WINDOW *win_inventory;
    WINDOW *win_game;
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
