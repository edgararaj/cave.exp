#include "camera.h"
#include "light.h"
#include "map.h"
#include "screen.h"

void ensure_camera_inside_map(Camera *camera)
{
    // Garante que a câmera não sai dos limites do mapa
    if (camera->x < 0)
        camera->x = 0;
    if (camera->y < 0)
        camera->y = 0;
    if (camera->x + camera->width > MAP_WIDTH)
        camera->x = MAP_WIDTH - camera->width;
    if (camera->y + camera->height > MAP_HEIGHT)
        camera->y = MAP_HEIGHT - camera->height;
}

void update_camera(Camera *camera, Vec2i player)
{
    int player_x = player.x;
    int player_y = player.y;
    // Verifica se o jogador está perto das bordas da câmera
    if (player_x - camera->x < camera->margin) // Se o jogador está perto da borda esquerda
    {
        camera->x = player_x - camera->margin;
    }
    else if (player_x - camera->x > camera->width - camera->margin) // Se o jogador está perto da borda direita
    {
        camera->x = player_x - camera->width + camera->margin;
    }
    if (player_y - camera->y < camera->margin) // Se o jogador está perto da borda superior
    {
        camera->y = player_y - camera->margin;
    }
    else if (player_y - camera->y > camera->height - camera->margin) // Se o jogador está perto da borda inferior
    {
        camera->y = player_y - camera->height + camera->margin;
    }

    ensure_camera_inside_map(camera);
}

void center_camera(Camera *camera, Vec2i player)
{
    int x = player.x;
    int y = player.y;
    camera->x = x - camera->width / 2;
    camera->y = y - camera->height / 2;

    ensure_camera_inside_map(camera);
}
