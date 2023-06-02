#include "utils.h"
#include <stdlib.h>

int random_between(int min, int max)
{
    if (min == max)
        return min;
    return rand() % (max - min) + min;
}

int timer_update(int *left, int delta)
{
    *left -= 1;
    if (*left < 0)
    {
        *left = 0;
        return 1;
    }
    return 0;
}

int is_between(int value, int min, int max)
{
    return value >= min && value <= max;
}

int cap_between(int value, int min, int max)
{
    if (value < min)
    {
        return min;
    }
    else if (value > max)
    {
        return max;
    }
    return value;
}