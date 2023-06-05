#include "colors.h"
#define LIGHT_RADIUS 20

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
    for (int i = 0; i < num_pairs; i++) // Cores realistas interpolacao logaritmica
    {
        float exp = (float)i / (float)num_pairs;
        float num = powf(2, exp) - 1;
        ts[i] = num;
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

void setup_colors(void)
{
    init_color(Color_Black, 0, 0, 0);
    init_color(Color_Red, 1000, 0, 0);
    init_color(Color_Green, 0, 1000, 0);
    init_color(Color_Yellow, 1000, 1000, 0);
    init_color(Color_Blue, 0, 0, 1000);
    init_color(Color_Magenta, 1000, 0, 1000);
    init_color(Color_Cyan, 0, 1000, 1000);
    init_color(Color_White, 1000, 1000, 1000);

    init_color(Culur_Black, 0, 0, 0);
    init_color(Culur_Gray, 300, 300, 300);
    init_color(Culur_Shine, 900, 0, 0);
    init_color(Culur_Shine_Dimmed, 200, 0, 0);
    init_color(Culur_Spike_Front, 500, 500, 500);
    init_color(Culur_Spike_Back, 100, 100, 100);
    init_color(Culur_Outer_Chest, 500, 250, 0);
    init_color(Culur_Chest, 1000, 1000, 0);
    init_color(Culur_Outer_Portal, 501, 0, 501);
    init_color(Culur_Portal, 784, 392, 784);
    init_color(Culur_Hotbar_Back, 500, 500, 500);
    init_color(Culur_Hotbar_Front, 1000, 1000, 1000);
    init_color(Culur_Hotbar_Selected_Back, 500, 500, 0);
    init_color(Culur_Hotbar_Selected_Front, 0, 1000, 0);
    init_color(Culur_Menu_Background, 0, 0, 0);   // Preto
    init_color(Culur_Menu_Text, 1000, 0, 0);      // Vermelho
    init_color(Culur_Menu_Highlight_Background, 500, 500, 500); // Cinza
    init_color(Culur_Menu_Highlight_Text, 1000, 1000, 1000);    // Branco

    int i;
    for (i = 0; i < Color__RGBSize; i++)
    {
        init_pair(i, i, i);
    }
    init_pair(Culur_Default, Color_White, Color_Black);
    init_pair(Culur_Default_Gray, Culur_Gray, Color_Black);
    init_pair(Culur_Default_Green, Color_Green, Color_Black);
    init_pair(Culur_Default_Blue, Color_Blue, Color_Black);
    init_pair(Culur_Default_Red, Color_Red, Color_Black);
    init_pair(Culur_Spike, Culur_Spike_Front, Culur_Spike_Back);
    init_pair(Culur_Hotbar, Culur_Hotbar_Front, Culur_Hotbar_Back);
    init_pair(Culur_Hotbar_Selected, Culur_Hotbar_Selected_Front, Culur_Hotbar_Selected_Back);
    init_pair(Culur_Menu, Culur_Menu_Text, Culur_Menu_Background);
    init_pair(Culur_Menu_Highlight, Culur_Menu_Highlight_Text, Culur_Menu_Highlight_Background);

    // Definir as cores base para o gradiente (valores entre 0 e 1000)
    short end_color[3] = {700, 500, 20}; // amarelo
    short start_color[3] = {0, 0, 0};    // preto

    init_gradient_color_pairs(start_color, end_color, LIGHT_RADIUS, Culur_Light_Gradient);
}
