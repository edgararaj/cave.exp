#include "colors.h"
#include "light.h"

// Função para interpolação linear entre duas cores
void lerp_color(short start_color[3], short end_color[3], float t, short *result_color)
{
    for (int i = 0; i < 3; i++)
    {
        result_color[i] = (short)(start_color[i] + t * (end_color[i] - start_color[i]));
    }
}

// Função para inicializar pares de cores com cores de gradiente
void init_gradient_color_pairs(short start_color[3], short end_color[3], int num_pairs, int base)
{
    float ts[num_pairs];
    for (int i = 0; i < num_pairs; i++)
    {
        ts[i] = powf(2, (float)i / (float)num_pairs) - 1;
    }
    for (int i = base; i < base + num_pairs; i++)
    {
        short gradient_color[3];
        lerp_color(start_color, end_color, ts[i - base], gradient_color);

        // Inicializar a cor do gradiente
        init_color(i, gradient_color[0], gradient_color[1], gradient_color[2]);
        init_pair(i, i, i);
    }
}

void setup_colors()
{
    init_color(Culur_Black, 0, 0, 0);
    init_color(Culur_Shine, 900, 0, 0);
    init_color(Culur_Shine_Dimmed, 200, 0, 0);
    int i;
    for (i = 0; i < Color__RGBSize; i++)
    {
        init_pair(i, i, i);
    }
    init_pair(Culur_Default, COLOR_WHITE, COLOR_BLACK);

    // Definir as cores base para o gradiente (valores entre 0 e 1000)
    short end_color[3] = {700, 500, 20}; // preto
    short start_color[3] = {0, 0, 0};    // branco

    init_gradient_color_pairs(start_color, end_color, LIGHT_RADIUS, Culur_Light_Gradient);
}