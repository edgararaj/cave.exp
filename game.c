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

Player_Stats player_stats;

void player_attack(GameState *gs, Mob *mobs, int num_mobs, Warrior *player, int delta_ms)
{
    gs->player_attacking = 200;
    for (int i = 0; i < num_mobs; i++)
    {
        if (mobs[i].warrior.hp <= 0)
            continue;
        warrior_attack(player, &mobs[i].warrior, delta_ms);
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

void update_player(RectFloat *st, int key, GameState *gs)
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

Rect project_rect(WINDOW *win, Camera camera, Rect player)
{
    return rect_translate(player, vec2i_mul_const(camera.offset, -1));
}
void render_rect(WINDOW *win, Camera camera, Rect player)
{
    print_rectangle(win, project_rect(win, camera, player));
}

Rect rect_float_to_rect(RectFloat rect)
{
    Rect result;
    result.color = rect.color;
    result.tl = vec2f_to_i(rect.tl);
    result.br = vec2f_to_i(rect.br);
    return result;
}

void shine_reset(Bitmap distmap)
{
    for (int x = 0; x < distmap.width; x++)
    {
        for (int y = 0; y < distmap.height; y++)
        {
            if (normal_map_decode(distmap.data[y * distmap.width + x]) == SHINE)
                set_normal_map_value(distmap, (Vec2i){x, y}, WALL);
        }
    }
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

void render_player_attack(GameState *gs, Rect player, Mob *mobs, int num_mobs, Vec2i window_size)
{
    Circle c;
    c.radius = 3;

    c.center = vec2f_to_i(
        rect_center(rect_translate(project_rect(gs->win_game, gs->camera, player), (Vec2i){-c.radius, -c.radius})));
    wattrset(gs->win_game, COLOR_PAIR(5));
    print_circumference(gs->win_game, window_size, c);
}

void draw_game(GameState *gs, Vec2i window_size, int key, int delta_ms)
{
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
        // add_term_line("%d, %d\n", gs->camera.x, gs->camera.y);
    }

    if (key == 'i')
    {
        int ch;
        while (1)
        {
            draw_inventory(gs->win_inventory, &gs->inventory);
            ch = getch();
            if (ch == 'i' || ch == 'q')
            { // Pressione 'i' ou 'q' para sair do inventário
                break;
            }
        }
    }

    if (key == 'm')
    {
        gs->minimap_maximized = !gs->minimap_maximized;
    }

    RectFloat prev_player = gs->player.rect;
    update_player(&gs->player.rect, key, gs);
    if (collide_rect_bitmap(rect_float_to_rect(gs->player.rect), gs->pixmap))
    {
        gs->player.rect = prev_player;
    }

    update_mobs(gs->mobs, MAX_MOBS, gs->pixmap, &gs->player, delta_ms);

    if (gs->cam_mode == CameraMode_Margin)
        update_camera(&gs->camera, player_center);
    else
        center_camera(&gs->camera, player_center);

    dist_reset(gs->pixmap);
    shine_reset(gs->pixmap);
    dist_pass(gs->pixmap, player_center, gs->illuminated);

    light_reset(gs->pixmap);
    for (int i = 0; i < MAX_TORCHES; i++)
    {
        Bitmap lightmap = alloc_bitmap(MAP_WIDTH, MAP_HEIGHT);
        light_reset(lightmap);
        light_pass(gs, gs->win_game, gs->camera, lightmap, gs->torches[i].position, gs->torches[i].radius,
                   LightType_Torch, gs->pixmap, gs->player_stats, &gs->inventory);

        mix_lightmap(gs->pixmap, lightmap, gs->camera);
        free_bitmap(lightmap);
    }

    Bitmap lightmap = alloc_bitmap(MAP_WIDTH, MAP_HEIGHT);
    light_reset(lightmap);
    light_pass(gs, gs->win_game, gs->camera, lightmap, rect_float_to_rect(gs->player.rect), LIGHT_RADIUS,
               LightType_Vision, gs->pixmap, gs->player_stats, &gs->inventory);

    mix_lightmap(gs->pixmap, lightmap, gs->camera);
    free_bitmap(lightmap);

    render_map(gs->win_game, gs, gs->camera, gs->pixmap, gs->win_game, gs->illuminated);

    if (key == 'j')
    {
        player_attack(gs, gs->mobs, MAX_MOBS, &gs->player, delta_ms);
    }

    for (int i = 0; i < MAX_TORCHES; i++)
    {
        wattrset(gs->win_game, COLOR_PAIR(3));
        print_pixel(gs->win_game, gs->torches[i].position.tl.x - gs->camera.x,
                    gs->torches[i].position.tl.y - gs->camera.y);
    }

    for (int i = 0; i < MAX_MOBS; i++)
    {
        if (gs->mobs[i].warrior.hp <= 0)
            continue;
        render_rect(gs->win_game, gs->camera, rect_float_to_rect(gs->mobs[i].warrior.rect));
        render_hp(gs->win_game, gs->camera, rect_float_to_rect(gs->mobs[i].warrior.rect), gs->mobs[i].warrior.hp);
    }

    if (gs->player_attacking)
    {
        timer_update(&gs->player_attacking, delta_ms);
        render_player_attack(gs, rect_float_to_rect(gs->player.rect), gs->mobs, MAX_MOBS, window_size);
    }

    if (get_normal_map_value(gs->pixmap, player_center) == SPIKE)
    {
        if (timer_update(&gs->player_spike_damage_cooldown, delta_ms))
        {
            gs->player_spike_damage_cooldown = SPIKE_DAMAGE_COOLDOWN;
            gs->player.hp -= SPIKE_DAMAGE;
            if (gs->player.hp < 0)
                gs->player.hp = 0;
        }
    }

    render_hp(gs->win_game, gs->camera, rect_float_to_rect(gs->player.rect), gs->player.hp);
    displayHUD(&player_stats);

    render_rect(gs->win_game, gs->camera, rect_float_to_rect(gs->player.rect));
    render_minimap(gs->win_game, gs->illuminated, window_size, player_center, gs->minimap_maximized);

    wrefresh(gs->win_game);
}
