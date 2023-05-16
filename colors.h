#pragma once
#include "term.h"
#include <ncurses.h>
#include <stdlib.h>

#define LIGHT_BASE 11

// Função para interpolação linear entre duas cores
void lerp_color(short start_color[3], short end_color[3], float t,
                short *result_color) {
    for (int i = 0; i < 3; i++) {
        result_color[i] =
            (short)(start_color[i] + t * (end_color[i] - start_color[i]));
    }
}

// Função para inicializar pares de cores com cores de gradiente
void init_gradient_color_pairs(short start_color[3], short end_color[3],
                               int num_pairs, int base) {
    float ts[num_pairs];
    for (int i = 0; i < num_pairs; i++) {
        ts[i] = powf(2, (float)i / (float)num_pairs) - 1;
    }
    for (int i = base; i < base + num_pairs; i++) {
        short gradient_color[3];
        lerp_color(start_color, end_color, ts[i - base], gradient_color);

        // Inicializar a cor do gradiente
        init_color(i, gradient_color[0], gradient_color[1], gradient_color[2]);
        init_pair(i, i, i);
    }
}

void setup_colors() {
    init_pair(0, COLOR_WHITE, COLOR_BLACK);
    init_pair(1, COLOR_CYAN, COLOR_CYAN);
    init_pair(2, COLOR_RED, COLOR_RED);
    init_pair(3, COLOR_BLUE, COLOR_BLUE);
    init_pair(6, COLOR_YELLOW, COLOR_YELLOW);
    init_pair(8, COLOR_GREEN, COLOR_GREEN);
    init_pair(9, COLOR_WHITE, COLOR_WHITE);
    init_pair(10, COLOR_BLACK, COLOR_BLACK);
    // init_color(11, 100, 1000, 1000);
    // init_pair(11, 11, 11);
    // init_color(12, 900, 900, 900);
    // init_pair(12, 12, 12);
    // init_color(13, 800, 800, 800);
    // init_pair(13, 13, 13);
    // init_color(14, 700, 700, 700);
    // init_pair(14, 14, 14);

    // Definir as cores base para o gradiente (valores entre 0 e 1000)
    short start_color[3] = {1000, 1000, 1000}; // preto
    short end_color[3] = {0, 0, 0};            // branco

    // Número de cores intermediárias para gerar
    int num_pairs = LIGHT_RADIUS;

    // Verificar se o terminal suporta cores e o número mínimo de pares de cores
    // necessários
    init_gradient_color_pairs(start_color, end_color, num_pairs, LIGHT_BASE);
}