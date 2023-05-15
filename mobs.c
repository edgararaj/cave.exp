#include "mobs.h"
#include "map.h"
#include "objects.h"
#define MAX_MOBS 10

void create_mobs(Bitmap pixmap, Mob *mobs, int num_mobs)
{
  for (int i = 0; i < num_mobs; i++)
  {
    int x, y;
    do
    {
      x = random_between(0, pixmap.width);
      y = random_between(0, pixmap.height);
    } while (pixmap.data[y * pixmap.width + x] != WALKABLE);

    mobs[i].type = random_between(0, MobType__Size);
    mobs[i].rect = (Rect){{x, y}, {x, y}, 6};
    mobs[i].dmg = random_between(2, 20);
    mobs[i].speed = random_between(1, 5);
    mobs[i].hp = random_between(10, 100);
  }
}