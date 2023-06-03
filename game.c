#include "game.h"
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
#include "items.h"
#include "buffs.h"

void init_game(GameState *gs, Rect window) {
    Rect rects[1];
    int rects_count = generate_rects(expand_rect(window, -5), rects, ARRAY_SIZE(rects));
    order_rects(rects, rects_count);

    if (gs->pixmap.data) { free(gs->pixmap.data); }
    Bitmap pixmap = alloc_bitmap(MAP_WIDTH, MAP_HEIGHT);
    generate_tunnels_and_rasterize(pixmap, rects, rects_count);
    erode(pixmap, 2200);
    int portal_room = random_between(0, rects_count - 1);
    int div = 2;
    int portal_div = random_between(0, div * div - 1);

    Chest yoo_chests[MAX_CHESTS];
    int num_chests = 0;
    Rect portal_rect;
    for (int i = 0; i < rects_count; i++)
    {
        Rect rect = rects[i];
        int chests = 0;
        for (int f = 0; f < div * div; f++)
        {
            Rect sub = rect_translate(subdivide_rect(rect, div, f), rect.tl);
            if (i != portal_room || (i == portal_room && f != portal_div))
            {
                if (chests < MAX_CHESTS_PER_ROOM && num_chests < MAX_CHESTS)
                {
                    yoo_chests[num_chests] = generate_chest(sub);
                    chests++;
                    num_chests++;
                }
                else {
                    generate_spikes(pixmap, sub);
                    generate_obstacles(pixmap, sub);
                }
            }
            else {
                portal_rect = generate_portal(sub);
            }
        }
    }
    int key_chest_pos = random_between(0, num_chests - 1);
    add_item(&yoo_chests[key_chest_pos].inventory, Item_Key, 1);
    bitmap_draw_box(pixmap, window);

    if (gs->illuminated.data) { free(gs->illuminated.data); }
    Bitmap illuminated = alloc_bitmap(MAP_WIDTH, MAP_HEIGHT);

    Vec2i first_rect_center = get_center(rects[0]);
    // for (int i = 0; i < 500; i++)
    // {
    //     first_rect_center = get_center(rects[0]);
    //     if (get_normal_map_value(pixmap, first_rect_center) == WALKABLE)
    //         return;
    // }
    Warrior player = (Warrior){0};
    player.rect =
        (RectFloat){{first_rect_center.x, first_rect_center.y}, {first_rect_center.x, first_rect_center.y}, 2};
    player.dmg = 5;
    player.hp = 100;
    player.maxHP = 100;
    player.weight = 10;
    player.velocity = (Vec2f){0, 0};

    Camera camera = {{{0, 0}}, 0, 0, 10};

    CameraMode cam_mode = CameraMode_Follow;

    Torch torches[MAX_TORCHES];
    create_torches(pixmap, torches, MAX_TORCHES);

    Mob mobs[MAX_MOBS];
    create_mobs(pixmap, mobs, MAX_MOBS);

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
    gs->minimap_maximized = false;
    gs->chests = yoo_chests;
    gs->chests_count = num_chests;
    gs->portal = portal_rect;
    gs->arrows = malloc(sizeof(Arrow) * MAX_ARROWS);
}

void player_attack(GameState *gs, Mob *mobs, int num_mobs, Warrior *player, int delta_us)
{
    gs->player_attacking = 10;
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

void update_player(RectFloat *st, int key, int delta_us)
{
    Vec2f move = {0};
    switch (key)
    {
    case KEY_A1:
        move = (Vec2f){-1, -1};
        break;
    case KEY_UP:
    case 'w':
        move = (Vec2f){0, -1};
        break;
    case KEY_A3:
        move = (Vec2f){1, -1};
        break;
    case KEY_LEFT:
    case 'a':
        move = (Vec2f){-1, 0};
        break;
    case KEY_B2:
        break;
    case KEY_RIGHT:
    case 'd':
        move = (Vec2f){1, 0};
        break;
    case KEY_C1:
        move = (Vec2f){-1, 1};
        break;
    case KEY_DOWN:
    case 's':
        move = (Vec2f){0, 1};
        break;
    case KEY_C3:
        move = (Vec2f){1, 1};
        break;
    default:
        break;
    }
    *st = rect_float_translate(*st, move);
}

Rect project_rect(Camera camera, Rect player)
{
    return rect_translate(player, vec2i_mul_const(camera.offset, -1));
}

void render_rect(WINDOW *win, Camera camera, Rect player)
{
    print_rectangle(win, project_rect(camera, player));
}

void mix_lightmap(Bitmap output, Bitmap input)
{
    for (int x = 0; x < input.width; x++)
    {
        for (int y = 0; y < input.height; y++)
        {
            Vec2i abs_pos = (Vec2i){x, y};
            int v1 = get_light_map_value(input, abs_pos);
            int v2 = get_light_map_value(output, abs_pos);
            set_light_map_value(output, abs_pos, (v1 + v2));
        }
    }
}

void render_player_attack(GameState *gs, Warrior player, Vec2i window_size)
{
    Circle c;
    c.radius = player.weight;

    c.center = vec2f_to_i(
        rect_center(rect_translate(project_rect(gs->camera, rect_float_to_rect(player.rect)), (Vec2i){-c.radius, -c.radius})));
    wattrset(gs->win_game, COLOR_PAIR(5));
    print_circle(gs->win_game, window_size, c);
}

int use_key(GameState* gs)
{
    if (collide_rect_rect(rect_float_to_rect(gs->player.rect), gs->portal))
    {
        add_term_line("SOCORRO SOCORRO AJUDA SOCORRO!\n");
        generate_random_buffs();
        apply_buffs(gs);

        Rect window = {};
        window.tl.x = 0;
        window.tl.y = 0;
        window.br.x = MAP_WIDTH;
        window.br.y = MAP_HEIGHT;
        Player_Stats prev_stats = gs->player_stats;
        init_game(gs, window);
        gs->player_stats = prev_stats;
        gs->player_stats.level++;
        return 1;
    }
    add_term_line("Cant use key here!\n");
    return 0;
}

void draw_arrow(GameState *gs, Arrow arrow)
{
    Vec2i pos = vec2f_to_i(rect_center(rect_translate(project_rect(gs->camera, rect_float_to_rect(arrow.rect)), (Vec2i){-1, -1})));
    wattrset(gs->win_game, COLOR_PAIR(Culur_Default_Red));
    print_pixel_custom(gs->win_game, pos.x, pos.y, ">");
}


void update_game(GameState *gs, Vec2i window_size, int key, State *state, int delta_us)
{
    wresize(gs->win_log, window_size.y - gs->minimap_height - gs->player_stats_height - gs->inventory_height, gs->sidebar_width * X_SCALE);
    wresize(gs->win_minimap, gs->minimap_height, gs->sidebar_width * X_SCALE);
    mvwin(gs->win_minimap, window_size.y - gs->minimap_height, 0);
    window_size.x -= gs->sidebar_width;
    wresize(gs->win_game, window_size.y, window_size.x);

    window_size.x /= X_SCALE; // A partir daqui nao mexer com janelas

    gs->camera.width = window_size.x;
    gs->camera.height = window_size.y;
    Vec2i player_center = vec2f_to_i(rect_float_center(gs->player.rect));
    if (key == 't')
    {
        ++gs->cam_mode;
        gs->cam_mode %= CameraMode__Size;
    }
    if (key == ' ')
    {
        center_camera(&gs->camera, player_center);
    }
    if (key == 'm')
    {
        gs->minimap_maximized = !gs->minimap_maximized;
    }
    if (key == 'j')
    {
        int i = gs->inventory.selected_item - 1;
        Item item = gs->inventory.items[i].name;
        if (gs->inventory.items[i].count > 0)
        {
            switch (item)
            {
            case Item_Sword:
                player_attack(gs, gs->mobs, MAX_MOBS, &gs->player, delta_us);
                break;
            case Item_HealthPotion:
                use_health_potion(&gs->player);
                remove_item(&gs->inventory, i);
                break;
            case Item_Key:
                if (use_key(gs)) {
                    remove_item(&gs->inventory, i);
                };
                break;
            }
        }
    }
    if (key > '0' && key <= '9')
    {
        int selected_item = key - '1';
        if (selected_item < gs->inventory.size)
        {
            gs->inventory.selected_item = selected_item + 1;
        }
    }

    RectFloat prev_player = gs->player.rect;
    update_player(&gs->player.rect, key, delta_us);
    if (collide_rect_bitmap(rect_float_to_rect(gs->player.rect), gs->pixmap))
    {
        gs->player.rect = prev_player;
    }

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
        light_pass(lightmap, gs->torches[i].position, gs->torches[i].radius, gs->pixmap);

        mix_lightmap(gs->pixmap, lightmap);
        free_bitmap(lightmap);
    }

    Bitmap lightmap = alloc_bitmap(MAP_WIDTH, MAP_HEIGHT);
    light_reset(lightmap);
    light_pass(lightmap, rect_float_to_rect(gs->player.rect), LIGHT_RADIUS, gs->pixmap);

    update_mobs(gs->mobs, MAX_MOBS, gs->pixmap, &gs->player, lightmap, delta_us, gs->arrows, &gs->arrow_count);

    for (int i = 0; i < gs->arrow_count; i++)
    {
        gs->arrows[i].rect = rect_float_translate(gs->arrows[i].rect, vec2f_div_const(gs->arrows[i].velocity, 7));
        if (collide_rect_rect(rect_float_to_rect(gs->arrows[i].rect), rect_float_to_rect(gs->player.rect)))
        {
            gs->player.hp -= 10;
            gs->arrows[i] = gs->arrows[gs->arrow_count - 1];
            gs->arrow_count--;
            i--;
        }
        else if (collide_rect_bitmap(rect_float_to_rect(gs->arrows[i].rect), gs->pixmap))
        {
            gs->arrows[i] = gs->arrows[gs->arrow_count - 1];
            gs->arrow_count--;
            i--;
        }
    }

    mix_lightmap(gs->pixmap, lightmap);
    free_bitmap(lightmap);

    for (int i = 0; i < gs->chests_count; i++)
    {
        if (!gs->chests[i].is_open)
        {
            if (collide_rect_rect(rect_float_to_rect(gs->player.rect), gs->chests[i].rect))
            {
                add_inventory(&gs->inventory, gs->chests[i].inventory);
                gs->chests[i].is_open = 1;
            }
        }
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
    if (key == 27 || key == 'p') {
        *state = State_Pause;
    }
}

void draw_game(GameState *gs, Vec2i window_size, int delta_us) {
    werase(gs->win_game);
    werase(gs->win_log);
    werase(gs->win_minimap);
    werase(gs->win_stats);
    werase(gs->win_inventory);

    Vec2i player_center = vec2f_to_i(rect_float_center(gs->player.rect));

    render_map(gs->win_game, gs->camera, gs->pixmap, gs->win_game, gs->illuminated);

    if (gs->player_attacking)
    {
        timer_update(&gs->player_attacking, delta_us);
        render_player_attack(gs, gs->player, window_size);
    }

    for (int i = 0; i < MAX_MOBS; i++)
    {
        int dist = get_dist_map_value(gs->pixmap, vec2f_to_i(rect_float_center(gs->mobs[i].warrior.rect)));
        if (gs->mobs[i].warrior.hp <= 0 || dist == 0 || dist > MAX_DIST_SHINE)
            continue;
        
        render_rect(gs->win_game, gs->camera, rect_float_to_rect(gs->mobs[i].warrior.rect));
        render_hp(gs->win_game, gs->camera, rect_float_to_rect(gs->mobs[i].warrior.rect), gs->mobs[i].warrior.hp);

        if (gs->mobs[i].warrior.attacking)
        {
            timer_update(&gs->mobs[i].warrior.attacking, delta_us);
            render_player_attack(gs, gs->mobs[i].warrior, window_size);
        }
    }

    for (int i = 0; i < MAX_TORCHES; i++)
    {
        wattrset(gs->win_game, COLOR_PAIR(3));
        print_pixel(gs->win_game, gs->torches[i].position.tl.x - gs->camera.x,
                    gs->torches[i].position.tl.y - gs->camera.y);
    }

    for (int i = 0; i < gs->chests_count; i++)
    {
        if (!gs->chests[i].is_open)
        {
            draw_chest(gs->win_game, project_rect(gs->camera, gs->chests[i].rect));
        }
    }

    for (int i = 0; i < gs->arrow_count; i++)
    {
        draw_arrow(gs, gs->arrows[i]);
    }

    draw_portal(gs->win_game, project_rect(gs->camera, gs->portal));

    render_hp(gs->win_game, gs->camera, rect_float_to_rect(gs->player.rect), gs->player.hp);

    render_rect(gs->win_game, gs->camera, rect_float_to_rect(gs->player.rect));
    render_minimap(gs->win_minimap, gs->illuminated, (Vec2i){gs->sidebar_width, gs->minimap_height}, player_center);
    draw_inventory(gs->win_inventory, &gs->inventory, (Vec2i){gs->sidebar_width * X_SCALE, gs->inventory_height}, gs->inventory.selected_item);

    render_term(gs->win_log);
    render_player_stats(gs->win_stats, gs->player_stats, gs->player, (Vec2i){gs->sidebar_width * X_SCALE, gs->player_stats_height});
    
    // wattrset(gs->win_game, COLOR_PAIR(Culur_Default));
    // mvwprintw(gs->win_game, window_size.y - 1, 0, "Press ESC or P to pause | %d FPS | %f ms", (int)(1e6/delta_us), 1e-3*delta_us);

    wrefresh(gs->win_game);
    wrefresh(gs->win_log);
    wrefresh(gs->win_minimap);
    wrefresh(gs->win_stats);
    wrefresh(gs->win_inventory);
}
