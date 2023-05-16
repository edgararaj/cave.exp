#include "mobs.h"
#include "map.h"
#include "objects.h"

void create_mobs(Bitmap pixmap, Mob *mobs, int num_mobs)
{
  int width = 1;
  int height = 1;
  for (int i = 0; i < num_mobs; i++)
  {
    int x, y;
    do
    {
      x = random_between(0, pixmap.width - width);
      y = random_between(0, pixmap.height - height);
    } while (pixmap.data[y * pixmap.width + x] != WALKABLE);

    mobs[i].type = random_between(0, MobType__Size);
    mobs[i].rect = (RectFloat){{x, y}, {x + width - 1, y + height - 1}, 6};
    mobs[i].dmg = random_between(2, 20);
    mobs[i].speed = random_between(1, 5);
    mobs[i].hp = random_between(10, 100);
  }
}

void update_mob(Mob *mob, Bitmap map)
{
  if (map.data[(int)mob->rect.tl.y * map.width + (int)mob->rect.tl.x] >= DIST_BASE)
  {
    int smallest = MAX_DIST;
    Vec2i smallest_add = {0, 0};
    for (int i = -1; i < 2; i++)
    {
      for (int j = -1; j < 2; j++)
      {
        if (i + j == -1 || i + j == 1)
        {
          Vec2i add = {i, j};
          RectFloat rect = rect_float_translate(mob->rect, vec2i_to_f(add));
          int data = map.data[(int)(rect.tl.y) * map.width + (int)rect.tl.x];
          if (data < smallest && data >= DIST_BASE && data < MAX_DIST)
          {
            smallest = map.data[(int)(rect.tl.y) * map.width + (int)rect.tl.x];
            smallest_add = add;
          }
        }
      }
    }
    mob->rect = rect_float_translate(mob->rect, vec2f_div_const(vec2i_to_f(smallest_add), mob->speed * 10));
  }
}

void update_mobs(Mob *mobs, int num_mobs, Vec2i player, Bitmap map)
{
  int distsqr = 200;
  for (int i = 0; i < num_mobs; i++)
  {
    if (vec2i_sqrdistance(vec2i_sub(vec2f_to_i(mobs[i].rect.tl), player)) < distsqr)
    {
      update_mob(&mobs[i], map);
    }
  }
}