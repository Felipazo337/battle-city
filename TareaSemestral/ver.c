#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "headers/def.h"
#include "headers/logica.h"

void generar_mapa_completo(int **mapa_destino);
// Declaraciones de funciones del generador
void bordearmapa(int, int, int**, int);
void rellenar(int, int, int*, int**, int);
void random(int*, int, int);

// Variables globales del juego
int **mapa = NULL;
Tanque jugador1, jugador2;
Bala bala1, bala2;

// Texturas
SDL_Texture* tiles[10] = {NULL}; 
const char* nombres_texturas[] = {
// ÍNDICE 0 (VACIO)
    "assets/empty.png", 
// ÍNDICE 1 (MURO)
    "assets/brick.png", 
// ÍNDICE 2 (NO USADO)
    NULL, 
// ÍNDICE 3 (IRROMPIBLE)
    "assets/steel.png",
// ÍNDICE 4 (BUSH)
    "assets/bush.png", 
// ÍNDICE 5 (AGUA)
    "assets/water.png", 
// ÍNDICE 6 (JUGADOR1)
    "assets/player.png",
// ÍNDICE 7 (JUGADOR2)
    "assets/player2.png",
// ÍNDICE 8 (TP1)
    "assets/tp.png", 
// ÍNDICE 9 (TP2)
    "assets/tp.png",
// ÍNDICE 10 (NO USADO)
    NULL 
};

#define CELL_SIZE 32

// Generar mapa (
void generar_mapa() {
    mapa = malloc(FILAS * sizeof(int*));
    for(int i = 0; i < FILAS; i++) 
        mapa[i] = calloc(COLUMNAS, sizeof(int));
    
    generar_mapa_completo(mapa);
}


int main(int argc, char* argv[]) {
    generar_mapa();

    inicializar_tanque(&jugador1, JUGADOR1, TBLOQUE-1, TBLOQUE-1);
    inicializar_tanque(&jugador2, JUGADOR2, COLUMNAS-TBLOQUE, FILAS-TBLOQUE);
    bala1.activa = bala2.activa = 0;

    escanear_tp(mapa);
    dibujar_tanque(mapa, &jugador1);
    dibujar_tanque(mapa, &jugador2);

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    SDL_Window* win = SDL_CreateWindow("Battle City Remake - ¡FUNCIONA!",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1200, 800, SDL_WINDOW_RESIZABLE);

    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // Cargar texturas
for(int i = 0; i <= 9; i++) {
    if(nombres_texturas[i]) {
        SDL_Surface* surf = IMG_Load(nombres_texturas[i]);
        if(surf) tiles[i] = SDL_CreateTextureFromSurface(ren, surf);
        SDL_FreeSurface(surf);
    }
}

    int running = 1;
    SDL_Event e;
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    Uint64 last_time = SDL_GetTicks64();
    int turno = 0;

    while(running) {
        Uint64 now = SDL_GetTicks64();
        if(now - last_time < 60) { SDL_Delay(5); continue; } // ~16 FPS
        last_time = now;

        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) running = 0;
        }

        // Controles jugador 1 (WASD + G para disparar)
        if(keys[SDL_SCANCODE_W]) mover_tanque(&jugador1, 'w', mapa);
        if(keys[SDL_SCANCODE_S]) mover_tanque(&jugador1, 's', mapa);
        if(keys[SDL_SCANCODE_A]) mover_tanque(&jugador1, 'a', mapa);
        if(keys[SDL_SCANCODE_D]) mover_tanque(&jugador1, 'd', mapa);
        if(keys[SDL_SCANCODE_G] && !bala1.activa) disparar(&bala1, &jugador1);

        // Controles jugador 2 (flechas + K para disparar)
        if(keys[SDL_SCANCODE_UP]) mover_tanque(&jugador2, 'w', mapa);
        if(keys[SDL_SCANCODE_DOWN]) mover_tanque(&jugador2, 's', mapa);
        if(keys[SDL_SCANCODE_LEFT]) mover_tanque(&jugador2, 'a', mapa);
        if(keys[SDL_SCANCODE_RIGHT]) mover_tanque(&jugador2, 'd', mapa);
        if(keys[SDL_SCANCODE_K] && !bala2.activa) disparar(&bala2, &jugador2);

        // Actualizar balas
        if(bala1.activa) actualizar_bala(&bala1, mapa, &jugador2, &jugador1);
        if(bala2.activa) actualizar_bala(&bala2, mapa, &jugador1, &jugador2);

        int ganador = evaluar_victoria(&jugador1, &jugador2, turno++);
        if(ganador) {
            printf("¡Juego terminado! Ganador: %s\n", ganador==1?"Jugador 1":ganador==2?"Jugador 2":"Empate");
            running = 0;
        }

        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);

        float scale = fminf((float)1200 / (COLUMNAS * CELL_SIZE), (float)800 / (FILAS * CELL_SIZE));
        int cell = CELL_SIZE * scale;
        int ox = (1200 - COLUMNAS * cell)/2;
        int oy = (800 - FILAS * cell)/2;

        for(int i = 0; i < FILAS; i++) {
            for(int j = 0; j < COLUMNAS; j++) {
                int val = mapa[i][j];
                SDL_Rect dst = {ox + j*cell, oy + i*cell, cell, cell};
                if(val >= 0 && val < 16 && tiles[val]) {
                    SDL_RenderCopy(ren, tiles[val], NULL, &dst);
                } else {
                    SDL_SetRenderDrawColor(ren, 100, 100, 100, 255);
                    SDL_RenderFillRect(ren, &dst);
                }
            }
        }

        // Dibujar balas
        if(bala1.activa) {
            SDL_SetRenderDrawColor(ren, 255, 255, 0, 255);
            SDL_Rect br = {ox + bala1.x*cell, oy + bala1.y*cell, cell, cell};
            SDL_RenderFillRect(ren, &br);
        }
        if(bala2.activa) {
            SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
            SDL_Rect br = {ox + bala2.x*cell, oy + bala2.y*cell, cell, cell};
            SDL_RenderFillRect(ren, &br);
        }

        SDL_RenderPresent(ren);
    }

    // Limpieza
    for(int i = 0; i < FILAS; i++) free(mapa[i]);
    free(mapa);
    for(int i = 0; i < 16; i++) if(tiles[i]) SDL_DestroyTexture(tiles[i]);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    IMG_Quit();
    SDL_Quit();
    return 0;
}