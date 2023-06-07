#include "game.h"
#include "buffs.h"
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
#include "movimento.h"
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

void init_game(GameState *gs, Rect window)
{
    zero_bitmap(gs->illuminated);
    zero_bitmap(gs->collision);
    zero_bitmap(gs->light);

    Rect rects[10];
    int rects_count = generate_rects(expand_rect(window, -5), rects, ARRAY_SIZE(rects));
    order_rects(rects, rects_count);

    generate_tunnels_and_rasterize(gs->collision, rects, rects_count);
    erode(gs->collision, 2200);
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
            Rect sub = subdivide_rect(rect, div, f);
            if (i != portal_room || (i == portal_room && f != portal_div))
            {
                if (chests < MAX_CHESTS_PER_ROOM && num_chests < MAX_CHESTS)
                {
                    yoo_chests[num_chests] = generate_chest(sub);
                    chests++;
                    num_chests++;
                }
                else
                {
                    generate_spikes(gs->collision, sub);
                    generate_obstacles(gs->collision, sub);
                }
            }
            else
            {
                portal_rect = generate_portal(sub);
            }
        }
    }
    int key_chest_pos = random_between(0, num_chests - 1);
    add_item(&yoo_chests[key_chest_pos].inventory, Item_Key, 1);
    bitmap_draw_box(gs->collision, window);

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
    player.dmg = 20;
    player.hp = 100;
    player.maxHP = 100;
    player.weight = 4;
    player.velocity = (Vec2f){0, 0};

    Camera camera = {{{0, 0}}, 0, 0, 10};

    CameraMode cam_mode = CameraMode_Follow;

    Torch torches[MAX_TORCHES];
    create_torches(gs->collision, torches, MAX_TORCHES);

    Mob mobs[MAX_MOBS];
    create_mobs(gs->collision, mobs, MAX_MOBS);

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
    gs->minimap_maximized = false;
    gs->chests = yoo_chests;
    gs->chests_count = num_chests;
    gs->portal = portal_rect;
    if (gs->arrows)
        free(gs->arrows);
    gs->arrows = malloc(sizeof(Arrow) * MAX_ARROWS);
}

void player_attack(Mob *mobs, int num_mobs, Warrior *player)
{
    int attacked = 0;
    if (!player->dmg_cooldown)
    {
        for (int i = 0; i < num_mobs; i++)
        {
            if (mobs[i].warrior.hp <= 0)
                continue;
            attacked += warrior_attack(player, &mobs[i].warrior);
        }
    }
    if (attacked)
    {
        player->dmg_cooldown = 1e6 * 0.2;
    }
    player->attacking = 0.1 * 1e6;
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
    int new_x = (int)(translated_rect.tl.x * X_SCALE + (size.x * X_SCALE) / 2.f - (char_width / 2.f));
    wattrset(win_game, COLOR_PAIR(Culur_Default));
    mvwprintw(win_game, translated_rect.tl.y - 1, new_x, "%d", hp);
}

Rect project_rect(Camera camera, Rect player)
{
    return rect_translate(player, vec2i_mul_const(camera.offset, -1));
}

void render_rect(WINDOW *win, Camera camera, Rect player)
{
    print_rectangle(win, project_rect(camera, player));
}

void render_player_attack(GameState *gs, Warrior player, Vec2i window_size)
{
    Circle c;
    c.radius = player.weight;

    c.center = vec2f_to_i(rect_center(
        rect_translate(project_rect(gs->camera, rect_float_to_rect(player.rect)), (Vec2i){-c.radius, -c.radius})));
    wattrset(gs->win_game, COLOR_PAIR(5));
    print_circle(gs->win_game, window_size, c);
}

int use_key(GameState *gs)
{
    if (collide_rect_rect(rect_float_to_rect(gs->player.rect), gs->portal))
    {
        generate_random_buffs();
        apply_buffs(gs);

        Rect window = {{0, 0}, {0, 0}, 0};
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
    Vec2i pos = vec2f_to_i(rect_center(project_rect(gs->camera, rect_float_to_rect(arrow.rect))));
    wattrset(gs->win_game, COLOR_PAIR(Culur_Default_Red));
    print_pixel_custom(gs->win_game, pos.x, pos.y, ">");
}

void mix_lightmap(Bitmap output, Bitmap input)
{
    for (int x = 0; x < input.width; x++)
    {
        for (int y = 0; y < input.height; y++)
        {
            Vec2i abs_pos = (Vec2i){x, y};
            int v1 = get_bitmap_value(input, abs_pos);
            int v2 = get_bitmap_value(output, abs_pos);
            set_bitmap_value(output, abs_pos, (v1 + v2));
        }
    }
}

void update_game(GameState *gs, Vec2i window_size, int key, State *state, int delta_us)
{
    wresize(gs->win_log, window_size.y - gs->minimap_height - gs->player_stats_height - gs->inventory_height,
            gs->sidebar_width * X_SCALE);
    wresize(gs->win_minimap, gs->minimap_height, gs->sidebar_width * X_SCALE);
    mvwin(gs->win_minimap, window_size.y - gs->minimap_height, 0);
    window_size.x -= gs->sidebar_width;
    wresize(gs->win_game, window_size.y, window_size.x);

    window_size.x /= X_SCALE; // A partir daqui nao mexer com janelas

    gs->camera.width = window_size.x;
    gs->camera.height = window_size.y;
    Vec2i player_center = vec2f_to_i(rect_float_center(gs->player.rect));

    if (gs->player.hp <= 0)
    {
        *state = State_Over;
    }

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
                player_attack(gs->mobs, MAX_MOBS, &gs->player);
                break;
            case Item_BlastGun:
                // adicionar mais tarde
                break;
            case Item_HealthPotion:
                use_health_potion(&gs->player);
                remove_item(&gs->inventory, i);
                break;
            case Item_ManaPotion:
                // adicionar mais tarde
                break;
            case Item_Key:
                if (use_key(gs))
                {
                    remove_item(&gs->inventory, i);
                };
                break;
            case Item__Size:
                // adicionar mais tarde
                break;
            default:
                // adicionar mais tarde
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

    timer_update(&gs->player.dmg_cooldown, delta_us);

    RectFloat prev_player = gs->player.rect;
    update_player(&gs->player.rect, key);
    if (collide_rect_bitmap(rect_float_to_rect(gs->player.rect), gs->collision))
    {
        gs->player.rect = prev_player;
    }

    if (gs->cam_mode == CameraMode_Margin)
        update_camera(&gs->camera, player_center);
    else
        center_camera(&gs->camera, player_center);

    for (int x = 0; x < MAP_WIDTH; x++)
    {
        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            set_bitmap_value(gs->distance, (Vec2i){x, y}, 0);
            if (get_bitmap_value(gs->collision, (Vec2i){x, y}) == SHINE)
                set_bitmap_value(gs->collision, (Vec2i){x, y}, WALL);
            set_bitmap_value(gs->light, (Vec2i){x, y}, 0);
        }
    }
    dist_pass(gs->distance, gs->collision, player_center, gs->illuminated);

    Bitmap player_light = alloc_bitmap(MAP_WIDTH, MAP_HEIGHT);
    light_pass(player_light, rect_float_to_rect(gs->player.rect), LIGHT_RADIUS, gs->collision);
    mix_lightmap(gs->light, player_light);

    update_mobs(gs->mobs, MAX_MOBS, gs->distance, &gs->player, player_light, delta_us, gs->arrows, &gs->arrow_count);
    free_bitmap(player_light);

    for (int i = 0; i < gs->arrow_count; i++)
    {
        gs->arrows[i].rect =
            rect_float_translate(gs->arrows[i].rect, vec2f_mul_const(gs->arrows[i].velocity, delta_us / 1e6 * 10));
        if (collide_rect_rect(rect_float_to_rect(gs->arrows[i].rect), rect_float_to_rect(gs->player.rect)))
        {
            gs->player.hp -= 10;
            gs->arrows[i] = gs->arrows[gs->arrow_count - 1];
            gs->arrow_count--;
            i--;
        }
        else if (collide_rect_bitmap(rect_float_to_rect(gs->arrows[i].rect), gs->collision))
        {
            gs->arrows[i] = gs->arrows[gs->arrow_count - 1];
            gs->arrow_count--;
            i--;
        }
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
        }
    }

    if (get_bitmap_value(gs->collision, player_center) == SPIKE)
    {
        if (timer_update(&gs->player_spike_damage_cooldown, delta_us))
        {
            gs->player_spike_damage_cooldown = SPIKE_DAMAGE_COOLDOWN;
            gs->player.hp -= SPIKE_DAMAGE;
        }
    }
    else
    {
        gs->player_spike_damage_cooldown = 0;
    }

    gs->inventory.items[0].cooldown = gs->player.dmg_cooldown / 1e6;

    if (key == 27 || key == 'p')
    {
        *state = State_Pause;
    }
}

void draw_game(GameState *gs, Vec2i window_size, int delta_us)
{
    werase(gs->win_game);
    werase(gs->win_log);
    werase(gs->win_minimap);
    werase(gs->win_stats);
    werase(gs->win_inventory);

    Vec2i player_center = vec2f_to_i(rect_float_center(gs->player.rect));

    for (int i = 0; i < MAX_TORCHES; i++)
    {
        int dist = get_rect_distance(gs->distance, gs->torches[i].position);
        if (dist == 0 || dist > MAX_DIST_SHINE)
            continue;
        Bitmap torch_light = alloc_bitmap(MAP_WIDTH, MAP_HEIGHT);
        light_pass(torch_light, gs->torches[i].position, gs->torches[i].radius, gs->collision);
        mix_lightmap(gs->light, torch_light);
    }

    render_map(gs->win_game, gs->camera, gs->collision, gs->light, gs->distance, gs->win_game, gs->illuminated);

    if (gs->player.attacking)
    {
        timer_update(&gs->player.attacking, delta_us);
        render_player_attack(gs, gs->player, window_size);
    }

    for (int i = 0; i < MAX_TORCHES; i++)
    {
        int dist = get_rect_distance(gs->distance, gs->torches[i].position);
        if (dist == 0 || dist > MAX_DIST_SHINE)
            continue;
        wattrset(gs->win_game, COLOR_PAIR(3));
        print_pixel(gs->win_game, gs->torches[i].position.tl.x - gs->camera.x,
                    gs->torches[i].position.tl.y - gs->camera.y);
    }

    for (int i = 0; i < gs->chests_count; i++)
    {
        if (!gs->chests[i].is_open)
        {
            int dist = get_rect_distance(gs->distance, gs->chests[i].rect);
            if (dist == 0 || dist > MAX_DIST_SHINE)
                continue;
            draw_chest(gs->win_game, project_rect(gs->camera, gs->chests[i].rect));
        }
    }

    for (int i = 0; i < gs->arrow_count; i++)
    {
        draw_arrow(gs, gs->arrows[i]);
    }

    int dist = get_rect_distance(gs->distance, gs->portal);
    if (dist != 0 && dist <= MAX_DIST_SHINE)
        draw_portal(gs->win_game, project_rect(gs->camera, gs->portal));

    for (int i = 0; i < MAX_MOBS; i++)
    {
        int dist = get_bitmap_value(gs->distance, vec2f_to_i(rect_float_center(gs->mobs[i].warrior.rect)));
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

    render_hp(gs->win_game, gs->camera, rect_float_to_rect(gs->player.rect), gs->player.hp);

    render_rect(gs->win_game, gs->camera, rect_float_to_rect(gs->player.rect));
    render_minimap(gs->win_minimap, gs->illuminated, (Vec2i){gs->sidebar_width, gs->minimap_height}, player_center);
    draw_inventory(gs->win_inventory, &gs->inventory, (Vec2i){gs->sidebar_width * X_SCALE, gs->inventory_height},
                   gs->inventory.selected_item);

    render_term(gs->win_log, delta_us);
    render_player_stats(gs->win_stats, gs->player_stats, gs->player,
                        (Vec2i){gs->sidebar_width * X_SCALE, gs->player_stats_height});

    // wattrset(gs->win_game, COLOR_PAIR(Culur_Default));
    // mvwprintw(gs->win_game, window_size.y - 1, 0, "Press ESC or P to pause | %d FPS | %f ms", (int)(1e6/delta_us),
    // 1e-3*delta_us);

    wrefresh(gs->win_game);
    wrefresh(gs->win_log);
    wrefresh(gs->win_minimap);
    wrefresh(gs->win_stats);
    wrefresh(gs->win_inventory);
}
