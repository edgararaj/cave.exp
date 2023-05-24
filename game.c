#include "camera.h"
#include "collide.h"
#include "colors.h"
#include "combat.h"
#include "dist.h"
#include "draw.h"
#include "hud.h"
#include "inventory.h"
#include "items.h"
#include "light.h"
#include "map.h"
#include "mobs.h"
#include "objects.h"
#include "player.h"
#include "state.h"
#include "term.h"
#include "utils.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void init_game(GameState *gs, Rect window) {
    Rect rects[20];
    int rects_count = generate_rects(expand_rect(window, -5), rects, ARRAY_SIZE(rects));
    order_rects(rects, rects_count);

    Bitmap pixmap = alloc_bitmap(MAP_WIDTH, MAP_HEIGHT);
    generate_tunnels_and_rasterize(pixmap, rects, rects_count);
    erode(pixmap, 2200);
    int portal_room = random_between(0, rects_count - 1);

    Chest yoo_chests[MAX_CHESTS];
    int num_chests = 0;
    for (int i = 0; i < rects_count; i++)
    {
        Rect rect = rects[i];
        int div = 3;
        int portal;
        if (i == portal_room)
        {
            portal = random_between(0, div * div - 1);
        }
        int chests = 0;
        for (int f = 0; f < div * div; f++)
        {
            Rect sub = rect_translate(subdivide_rect(rect, div, f), rect.tl);
            if (i != portal_room || (i == portal_room && f != portal))
            {
                if (rand() % 100 < 20 && chests < MAX_CHESTS_PER_ROOM && num_chests < MAX_CHESTS)
                {
                    yoo_chests[num_chests++] = generate_chest(sub);
                    chests++;
                }
                else {
                    generate_spikes(pixmap, sub);
                    generate_obstacles(pixmap, sub);
                }
            }
            else {
                generate_portal(pixmap, sub);
            }
        }
    }
    bitmap_draw_box(pixmap, window);

    uint32_t illuminated_data[MAP_WIDTH][MAP_HEIGHT] = {};
    Bitmap illuminated = {(uint32_t *)illuminated_data, {{MAP_WIDTH, MAP_HEIGHT}}};

    Vec2i first_rect_center = get_center(rects[0]);
    // for (int i = 0; i < 500; i++)
    // {
    //     first_rect_center = get_center(rects[0]);
    //     if (get_normal_map_value(pixmap, first_rect_center) == WALKABLE)
    //         return;
    // }
    Warrior player;
    player.rect =
        (RectFloat){{first_rect_center.x, first_rect_center.y}, {first_rect_center.x, first_rect_center.y}, 2};
    player.dmg = 5;
    player.hp = 100;
    player.maxHP = 100;
    player.kills = 0;
    player.weight = 5;
    player.velocity = (Vec2f){0, 0};
    player.dmg_cooldown = 0;

    Camera camera = {{{0, 0}}, 0, 0, 10};

    CameraMode cam_mode = CameraMode_Follow;

    Torch torches[MAX_TORCHES];
    create_torches(pixmap, torches, MAX_TORCHES);

    Mob mobs[MAX_MOBS];
    create_mobs(pixmap, mobs, MAX_MOBS);

    Inventory inventory;
    add_item(&inventory, ItemType_HealthPotion, 1);

    // Define the items
    // Item sword = {ITEM_TYPE_SWORD, "Sword", 'S', COLOR_WHITE};
    // Item blastgun = {ITEM_TYPE_BLASTGUN, "Blastgun", 'B', COLOR_WHITE};
    // Item health_potion = {ITEM_TYPE_HEALTH_POTION, "Health Potion", 'H', COLOR_WHITE};
    // Item coins = {ITEM_TYPE_COINS, "Coins", 'C', COLOR_WHITE};
    // Item key = {ITEM_TYPE_KEY, "Key", 'K', COLOR_WHITE};

    // Add the items to the inventory
    // add_item(&inventory, sword);
    // add_item(&inventory, blastgun);
    // for (int i = 0; i < 5; i++)
    // {
    //     add_item(&inventory, health_potion);
    // }
    // for (int i = 0; i < 20; i++)
    // {
    //     add_item(&inventory, coins);
    // }
    // add_item(&inventory, key);

    Player_Stats player_stats;
    player_stats.hp = 100;
    player_stats.mana = 50;
    player_stats.maxMana = 50;
    player_stats.level = 1;
    player_stats.defense = 5;
    player_stats.rbp = 0;

    gs->cam_mode = cam_mode;
    gs->player_stats = player_stats;
    gs->camera = camera;
    gs->player = player;
    gs->torches = torches;
    gs->mobs = mobs;
    gs->pixmap = pixmap;
    gs->illuminated = illuminated;
    gs->player_attacking = 0;
    gs->minimap_maximized = false;
    gs->inventory = inventory;
    gs->chests = yoo_chests;
    gs->chests_count = num_chests;
}


Player_Stats player_stats;

void player_attack(GameState *gs, Mob *mobs, int num_mobs, Warrior *player, int delta_us)
{
    gs->player_attacking = 0.2 * 1e6;
    for (int i = 0; i < num_mobs; i++)
    {
        if (mobs[i].warrior.hp <= 0)
            continue;
        warrior_attack(player, &mobs[i].warrior, delta_us);
    }
}

int char_width_int(int value)
{
    return snprintf(NULL, 0, "%d", value);
}

void render_hp(WINDOW *win_game, Camera camera, Rect rect, int hp)
{
    Vec2i size = rect_size(rect);
    Rect translated_rect = rect_translate(rect, vec2i_mul_const(camera.offset, -1));
    int char_width = char_width_int(hp);
    int new_x = (int)(translated_rect.tl.x * X_SCALE + (size.x * X_SCALE) / 2.f - (char_width / 2.f)) + 1;
    wattrset(win_game, COLOR_PAIR(Culur_Default));
    mvwprintw(win_game, translated_rect.tl.y - 1, new_x, "%d", hp);
}

void render_hotbar(WINDOW *win, Hotbar *hotbar, Vec2i window_size)
{
    // Calculate the position of the hotbar
    int hotbar_x = (window_size.x - HOTBAR_SIZE) / 2;
    int hotbar_y = window_size.y - 2;

    // Draw the hotbar
    wattrset(win, COLOR_PAIR(Culur_Hotbar));
    for (int i = 0; i < HOTBAR_SIZE; i++)
    {
        // Draw the hotbar slot
        mvwprintw(win, hotbar_y, hotbar_x + i * 2, "[ ]");

        // Draw the item in the slot
        if (hotbar->items[i].count > 0)
        {
            // This is just a placeholder, you'll need to replace 'I' with the actual item icon
            mvwprintw(win, hotbar_y, hotbar_x + i * 2 + 1, "I");

            // Draw the item count
            mvwprintw(win, hotbar_y + 1, hotbar_x + i * 2, "%d", hotbar->items[i].count);
        }

        // Highlight the selected item
        if (i == hotbar->selected)
        {
            wattrset(win, COLOR_PAIR(Culur_Hotbar_Selected));
            mvwprintw(win, hotbar_y, hotbar_x + i * 2, "[%c]", 'I'); // Replace 'I' with the actual item icon
            wattrset(win, COLOR_PAIR(Culur_Hotbar));
        }
    }
}

void update_player(RectFloat *st, int key)
{
    switch (key)
    {
    case KEY_A1:
    case '7':
        *st = rect_float_translate(*st, (Vec2f){-1, -1});
        break;
    case KEY_UP:
    case '8':
    case 'w':
        *st = rect_float_translate(*st, (Vec2f){0, -1});
        break;
    case KEY_A3:
    case '9':
        *st = rect_float_translate(*st, (Vec2f){1, -1});
        break;
    case KEY_LEFT:
    case '4':
    case 'a':
        *st = rect_float_translate(*st, (Vec2f){-1, 0});
        break;
    case KEY_B2:
    case '5':
        break;
    case KEY_RIGHT:
    case '6':
    case 'd':
        *st = rect_float_translate(*st, (Vec2f){1, 0});
        break;
    case KEY_C1:
    case '1':
        *st = rect_float_translate(*st, (Vec2f){-1, 1});
        break;
    case KEY_DOWN:
    case '2':
    case 's':
        *st = rect_float_translate(*st, (Vec2f){0, 1});
        break;
    case KEY_C3:
    case '3':
        *st = rect_float_translate(*st, (Vec2f){1, 1});
        break;
    default:
        break;
    }
}

Rect project_rect(Camera camera, Rect player)
{
    return rect_translate(player, vec2i_mul_const(camera.offset, -1));
}

void render_rect(WINDOW *win, Camera camera, Rect player)
{
    print_rectangle(win, project_rect(camera, player));
}

Rect rect_float_to_rect(RectFloat rect)
{
    Rect result;
    result.color = rect.color;
    result.tl = vec2f_to_i(rect.tl);
    result.br = vec2f_to_i(rect.br);
    return result;
}

void mix_lightmap(Bitmap output, Bitmap input, Camera camera)
{
    for (int x = 0; x < input.width; x++)
    {
        for (int y = 0; y < input.height; y++)
        {
            Vec2i screen_pos = (Vec2i){x, y};
            Vec2i abs_pos = vec2i_add((Vec2i){x, y}, camera.offset);
            int v1 = get_light_map_value(input, screen_pos);
            int v2 = get_light_map_value(output, abs_pos);
            set_light_map_value(output, abs_pos, (v1 + v2));
        }
    }
}

void render_player_attack(GameState *gs, Rect player, Vec2i window_size)
{
    Circle c;
    c.radius = 3;

    c.center = vec2f_to_i(
        rect_center(rect_translate(project_rect(gs->camera, player), (Vec2i){-c.radius, -c.radius})));
    wattrset(gs->win_game, COLOR_PAIR(5));
    print_circumference(gs->win_game, window_size, c);
}

void draw_game(GameState *gs, Vec2i window_size, int key, State *state, int delta_us) {
    int minimap_height = 20;
    int sidebar_width = minimap_height;
    int player_stats_height = 10;
    int inventory_height = 10;
    wresize(gs->win_log, window_size.y - minimap_height - player_stats_height - inventory_height, sidebar_width * X_SCALE);
    wresize(gs->win_minimap, minimap_height, sidebar_width * X_SCALE);
    mvwin(gs->win_minimap, window_size.y - minimap_height, 0);
    window_size.x -= sidebar_width;
    wresize(gs->win_game, window_size.y, window_size.x);

    werase(gs->win_game);
    werase(gs->win_log);
    werase(gs->win_minimap);
    werase(gs->win_inventory);

    window_size.x /= X_SCALE; // A partir daqui nao mexer com janelas

    gs->camera.width = window_size.x;
    gs->camera.height = window_size.y;
    if (key == 't')
    {
        ++gs->cam_mode;
        gs->cam_mode %= CameraMode__Size;
    }

    Vec2i player_center = vec2f_to_i(rect_float_center(gs->player.rect));
    if (key == ' ')
    {
        center_camera(&gs->camera, player_center);
    }

    if (key == 'm')
    {
        gs->minimap_maximized = !gs->minimap_maximized;
    }

    RectFloat prev_player = gs->player.rect;
    update_player(&gs->player.rect, key);
    if (collide_rect_bitmap(rect_float_to_rect(gs->player.rect), gs->pixmap))
    {
        gs->player.rect = prev_player;
    }

    update_mobs(gs->mobs, MAX_MOBS, gs->pixmap, &gs->player, delta_us);

    if (gs->cam_mode == CameraMode_Margin)
        update_camera(&gs->camera, player_center);
    else
        center_camera(&gs->camera, player_center);

    for (int x = 0; x < gs->pixmap.width; x++)
    {
        for (int y = 0; y < gs->pixmap.height; y++)
        {
            if (get_normal_map_value(gs->pixmap, (Vec2i){x, y}) == SHINE)
                set_normal_map_value(gs->pixmap, (Vec2i){x, y}, WALL);
            set_dist_map_value(gs->pixmap, (Vec2i){x, y}, 0);
        }
    }
    dist_pass(gs->pixmap, player_center, gs->illuminated);
    light_reset(gs->pixmap);

    for (int i = 0; i < MAX_TORCHES; i++)
    {
        Bitmap lightmap = alloc_bitmap(MAP_WIDTH, MAP_HEIGHT);
        light_reset(lightmap);
        light_pass(gs->camera, lightmap, gs->torches[i].position, gs->torches[i].radius, LightType_Torch, gs->pixmap);

        mix_lightmap(gs->pixmap, lightmap, gs->camera);
        free_bitmap(lightmap);
    }

    Bitmap lightmap = alloc_bitmap(MAP_WIDTH, MAP_HEIGHT);
    light_reset(lightmap);
    light_pass(gs->camera, lightmap, rect_float_to_rect(gs->player.rect), LIGHT_RADIUS, LightType_Vision, gs->pixmap);

    mix_lightmap(gs->pixmap, lightmap, gs->camera);
    free_bitmap(lightmap);

    render_map(gs->win_game,gs->camera, gs->pixmap, gs->win_game, gs->illuminated);

    if (key == 'j')
    {
        player_attack(gs, gs->mobs, MAX_MOBS, &gs->player, delta_us);
    }

    for (int i = 0; i < MAX_TORCHES; i++)
    {
        wattrset(gs->win_game, COLOR_PAIR(3));
        print_pixel(gs->win_game, gs->torches[i].position.tl.x - gs->camera.x,
                    gs->torches[i].position.tl.y - gs->camera.y);
    }

    for (int i = 0; i < MAX_MOBS; i++)
    {
        int dist = get_dist_map_value(gs->pixmap, vec2f_to_i(rect_float_center(gs->mobs[i].warrior.rect)));
        // add_term_line("%d\n", dist);
        if (gs->mobs[i].warrior.hp <= 0 || dist == 0 || dist > MAX_DIST_SHINE)
            continue;
        
        render_rect(gs->win_game, gs->camera, rect_float_to_rect(gs->mobs[i].warrior.rect));
        render_hp(gs->win_game, gs->camera, rect_float_to_rect(gs->mobs[i].warrior.rect), gs->mobs[i].type);
    }

    for (int i = 0; i < gs->chests_count; i++)
    {
        if (!gs->chests[i].is_open)
        {
            if (collide_rect_rect(rect_float_to_rect(gs->player.rect), gs->chests[i].rect))
            {
                add_inventory(&gs->inventory, gs->chests[i].inventory);
                gs->chests[i].is_open = 1;
            }
            draw_chest(gs->win_game, project_rect(gs->camera, gs->chests[i].rect));
        }
    }

    if (gs->player_attacking)
    {
        timer_update(&gs->player_attacking, delta_us);
        render_player_attack(gs, rect_float_to_rect(gs->player.rect), window_size);
    }

    if (get_normal_map_value(gs->pixmap, player_center) == SPIKE)
    {
        if (timer_update(&gs->player_spike_damage_cooldown, delta_us))
        {
            gs->player_spike_damage_cooldown = SPIKE_DAMAGE_COOLDOWN;
            gs->player.hp -= SPIKE_DAMAGE;
            if (gs->player.hp < 0)
                gs->player.hp = 0;
        }
    }

    render_hp(gs->win_game, gs->camera, rect_float_to_rect(gs->player.rect), gs->player.hp);

    render_rect(gs->win_game, gs->camera, rect_float_to_rect(gs->player.rect));
    render_minimap(gs->win_minimap, gs->illuminated, (Vec2i){sidebar_width, minimap_height}, player_center);
    draw_inventory(gs->win_inventory, &gs->inventory, (Vec2i){sidebar_width * X_SCALE, inventory_height});

    if (key == 27 || key == 'p') {
        *state = State_Pause;
    }
    render_term(gs->win_log);
    render_player_stats(gs->win_stats, gs->player_stats, gs->player, (Vec2i){sidebar_width * X_SCALE, player_stats_height});

    wrefresh(gs->win_game);
    wrefresh(gs->win_log);
    wrefresh(gs->win_minimap);
    wrefresh(gs->win_stats);
    wrefresh(gs->win_inventory);
}
