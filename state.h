#pragma once
typedef struct state {
	int playerX;
	int playerY;
} STATE;


typedef struct
{
    int x;
    int y;
} Vec2i;

typedef struct
{
    Vec2i center;
    int radius;
} Circle;



typedef struct
{
    float x;
    float y;
} Vec2f;

typedef struct
{
    Vec2i start;
    Vec2i end;
} Line;


typedef struct
{
    Vec2i center;
    Vec2i radius;
} Ellipse;