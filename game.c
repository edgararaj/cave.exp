#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "camera.h"
#include "colors.h"
#include "combat.h"
#include "map.h"
#include "mobs.h"
#include "objects.h"
#include "state.h"
#include "term.h"
#include "light.h"

void warrior_attack(Warrior *a, Warrior *b)
{
    if (vec2f_sqrdistance(vec2f_sub(rect_float_center(a->rect), rect_float_center(b->rect))) <= a->weight * a->weight)
    {
        // add_term_line("HP: %d, -%d\n", mobs[i].hp, mobs[i].dmg);
        b->hp -= a->dmg;
        if (b->hp <= 0)
        {
            a->kills += 1;
        }
    }
}

void player_attack(GameState *gs, Mob *mobs, int num_mobs, Warrior *player)
{
    gs->player_attacking = 200;
    for (int i = 0; i < num_mobs; i++)
    {
        if (mobs[i].warrior.hp <= 0)
            return;
        warrior_attack(player, &mobs[i].warrior);
    }
}

int char_width_int(int value)
{
    return snprintf(NULL, 0, "%d", value);
}

void render_life(WINDOW *win_game, Camera camera, Rect rect, int life)
{
    Vec2i size = rect_size(rect);
    Rect translated_rect = rect_translate(rect, vec2i_mul_const(camera.offset, -1));
    int char_width = char_width_int(life);
    int new_x = (int)(translated_rect.tl.x * X_SCALE + (size.x * X_SCALE) / 2.f - (char_width / 2.f)) + 1;
    wattrset(win_game, COLOR_PAIR(Culur_Default));
    mvwprintw(win_game, translated_rect.tl.y - 1, new_x, "%d", life);
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

void timer_update(int *left, int delta)
{
    *left -= delta;
    if (*left < 0)
        *left = 0;
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

    RectFloat prev_player_rect = gs->player.rect;
    update_player(&gs->player.rect, key);
    if (collide_rect_bitmap(rect_float_to_rect(gs->player.rect), gs->pixmap))
    {
        gs->player.rect = prev_player_rect;
    }

    update_mobs(gs->mobs, MAX_MOBS, gs->pixmap, &gs->player);

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
        light_pass(gs->win_game, gs->camera, lightmap, gs->torches[i].position, gs->torches[i].radius, LightType_Torch,
                   gs->pixmap);
        mix_lightmap(gs->pixmap, lightmap, gs->camera);
        free_bitmap(lightmap);
    }

    Bitmap lightmap = alloc_bitmap(MAP_WIDTH, MAP_HEIGHT);
    light_reset(lightmap);
    light_pass(gs->win_game, gs->camera, lightmap, rect_float_to_rect(gs->player.rect), LIGHT_RADIUS, LightType_Vision,
               gs->pixmap);
    mix_lightmap(gs->pixmap, lightmap, gs->camera);
    free_bitmap(lightmap);

    render_map(gs->win_game, gs->camera, gs->pixmap, gs->win_game, gs->illuminated);

    if (key == 'j')
    {
        player_attack(gs, gs->mobs, MAX_MOBS, &gs->player);
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
        render_life(gs->win_game, gs->camera, rect_float_to_rect(gs->mobs[i].warrior.rect), gs->mobs[i].warrior.hp);
    }

    if (gs->player_attacking)
    {
        timer_update(&gs->player_attacking, delta_ms);
        render_player_attack(gs, rect_float_to_rect(gs->player.rect), gs->mobs, MAX_MOBS, window_size);
    }

    render_life(gs->win_game, gs->camera, rect_float_to_rect(gs->player.rect), gs->player.hp);

    render_rect(gs->win_game, gs->camera, rect_float_to_rect(gs->player.rect));
    render_minimap(gs->win_game, gs->illuminated, window_size, player_center);

    wrefresh(gs->win_game);
}
