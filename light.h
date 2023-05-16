#pragma once
#include "objects.h"

#define MAX_TORCHES 10
#define LIGHT_RADIUS 20

typedef struct {
    Rect position;
    int intensitiy;
} Torch;