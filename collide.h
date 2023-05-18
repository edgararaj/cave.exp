#pragma once
#include "objects.h"
int collide_ellipse_line(Ellipse ellipse, Line line);
int collide_circle_line(Circle ellipse, Line line);
int collide_rect_rect(Rect a, Rect b);
int collide_rect_bitmap(Rect rect, Bitmap bitmap);