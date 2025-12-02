#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "headers/def.h"
#include "headers/logica.h"
#include <math.h>


double angulo_T(char d) {
    switch(d) {
        case 'N': return 0;          // Norte 
        case 'S': return 180;        // Sur 
        case 'E': return 90;         // Derecha 
        case 'O': return 270;        // Izquierda 
    }
    return 0;   
}


void generar_mapa_completo(int **mapa_destino);


int **mapa = NULL;
Tanque jugador1, jugador2;
Bala bala1, bala2;

// Texturas 
SDL_Texture* tiles[10] = {NULL};
const char* nombres_texturas[] = {
    "assets/empty.png",     // 0 Vacio
    "assets/brick.png",     // 1 Muro
    NULL,                   // 2 (no usado)
    "assets/steel.png",     // 3 Irrompible
    "assets/bush.png",      // 4 Bush
    "assets/water.png",     // 5 Agua
    "assets/player.png",    // 6 Jugador 1
    "assets/player2.png",   // 7 Jugador 2
    "assets/tp1.png",       // 8 TP1
    "assets/tp2.png"        // 9 TP2
};

SDL_Texture *btn_guardar   = NULL;
SDL_Texture *btn_continuar = NULL;
SDL_Texture *btn_salir     = NULL;

int menu_pausa  = 0;
int opcion_menu = 0;


void generar_mapa() {
    mapa = malloc(FILAS * sizeof(int*));
    for(int i = 0; i < FILAS; i++) 
        mapa[i] = calloc(COLUMNAS, sizeof(int));
    generar_mapa_completo(mapa);
}

int main(int argc, char* argv[]) {
    int opcion;
    int turnos = 0; //Inicializacion de los turnos

    printf("\n========== BATTLE CITY ==========\n");
    printf("1) Nueva Partida\n2) Cargar Partida\n> ");
    scanf("%d",&opcion);

    generar_mapa();
// ======= CARGAR PARTIDA =======
    if(opcion == 2){
        printf("\nArchivo(con extension .txt por favor): "); 
        char archivo_de_carga[100];
        scanf("%99s", archivo_de_carga);

        if(cargar_partida(archivo_de_carga,mapa,&jugador1,&jugador2,&bala1,&bala2, &turnos)){
            printf("Partida Cargada exitosamente.\n");
            printf(" Turnos : %d\n", turnos);
            printf(" J1: %d vidas, %d kills\n", jugador1.vidas, jugador1.kills);
            printf(" J2: %d vidas, %d kills\n", jugador2.vidas, jugador2.kills);
        } else {
            printf("Error al cargar partida. Iniciando partida nueva... \n");
            opcion = 1;
        }
    }
   // ======= NUEVA PARTIDA =======
    if(opcion == 1){       
        escanear_tp(mapa);                 
        inicializar_tanque(&jugador1, JUGADOR1, 1, 1);
        inicializar_tanque(&jugador2, JUGADOR2, COLUMNAS-2, FILAS-2);
        //Nuevo
        dibujar_tanque(mapa, &jugador1);
        dibujar_tanque(mapa, &jugador2);
        bala1.activa = bala2.activa = 0;
        turnos = 0;
    }
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    // Ventana de la interfaz 
    SDL_Window* win = SDL_CreateWindow("Battle City",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1200, 900, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);  

    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

// Se cargan las texturas.
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

//  Botones de la interfaz para el menú que pausa la partida.
    btn_guardar   = IMG_LoadTexture(ren,"assets/guardar.png");
    btn_continuar = IMG_LoadTexture(ren,"assets/continuar.png");
    btn_salir     = IMG_LoadTexture(ren,"assets/salir.png");

    int running = 1;
    SDL_Event e;
    Uint64 last_time = SDL_GetTicks64();

    int contador_bala = 0;
    const int BALA_UPDATE_RATE = 5;//Se actualizan las balas cada 5 frames


    while(running) {
        Uint64 now = SDL_GetTicks64();
        if(now - last_time < 16) { SDL_Delay(1); continue; } // 60 FPS
        last_time = now;

        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) running = 0;

            if(e.type == SDL_KEYDOWN) {
                SDL_Scancode sc = e.key.keysym.scancode;

                if(sc == SDL_SCANCODE_ESCAPE) { 
                    menu_pausa = !menu_pausa; 
                    if(menu_pausa) opcion_menu = 0; 
                    SDL_Delay(120); 
                }

                // Configuración del menú pausa
                if(menu_pausa) {
                    if(sc == SDL_SCANCODE_UP)    opcion_menu = (opcion_menu + 2) % 3;
                    if(sc == SDL_SCANCODE_DOWN)  opcion_menu = (opcion_menu + 1) % 3;

                    if(sc == SDL_SCANCODE_RETURN || sc == SDL_SCANCODE_KP_ENTER) {
                        if(opcion_menu == 0) { // GUARDAR
                            printf("Guardar como(sin extension .txt por favor): ");
                            char archivo[100];
                            fflush(stdout);
                            scanf("%99s", archivo);

                            char ruta[110];
                            snprintf(ruta, sizeof(ruta), "%s.txt", archivo);
                            if(!guardar_partida(ruta, mapa, &jugador1, &jugador2, &bala1, &bala2, turnos)){
                                printf("Error al guardar partida.\n");
                            } else {
                                printf("Partida guardada!\n");
                            }

                        }
                        if(opcion_menu == 1) menu_pausa = 0;  // CONTINUAR
                        if(opcion_menu == 2) running = 0;     // SALIR
                    }
                    continue; 
                }

                // Se configuran los controles de cada jugador: 
                // Jugador 1: para moverse usa W,S,A,D y para disparar el ESPACIO. 
                if(sc == SDL_SCANCODE_W)     { mover_tanque(&jugador1, 'w', mapa); turnos++; }
                if(sc == SDL_SCANCODE_S)     { mover_tanque(&jugador1, 's', mapa); turnos++; }
                if(sc == SDL_SCANCODE_A)     { mover_tanque(&jugador1, 'a', mapa); turnos++; }
                if(sc == SDL_SCANCODE_D)     { mover_tanque(&jugador1, 'd', mapa); turnos++; }
                if(sc == SDL_SCANCODE_SPACE) {
                    if(!menu_pausa && !bala1.activa){
                        disparar(&bala1, &jugador1); 
                        turnos++;
                    }  
                } 

                // Jugador 2: Para moverse usa las flechitas del teclado y para disparar el ENTER.
                if(sc == SDL_SCANCODE_UP)    { mover_tanque(&jugador2, 'w', mapa); turnos++; }
                if(sc == SDL_SCANCODE_DOWN)  { mover_tanque(&jugador2, 's', mapa); turnos++; }
                if(sc == SDL_SCANCODE_LEFT)  { mover_tanque(&jugador2, 'a', mapa); turnos++; }
                if(sc == SDL_SCANCODE_RIGHT) { mover_tanque(&jugador2, 'd', mapa); turnos++; }
                if(sc == SDL_SCANCODE_RETURN){ 
                    if(!menu_pausa && !bala2.activa){
                        disparar(&bala2, &jugador2); 
                        turnos++; 
                    }
                }
                    
            }
        }

        


        // Actualizar Conteo de balas
        contador_bala++;
        if(contador_bala >= BALA_UPDATE_RATE){
            if(!menu_pausa && bala1.activa) actualizar_bala(&bala1, mapa, &jugador2, &jugador1);
            if(!menu_pausa && bala2.activa) actualizar_bala(&bala2, mapa, &jugador1, &jugador2);
            contador_bala = 0;

        }
        
        
        int ganador = evaluar_victoria(&jugador1, &jugador2,turnos);
        if(ganador) {
        printf("\n\nJuego terminado! Ganador: JUGADOR %d\n", ganador);
            printf("Estadisticas Finales:\n");
            printf("\n Jugador 1: %d kills, %d vidas\n", jugador1.kills, jugador1.vidas);
            printf(" Jugador 2: %d kills, %d vidas\n", jugador2.kills, jugador2.vidas);
            printf("\n Turnos totales: %d\n", turnos);
            printf("\n");
            SDL_Delay(3000);
            running = 0;

        }

        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);

        // Configuración escalado de las dimensiones de la interfaz 
        int win_w, win_h;
        SDL_GetWindowSize(win, &win_w, &win_h);

        float scale_x = (float)(win_w  - 40) / (COLUMNAS * 64.0f);
        float scale_y = (float)(win_h  - 80) / (FILAS    * 64.0f);
        float scale = fminf(scale_x, scale_y);

        int cell = (int)(64.0f * scale);
        if (cell < 16) cell = 16;

        int ox = (win_w - COLUMNAS * cell) / 2;
        int oy = (win_h - FILAS    * cell) / 2;

        // Dibujar mapa en la interfaz
        for(int i = 0; i < FILAS; i++) {
            for(int j = 0; j < COLUMNAS; j++) {
                int val = mapa[i][j];
                SDL_Rect dst = {ox + j*cell, oy + i*cell, cell, cell};
                if(val >= 0 && val < 10 && tiles[val]) {
                    SDL_RenderCopy(ren, tiles[val], NULL, &dst);
                }
            }
        }


        // Configuración de la rotación de tanques según: 'N','S','E','O' 

        if (jugador1.vidas > 0) {
            SDL_Rect dst = {ox + jugador1.x * cell, oy + jugador1.y * cell, cell, cell};
            SDL_RenderCopyEx(ren, tiles[6], NULL, &dst, angulo_T(jugador1.direccion), NULL, SDL_FLIP_NONE);
        }

        if (jugador2.vidas > 0) {
            SDL_Rect dst = {ox + jugador2.x * cell, oy + jugador2.y * cell, cell, cell};
            SDL_RenderCopyEx(ren, tiles[7], NULL, &dst, angulo_T(jugador2.direccion), NULL, SDL_FLIP_NONE);
        }

        // Renderizado balas
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

        printf("\rJ1: %dV %dK | J2: %dV %dK | Turnos: %d/%d  ",
            jugador1.vidas, jugador1.kills,
            jugador2.vidas, jugador2.kills,
            turnos, MAX_TURNOS);
        fflush(stdout);


        // Renderizado de la interfaz del menú de pausa.
        if(menu_pausa){

            SDL_SetRenderDrawBlendMode(ren,SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(ren,0,0,0,180);
            SDL_Rect dark={0,0,win_w,win_h};
            SDL_RenderFillRect(ren,&dark);

           
            int bw = 300, bh = 50, sep = 60;

            SDL_Rect r1={win_w/2-bw/2, win_h/2-sep,     bw,bh};
            SDL_Rect r2={win_w/2-bw/2, win_h/2,         bw,bh};
            SDL_Rect r3={win_w/2-bw/2, win_h/2+sep,     bw,bh};

            if(btn_guardar)   SDL_RenderCopy(ren,btn_guardar  ,NULL,&r1);
            if(btn_continuar) SDL_RenderCopy(ren,btn_continuar,NULL,&r2);
            if(btn_salir)     SDL_RenderCopy(ren,btn_salir    ,NULL,&r3);
            
            // Configuración efecto botones
            static float t = 0; 
            t += 0.2f;  

            
            Uint8 glow = 100 + (Uint8)(155 * (sin(t) * 0.5f + 0.5f));  

           
            int grosor = 8;  

            
            SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
            for(int i = grosor; i >= 1; i--) {
                Uint8 alpha = 255 / (i + 2);  
                SDL_SetRenderDrawColor(ren, glow, glow, 30, alpha);

                SDL_Rect glow_rect;
                if(opcion_menu == 0) glow_rect = (SDL_Rect){r1.x - i, r1.y - i, r1.w + i*2, r1.h + i*2};
                if(opcion_menu == 1) glow_rect = (SDL_Rect){r2.x - i, r2.y - i, r2.w + i*2, r2.h + i*2};
                if(opcion_menu == 2) glow_rect = (SDL_Rect){r3.x - i, r3.y - i, r3.w + i*2, r3.h + i*2};

                SDL_RenderFillRect(ren, &glow_rect);
            }

            
            SDL_SetRenderDrawColor(ren, 255, 255, 0, 255);
            SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_ADD);  

            if(opcion_menu == 0) SDL_RenderDrawRect(ren, &(SDL_Rect){r1.x-2, r1.y-2, r1.w+4, r1.h+4});
            if(opcion_menu == 1) SDL_RenderDrawRect(ren, &(SDL_Rect){r2.x-2, r2.y-2, r2.w+4, r2.h+4});
            if(opcion_menu == 2) SDL_RenderDrawRect(ren, &(SDL_Rect){r3.x-2, r3.y-2, r3.w+4, r3.h+4});
        }

        SDL_RenderPresent(ren);
    }


    // Limpieza
    for(int i = 0; i < FILAS; i++) free(mapa[i]);
    free(mapa);
    for(int i = 0; i < 10; i++) if(tiles[i]) SDL_DestroyTexture(tiles[i]);
    if(btn_guardar) SDL_DestroyTexture(btn_guardar);
    if(btn_continuar) SDL_DestroyTexture(btn_continuar);
    if(btn_salir) SDL_DestroyTexture(btn_salir);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    IMG_Quit();
    SDL_Quit();
    return 0;

}






