#pragma once

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define SIGN(x) (((x) < 0) ? -1 : ((x) > 0))
#define ABS(x) (((x) < 0) ? -(x) : (x))
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
int random_between(int min, int max);
int timer_update(int *left, int delta);
int is_between(int value, int min, int max);
int cap_between(int value, int min, int max);
