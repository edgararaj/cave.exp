#include "utils.h"
#include <stdlib.h>

int random_between(int min, int max) { return rand() % (max - min) + min; }

int timer_update(int *left, int delta)
{
    *left -= delta;
    if (*left < 0)
    {
        *left = 0;
        return 1;
    }
    return 0;
}
