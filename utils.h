#pragma once

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define SIGN(x) ((x < 0) ? -1 : (x > 0))
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
int random_between(int min, int max);
