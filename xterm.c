#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "screen.h"
#include "utils.h"

#define XSTR(s) STR(s)
#define STR(s) #s

typedef struct {
    char *name;
    char *value;
} Xresource;

char *get_xresource(char *start, char *end) {
    Xresource resources[] = {{"FONTSIZE", XSTR(FONT_SIZE)},
                             {"WIDTH", XSTR(TERM_WIDTH)},
                             {"HEIGHT", XSTR(TERM_HEIGHT)}};

    for (size_t i = 0; i < ARRAY_SIZE(resources); i++) {
        if (strncmp(start, resources[i].name, end - start) == 0) {
            return resources[i].value;
        }
    }
    assert(!"Xresource not found");
    return 0;
}

void setup_xresources() {
    char file[256];
    strcat(strcpy(file, getenv("HOME")), "/.Xresources");
    FILE *w = fopen(file, "w");
    FILE *f = fopen(".Xresources.template", "r");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *temp = malloc(fsize + 1);
    fread(temp, fsize, 1, f);

    char *find = temp;
    while (1) {
        char *start = strstr(find, "<!");
        if (start == NULL)
            break;
        char *end = strstr(start, ">");
        char *value = get_xresource(start + 2, end);
        fwrite(find, 1, start - find, w);
        fwrite(value, 1, strlen(value), w);
        find = end + 1;
    }
    fwrite(find, 1, temp + strlen(temp) - find, w);
    system("xrdb ~/.Xresources");
}
