#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "headers/def.h"
#include "headers/logica.h"
#ifndef FUNCMAPA_H
#define FUNCMAPA_H

void bordearmapa(int x,int y,int **m,int material){
    for(int i= 0; i<y; i++){
        for(int j= 0; j<x; j++){
            if((j==0) || (i==0) || (i==y-1) || (j==x-1)) m[i][j]= material;
        }
    }
}
void random(int *x, int max, int min){
    (*x)= (rand()%(max-(min)+1))+(min);
}
void rellenar(int nx,int ny,int *c,int **m, int material){
    if(m[ny][nx]==VACIO) (*c)++;
    m[ny][nx]= material;
}

#endif

void generar_mapa_completo(int **mapa_destino);

// Variables globales
int **mapa = NULL;
Tanque jugador1, jugador2;
Bala bala1, bala2;

// Texturas (solo 0 a 9 usadas)
SDL_Texture* tiles[10] = {NULL};
const char* nombres_texturas[] = {
    "assets/empty.png",     // 0
    "assets/brick.png",     // 1
    NULL,                   // 2 (no usado)
    "assets/steel.png",     // 3
    "assets/bush.png",      // 4
    "assets/water.png",     // 5
    "assets/player.png",    // 6 → JUGADOR 1
    "assets/player2.png",   // 7 → JUGADOR 2
    "assets/tp1.png",        // 8
    "assets/tp2.png"         // 9
};

// Generar mapa
void generar_mapa() {
    mapa = malloc(FILAS * sizeof(int*));
    for(int i = 0; i < FILAS; i++) 
        mapa[i] = calloc(COLUMNAS, sizeof(int));
    generar_mapa_completo(mapa);
}

int main(int argc, char* argv[]) {
    generar_mapa();

inicializar_tanque(&jugador1, JUGADOR1, 1, 1);                 // esquina superior izquierda
inicializar_tanque(&jugador2, JUGADOR2, COLUMNAS-2, FILAS-2);  // esquina inferior derecha (1x1) 
    bala1.activa = bala2.activa = 0;

    escanear_tp(mapa);
    dibujar_tanque(mapa, &jugador1);
    dibujar_tanque(mapa, &jugador2);

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    // Ventana ajustada al mapa 18x17
    SDL_Window* win = SDL_CreateWindow("Battle City",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1200, 900, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);  

    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // Cargar texturas
    for(int i = 0; i <= 9; i++) {
        if(nombres_texturas[i]) {
            SDL_Surface* surf = IMG_Load(nombres_texturas[i]);
            if(surf) {
                tiles[i] = SDL_CreateTextureFromSurface(ren, surf);
                SDL_FreeSurface(surf);
            } else {
                printf("Error cargando textura: %s\n", nombres_texturas[i]);
            }
        }
    }

    int running = 1;
    SDL_Event e;
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    Uint64 last_time = SDL_GetTicks64();
    int turno = 0;

    while(running) {
        Uint64 now = SDL_GetTicks64();
        if(now - last_time < 16) { SDL_Delay(1); continue; } // ~60 FPS
        last_time = now;

        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) running = 0;

            if(e.type == SDL_KEYDOWN) {
            SDL_Scancode sc = e.key.keysym.scancode;

            // Jugador 1
            if(sc == SDL_SCANCODE_W) { mover_tanque(&jugador1, 'w', mapa); turno++; }
            if(sc == SDL_SCANCODE_S) { mover_tanque(&jugador1, 's', mapa); turno++; }
            if(sc == SDL_SCANCODE_A) { mover_tanque(&jugador1, 'a', mapa); turno++; }
            if(sc == SDL_SCANCODE_D) { mover_tanque(&jugador1, 'd', mapa); turno++; }
            if(sc == SDL_SCANCODE_G && !bala1.activa) { disparar(&bala1, &jugador1); }

            // Jugador 2
            if(sc == SDL_SCANCODE_UP)    { mover_tanque(&jugador2, 'w', mapa); turno++; }
            if(sc == SDL_SCANCODE_DOWN)  { mover_tanque(&jugador2, 's', mapa); turno++; }
            if(sc == SDL_SCANCODE_LEFT)  { mover_tanque(&jugador2, 'a', mapa); turno++; }
            if(sc == SDL_SCANCODE_RIGHT) { mover_tanque(&jugador2, 'd', mapa); turno++; }
            if(sc == SDL_SCANCODE_K && !bala2.activa) { disparar(&bala2, &jugador2); }
            }
        }

        


        // Actualizar balas
        if(bala1.activa) actualizar_bala(&bala1, mapa, &jugador2, &jugador1);
        if(bala2.activa) actualizar_bala(&bala2, mapa, &jugador1, &jugador2);

        
        int ganador = evaluar_victoria(&jugador1, &jugador2,turno);
        if(ganador) {
            printf("¡Juego terminado! Ganador: Jugador %d\n", ganador);
            SDL_Delay(3000);
            running = 0;
        }

        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);

        // Escala automática perfecta 
        int win_w, win_h;
        SDL_GetWindowSize(win, &win_w, &win_h);

        float scale_x = (float)(win_w  - 40) / (COLUMNAS * 64.0f);
        float scale_y = (float)(win_h  - 80) / (FILAS    * 64.0f);
        float scale = fminf(scale_x, scale_y);

        int cell = (int)(64.0f * scale);
        if (cell < 16) cell = 16;

        int ox = (win_w - COLUMNAS * cell) / 2;
        int oy = (win_h - FILAS    * cell) / 2;

        // Dibujar mapa
        for(int i = 0; i < FILAS; i++) {
            for(int j = 0; j < COLUMNAS; j++) {
                int val = mapa[i][j];
                SDL_Rect dst = {ox + j*cell, oy + i*cell, cell, cell};
                if(val >= 0 && val < 10 && tiles[val]) {
                    SDL_RenderCopy(ren, tiles[val], NULL, &dst);
                }
            }
        }


        // ---- DIBUJAR TANQUES CON ROTACIÓN SEGÚN 'N','S','E','O' ----


        double angulo_T(char d) {
            switch(d) {
                case 'N': return 0;       // Norte = 0°
                case 'S': return 180; // Sur 
                case 'E': return 90;     // Derecha 
                case 'O': return 270;      // Izquierda 
            }
            return 0;
        }

        if (jugador1.vidas > 0) {
            SDL_Rect dst = {ox + jugador1.x * cell, oy + jugador1.y * cell, cell, cell};
            SDL_RenderCopyEx(ren, tiles[6], NULL, &dst, angulo_T(jugador1.direccion), NULL, SDL_FLIP_NONE);
        }

        if (jugador2.vidas > 0) {
            SDL_Rect dst = {ox + jugador2.x * cell, oy + jugador2.y * cell, cell, cell};
            SDL_RenderCopyEx(ren, tiles[7], NULL, &dst, angulo_T(jugador2.direccion), NULL, SDL_FLIP_NONE);
        }

        // === BALAS PEQUEÑAS  ===
        if(bala1.activa) {
            SDL_SetRenderDrawColor(ren, 255, 255, 0, 255);
            SDL_Rect br = {ox + bala1.x*cell + cell/4, oy + bala1.y*cell + cell/4, cell/2, cell/2};
            SDL_RenderFillRect(ren, &br);
        }
        if(bala2.activa) {
            SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
            SDL_Rect br = {ox + bala2.x*cell + cell/4, oy + bala2.y*cell + cell/4, cell/2, cell/2};
            SDL_RenderFillRect(ren, &br);
        }

        SDL_RenderPresent(ren);
    }

    // Limpieza
    for(int i = 0; i < FILAS; i++) free(mapa[i]);
    free(mapa);
    for(int i = 0; i < 10; i++) if(tiles[i]) SDL_DestroyTexture(tiles[i]);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    IMG_Quit();
    SDL_Quit();
    return 0;
}