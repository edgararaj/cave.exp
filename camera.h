#pragma once
#include "objects.h"

typedef struct {
    union {
        Vec2i offset;
        struct {
            int x, y; // Coordenadas da câmera
        };
    };
    int width, height; // Largura e altura da câmera
    int margin;        // Margem nas bordas da câmera
} Camera;

typedef enum {
    CameraMode_Margin,
    CameraMode_Follow,
    CameraMode__Size
} CameraMode;

void update_camera(Camera *camera, int player_x, int player_y);