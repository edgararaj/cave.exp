#include <assert.h>
#include <math.h>

#include "camera.h"
#include "collide.h"
#include "colors.h"
#include "draw.h"
#include "light.h"
#include "map.h"
#include "objects.h"
#include "state.h"
#include "term.h"
#include "utils.h"

uint32_t dist_map_encode(int value)
{
    return (value & PARTITION_MASK) << DIST_MAP_SHIFT;
}

uint32_t light_map_encode(int value)
{
    return (value & PARTITION_MASK) << LIGHT_MAP_SHIFT;
}

int light_map_decode(uint32_t value)
{
    return (value & LIGHT_MAP_MASK) >> LIGHT_MAP_SHIFT;
}

int dist_map_decode(uint32_t value)
{
    return (value & DIST_MAP_MASK) >> DIST_MAP_SHIFT;
}

int normal_map_decode(uint32_t value)
{
    return (value & NORMAL_MAP_MASK) >> NORMAL_MAP_SHIFT;
}

uint32_t normal_map_encode(int value)
{
    return value << NORMAL_MAP_SHIFT;
}

void set_normal_map_value(Bitmap bitmap, Vec2i pos, int value)
{
    bitmap.data[pos.y * bitmap.width + pos.x] &= (~NORMAL_MAP_MASK);
    bitmap.data[pos.y * bitmap.width + pos.x] |= normal_map_encode(value);
}

void set_light_map_value(Bitmap bitmap, Vec2i pos, int value)
{
    // assert(value >= 0 && value <= LIGHT_RADIUS);
    bitmap.data[pos.y * bitmap.width + pos.x] &= (~LIGHT_MAP_MASK);
    bitmap.data[pos.y * bitmap.width + pos.x] |= light_map_encode(value);
}

int get_light_map_value(Bitmap bitmap, Vec2i pos)
{
    return light_map_decode(bitmap.data[pos.y * bitmap.width + pos.x]);
}

int get_normal_map_value(Bitmap bitmap, Vec2i pos)
{
    return normal_map_decode(bitmap.data[pos.y * bitmap.width + pos.x]);
}

int get_dist_map_value(Bitmap bitmap, Vec2i pos)
{
    return dist_map_decode(bitmap.data[pos.y * bitmap.width + pos.x]);
}

void add_light_map_value(Bitmap bitmap, Vec2i pos, int value)
{
    int result = value + get_light_map_value(bitmap, pos);
    // if (result > LIGHT_RADIUS) {
    // add_term_line("%d\n", result);
    // }
    // if (result > LIGHT_RADIUS)
    //     result = LIGHT_RADIUS;
    set_light_map_value(bitmap, pos, result);
}

void set_dist_map_value(Bitmap bitmap, Vec2i pos, int value)
{
    // assert(value >= 0 && value <= MAX_DIST);
    bitmap.data[pos.y * bitmap.width + pos.x] &= (~DIST_MAP_MASK);
    bitmap.data[pos.y * bitmap.width + pos.x] |= dist_map_encode(value);
}

void apply_horizontal_tunnel(int x1, int x2, int y, Bitmap bitmap)
{
    int start = x1 < x2 ? x1 : x2;
    int end = x1 > x2 ? x1 : x2;

    for (int x = start; x <= end; x++)
    {
        set_normal_map_value(bitmap, (Vec2i){x, y}, WALKABLE);
        if (y + 1 < bitmap.height) // Verifica se não estamos no limite do bitmap
        {
            set_normal_map_value(bitmap, (Vec2i){x, y + 1}, WALKABLE);
        }
    }
}

void apply_vertical_tunnel(int y1, int y2, int x, Bitmap bitmap)
{
    int start = y1 < y2 ? y1 : y2;
    int end = y1 > y2 ? y1 : y2;

    for (int y = start; y <= end; y++)
    {
        set_normal_map_value(bitmap, (Vec2i){x, y}, WALKABLE);
        if (x + 1 < bitmap.width) // Verifica se não estamos no limite do bitmap
        {
            set_normal_map_value(bitmap, (Vec2i){x + 1, y}, WALKABLE);
        }
    }
}

int radius_count(Bitmap bitmap, int x, int y, int r)
{
    int result = 0;

    int xmin = MAX(x - r, 0);
    int xmax = MIN(x + r, bitmap.width - 1);
    for (int i = xmin; i <= xmax; i++)
    {
        int ymin = MAX(y - r, 0);
        int ymax = MIN(y + r, bitmap.height - 1);
        for (int j = ymin; j <= ymax; j++)
        {
            if (normal_map_decode(bitmap.data[j * bitmap.width + i]) == WALL)
                result++;
        }
    }
    return result;
}

void generate_obstacles(Bitmap bitmap, Rect rect2)
{
    Rect rect = rect2;
    for (int x = rect.tl.x; x < rect.br.x; x++)
    {
        for (int y = rect.tl.y; y < rect.br.y; y++)
        {
            if (rand() % 100 < 10 && radius_count(bitmap, x, y, 4) <= 10)
            {
                set_normal_map_value(bitmap, (Vec2i){x, y}, WALL);
            }
        }
    }
    for (int k = 0; k < 100; k++)
    {
        for (int x = rect.tl.x; x < rect.br.x; x++)
        {
            for (int y = rect.tl.y; y < rect.br.y; y++)
            {
                if (radius_count(bitmap, x, y, 1) >= 4)
                {
                    set_normal_map_value(bitmap, (Vec2i){x, y}, WALL);
                }
            }
        }
    }
    for (int x = rect.tl.x; x < rect.br.x; x++)
    {
        for (int y = rect.tl.y; y < rect.br.y; y++)
        {
            if (radius_count(bitmap, x, y, 2) <= 4 && normal_map_decode(bitmap.data[y * bitmap.width + x]) == WALL)
            {
                set_normal_map_value(bitmap, (Vec2i){x, y}, WALKABLE);
            }
        }
    }
}

void generate_tunnels_and_rasterize(Bitmap bitmap, Rect *rects, int rect_count)
{
    bitmap_draw_rectangle(bitmap, rects[0]);
    for (int i = 1; i < rect_count; i++)
    {
        bitmap_draw_rectangle(bitmap, rects[i]);
        Vec2i prev_center = get_center(rects[i - 1]);
        Vec2i new_center = get_center(rects[i]);

        // add_term_line("P:%d,%d N:%d,%d", prev_center.x, prev_center.y,
        // new_center.x, new_center.y);

        if (rand() % 2 == 1)
        {
            apply_horizontal_tunnel(prev_center.x, new_center.x, prev_center.y, bitmap);
            apply_vertical_tunnel(prev_center.y, new_center.y, new_center.x, bitmap);
        }
        else
        {
            apply_vertical_tunnel(prev_center.y, new_center.y, prev_center.x, bitmap);
            apply_horizontal_tunnel(prev_center.x, new_center.x, new_center.y, bitmap);
        }
    }
}

int generate_rects(Rect window, Rect *rects, int rects_max)
{
    int rects_count = 0;

    int div = 2;
    int rects_per_div = rects_max / (div * div);
    for (int f = 0; f < div * div; f++)
    {
        for (int i = 0; i < rects_per_div; i++)
        {
            Rect new_rect;
            int valid_new_rect;
            Rect sub = subdivide_rect(window, div, f);
            for (int j = 0; j < 255; j++)
            {
                valid_new_rect = 1;
                new_rect = gen_random_subrect(sub);
                for (int n = 0; n < rects_count; n++)
                {
                    if (collide_rect_rect(rects[n], expand_rect(new_rect, 2)))
                    {
                        valid_new_rect = 0;
                        break;
                    }
                }
                if (valid_new_rect)
                    break;
            }
            if (valid_new_rect)
            {
                rects[rects_count++] = new_rect;
                add_term_line("n %d", rects_count);
            }
        }
    }

    return rects_count;
}

void order_rects(Rect *rects, int rects_count)
{
    for (int i = 0; i < rects_count; i++)
    {
        for (int j = 0; j < rects_count; j++)
        {
            if (rects[i].tl.x < rects[j].tl.x)
            {
                Rect temp = rects[i];
                rects[i] = rects[j];
                rects[j] = temp;
            }
        }
    }
}

void erode(Bitmap bitmap, int iterations)
{
    // Erode com DLA
    for (int i = 0; i < iterations; i++)
    {
        // Encontre todas as células abertas
        int open_tiles[bitmap.width * bitmap.height];
        int open_tile_count = 0;

        for (int j = 0; j < bitmap.width * bitmap.height; j++)
        {
            if (normal_map_decode(bitmap.data[j]) == WALKABLE)
            {
                open_tiles[open_tile_count++] = j;
            }
        }

        // Escolha uma célula aberta aleatoriamente
        int random_index = rand() % open_tile_count;
        int digger = open_tiles[random_index];
        int digger_x = digger % bitmap.width;
        int digger_y = digger / bitmap.width;

        // Enquanto a célula escolhida ainda for um '#', escolha uma direção
        // aleatória e mova o "digger"
        int cap = 1000;
        while (normal_map_decode(bitmap.data[digger]) == WALKABLE && cap-- > 0)
        {
            int direction = rand() % 4;

            switch (direction)
            {
            case 0: // Mova para a esquerda
                if (digger_x > 2)
                {
                    digger_x -= 1;
                }
                break;
            case 1: // Mova para a direita
                if (digger_x < bitmap.width - 2)
                {
                    digger_x += 1;
                }
                break;
            case 2: // Mova para cima
                if (digger_y > 2)
                {
                    digger_y -= 1;
                }
                break;
            case 3: // Mova para baixo
                if (digger_y < bitmap.height - 2)
                {
                    digger_y += 1;
                }
                break;
            }

            digger = digger_y * bitmap.width + digger_x;
        }

        // Marque a nova célula como um '#'
        set_normal_map_value(bitmap, (Vec2i){digger_x, digger_y}, WALKABLE);
    }
}

int map_is_wall(Bitmap pixmap, Vec2f pos)
{
    int data = get_normal_map_value(pixmap, vec2f_to_i(pos));
    return data == WALL || data == SHINE;
}

void generate_spikes(Bitmap pixmap, Rect rect2)
{
    Rect rect = rect2;
    for (int x = rect.tl.x; x < rect.br.x; x++)
    {
        for (int y = rect.tl.y; y < rect.br.y; y++)
        {
            Vec2i pos = (Vec2i){x, y};
            if (get_normal_map_value(pixmap, pos) == WALKABLE && rand() % 100 < 1)
            { // 3.5% chance to place a spike
                set_normal_map_value(pixmap, pos, SPIKE);
            }
        }
    }
}

Inventory generate_chest_items()
{
    Inventory result = {0};
    int num_items = random_between(1, 3);
    for (int j = 0; j < num_items; j++)
    {
        int item = random_between(0, Item__Size);
        int count = random_between(1, 3);
        add_item(&result, item, count);
    }
    return result;
}

Chest generate_chest(Rect rect2)
{
    Rect rect = gen_subrect_with_size((Vec2i){4, 3}, rect2);
    Inventory inventory = generate_chest_items();
    return (Chest){inventory, rect, 0};
}

void draw_chest(WINDOW *win, Rect rect)
{
    for (int dx = 0; dx < 4; dx++)
    {
        for (int dy = 0; dy < 3; dy++)
        {
            Vec2i pos = (Vec2i){rect.tl.x + dx, rect.tl.y + dy};
            if (dy == 1 && (dx == 1 || dx == 2))
            {
                wattrset(win, COLOR_PAIR(Culur_Chest));
            }
            else
            {
                wattrset(win, COLOR_PAIR(Culur_Outer_Chest));
            }
            print_pixel(win, pos.x, pos.y);
        }
    }
}

Rect generate_portal(Rect rect2)
{
    Rect rect = gen_subrect_with_size((Vec2i){5, 4}, rect2);
    return rect;
}

void draw_portal(WINDOW *win, Rect rect)
{
    for (int dx = 0; dx < 5; dx++)
    {
        for (int dy = 0; dy < 4; dy++)
        {
            Vec2i pos = (Vec2i){rect.tl.x + dx, rect.tl.y + dy};
            if ((dy == 1 || dy == 2) && (dx == 1 || dx == 2 || dx == 3))
            {
                wattrset(win, COLOR_PAIR(Culur_Portal));
            }
            else
            {
                wattrset(win, COLOR_PAIR(Culur_Outer_Portal));
            }
            print_pixel(win, pos.x, pos.y);
        }
    }
}

int map_is_walkable(Bitmap pixmap, Vec2f pos, Vec2f inc)
{
    Vec2f inc_x = {inc.x, 0};
    Vec2f inc_y = {0, inc.y};
    return ((!map_is_wall(pixmap, vec2f_add(pos, inc_x)) || !map_is_wall(pixmap, vec2f_add(pos, inc_y))) &&
            !map_is_wall(pixmap, vec2f_add(pos, inc)));
}

void render_map(WINDOW *win_game, Camera camera, Bitmap map, WINDOW *window, Bitmap illuminated)
{
    for (int x = 0; x < camera.width; ++x)
    {
        for (int y = 0; y < camera.height; ++y)
        {
            int map_x = x + camera.x;
            int map_y = y + camera.y;
            if (!is_between(map_x, 0, map.width) || !is_between(map_y, 0, map.height))
            {
                continue;
            }
            uint32_t data = map.data[map_y * map.width + map_x];

            if (normal_map_decode(data) == WALKABLE)
            {
                wattrset(win_game, COLOR_PAIR(Culur_Light_Gradient + MIN(light_map_decode(data), LIGHT_RADIUS - 1)));
                print_pixel(window, x, y);
            }
            if (normal_map_decode(data) == SHINE)
            {
                wattrset(win_game, COLOR_PAIR(Culur_Shine));
                print_pixel(window, x, y);
            }
            else if (normal_map_decode(data) == SPIKE)
            {
                wattrset(win_game, COLOR_PAIR(Culur_Spike));
                print_pixel_custom(window, x, y, "^");
            }
            else if (normal_map_decode(data) == PORTAL)
            {
                wattrset(win_game, COLOR_PAIR(Culur_Portal));
                print_pixel(window, x, y);
            }
            else if (normal_map_decode(data) == OUTER_PORTAL)
            {
                wattrset(win_game, COLOR_PAIR(Culur_Outer_Portal));
                print_pixel(window, x, y);
            }
            else
            {
                if (get_normal_map_value(illuminated, (Vec2i){map_x, map_y}) == SHINE)
                {
                    wattrset(win_game, COLOR_PAIR(Culur_Shine_Dimmed));
                    print_pixel(window, x, y);
                }
            }
        }
    }
}

void box_sampling_scale(Bitmap illuminated, Vec2i window_size, Bitmap scaled, int size)
{
    // Use box sampling to downscale the image and sample a square of size
    // size*size
    float scale_x = (float)illuminated.width / window_size.x;
    float scale_y = (float)illuminated.height / window_size.y;

    for (int y = 0; y < window_size.y; y++)
    {
        for (int x = 0; x < window_size.x; x++)
        {
            float map_x = x * scale_x;
            float map_y = y * scale_y;
            // Box sampling
            int map_x1 = floor(map_x) - size / 2;
            int map_y1 = floor(map_y) - size / 2;
            int map_x2 = map_x1 + size;
            int map_y2 = map_y1 + size;
            float sum = 0;
            for (int i = map_x1; i <= map_x2; i++)
            {
                for (int j = map_y1; j <= map_y2; j++)
                {
                    sum += get_normal_map_value(illuminated, (Vec2i){i, j});
                }
            }
            float average = sum / (size * size);
            set_normal_map_value(scaled, (Vec2i){x, y}, average * (LIGHT_RADIUS - 1));
        }
    }

}

void bilinear_scale(Bitmap illuminated, Vec2i window_size, Bitmap scaled)
{
    float scale_x = (float)illuminated.width / window_size.x;
    float scale_y = (float)illuminated.height / window_size.y;

    for (int y = 0; y < window_size.y; y++)
    {
        for (int x = 0; x < window_size.x; x++)
        {
            float map_x = x * scale_x;
            float map_y = y * scale_y;
            // Bilinear interpolation
            int map_x1 = floor(map_x);
            int map_x2 = ceil(map_x);
            int map_y1 = floor(map_y);
            int map_y2 = ceil(map_y);
            float x_ratio = map_x - map_x1;
            float y_ratio = map_y - map_y1;
            float x_opposite = 1 - x_ratio;
            float y_opposite = 1 - y_ratio;
            float top_left = illuminated.data[map_y1 * illuminated.width + map_x1] == WALKABLE ? LIGHT_RADIUS -1 : 0;
            float top_right = illuminated.data[map_y1 * illuminated.width + map_x2] == WALKABLE ? LIGHT_RADIUS -1 : 0;
            float bottom_left = illuminated.data[map_y2 * illuminated.width + map_x1] == WALKABLE ? LIGHT_RADIUS -1: 0;
            float bottom_right = illuminated.data[map_y2 * illuminated.width + map_x2] == WALKABLE ? LIGHT_RADIUS -1: 0;
            float top = top_left * x_opposite + top_right * x_ratio;
            float bottom = bottom_left * x_opposite + bottom_right * x_ratio;
            float value = top * y_opposite + bottom * y_ratio;

            set_normal_map_value(scaled, (Vec2i){x, y}, value);
        }
    }
}

void guassian_blur(Bitmap map, Bitmap temp)
{
    float kernel[3][3] = {
        {0.0625, 0.125, 0.0625},
        {0.125, 0.25, 0.125},
        {0.0625, 0.125, 0.0625}};
    for (int y = 0; y < map.height; y++)
    {
        for (int x = 0; x < map.width; x++)
        {
            float value = 0;
            for (int ky = 0; ky < 3; ky++)
            {
                for (int kx = 0; kx < 3; kx++)
                {
                    int map_x = x + kx - 1;
                    int map_y = y + ky - 1;
                    if (is_between(map_x, 0, map.width) && is_between(map_y, 0, map.height))
                    {
                        value += get_normal_map_value(map, (Vec2i){map_x, map_y}) * kernel[ky][kx];
                    }
                }
            }
            set_normal_map_value(temp, (Vec2i){x, y}, value);
        }
    }
    for (int y = 0; y < map.height; y++)
    {
        for (int x = 0; x < map.width; x++)
        {
            set_normal_map_value(map, (Vec2i){x, y}, get_normal_map_value(temp, (Vec2i){x, y}));
        }
    }
}

void render_minimap(WINDOW *win, Bitmap illuminated, Vec2i window_size, Vec2i player_pos)
{
    Bitmap scaled = alloc_bitmap(window_size.x, window_size.y);
    // bilinear_scale(illuminated, window_size, scaled);
    box_sampling_scale(illuminated, window_size, scaled, 4);

    Bitmap blurred = scaled;
    // Bitmap blurred = alloc_bitmap(window_size.x, window_size.y);
    // guassian_blur(scaled, blurred);
    // free_bitmap(scaled);

    for (int y = 0; y < window_size.y; y++)
    {
        for (int x = 0; x < window_size.x; x++)
        {
            int value = get_normal_map_value(blurred, (Vec2i){x, y});
            wattrset(win, COLOR_PAIR(Culur_Light_Gradient + MIN(value, LIGHT_RADIUS - 1)));
            print_pixel(win, x, y);
        }
    }

    free_bitmap(blurred);

    float scale_x = (float)illuminated.width / window_size.x;
    float scale_y = (float)illuminated.height / window_size.y;

    wattrset(win, COLOR_PAIR(Color_Blue));
    int player_x = player_pos.x / scale_x;
    int player_y = player_pos.y / scale_y;
    print_pixel(win, player_x, player_y);
}
