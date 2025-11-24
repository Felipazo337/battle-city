#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CELL_SIZE_ORIGINAL 32

int** mapa = NULL;
int ROWS = 0, COLS = 0;
SDL_Texture* tiles[16] = {NULL};

const char* nombres_texturas[] = {
    "empty.png", "brick.png", "steel.png", "water.png",
    "bush.png", "eagle.png", "player.png", "player2.png",
    "enemy2.png", NULL
};

int cargar_mapa_desde_archivo(const char* archivo) {
    FILE* f = fopen(archivo, "r");
    if (!f) { printf("No se encontró %s\n", archivo); return 0; }

    ROWS = 44;
    COLS = 41;

    mapa = malloc(ROWS * sizeof(int*));
    for (int i = 0; i < ROWS; i++) mapa[i] = malloc(COLS * sizeof(int));

    char linea[256];
    int fila = 0;
    while (fila < ROWS && fgets(linea, sizeof(linea), f)) {
        linea[strcspn(linea, "\r\n")] = '\0';
        if (strlen(linea) < 41) continue;
        for (int j = 0; j < COLS; j++) {
            mapa[fila][j] = linea[j] - '0';
        }
        fila++;
    }
    ROWS = fila;
    fclose(f);
    return 1;
}

void liberar_mapa() {
    if (mapa) {
        for (int i = 0; i < ROWS; i++) free(mapa[i]);
        free(mapa);
        mapa = NULL;
    }
}

int main(int argc, char* argv[]) {
    if (!cargar_mapa_desde_archivo("mapa_salida.txt")) return 1;


    mapa[4][4] = 6;

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    SDL_Window* win = SDL_CreateWindow("Battle City - Tarea Semestral",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1400, 900,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);

    if (!win) { printf("Error ventana: %s\n", SDL_GetError()); return 1; }

    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ren) { printf("Error renderer: %s\n", SDL_GetError()); return 1; }

    for (int i = 0; nombres_texturas[i]; i++) {
        tiles[i] = IMG_LoadTexture(ren, nombres_texturas[i]);
    }

    int running = 1;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;
        }

        int w, h;
        SDL_GetWindowSize(win, &w, &h);
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);

        float scale = fminf((float)w / (COLS * CELL_SIZE_ORIGINAL),
                            (float)h / (ROWS * CELL_SIZE_ORIGINAL));

        int cell = (int)(CELL_SIZE_ORIGINAL * scale);
        int offset_x = (w - COLS * cell) / 2;
        int offset_y = (h - ROWS * cell) / 2;

        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                int val = mapa[i][j];

                
                int dibujar_como = val;
                if (val == 1) dibujar_como = 2;
                else if (val == 2) dibujar_como = 1;

                SDL_Rect dst = { offset_x + j * cell, offset_y + i * cell, cell, cell };

                if (dibujar_como > 0 && dibujar_como < 16 && tiles[dibujar_como]) {
                    SDL_RenderCopy(ren, tiles[dibujar_como], NULL, &dst);
                } else {
                    switch (dibujar_como) {
                        case 1: SDL_SetRenderDrawColor(ren, 200, 80, 40, 255);   break; // ladrillo
                        case 2: SDL_SetRenderDrawColor(ren, 150, 150, 150, 255); break; // acero
                        case 6: SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);     break; // player
                        default: SDL_SetRenderDrawColor(ren, 40, 40, 40, 255);  break;
                    }
                    SDL_RenderFillRect(ren, &dst);
                }
            }
        }
        SDL_RenderPresent(ren);
    }

 
    for (int i = 0; i < 16; i++) if (tiles[i]) SDL_DestroyTexture(tiles[i]);
    liberar_mapa();
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    IMG_Quit();
    SDL_Quit();
    return 0;
}