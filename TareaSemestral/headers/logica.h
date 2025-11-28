#ifndef LOGICA_H
#define LOGICA_H 

#include <time.h>
#include "def.h"

#define MAX_VIDAS 3
#define KILLS_PARA_GANAR 3
#define MAX_TURNOS 5000
#define COOLDOWN_TP 15

//Agrupo las variables del tanque en un estructura
typedef struct {
 	int x;
 	int y;
 	char direccion; //N, S, E, O
 	int id; //Jugador 1 (6) o Jugador 2 (7)	
 	int kills;
 	int vidas;
 	int celda_debajo;
 } Tanque;

typedef struct{
 	int x;
 	int y;
 	char direccion; //N, S, E, O
 	int activa;
 	int quien;//id del jugador que dispara
} Bala;

//Variables globales del tp
extern int tp1_x, tp1_y;
extern int tp2_x, tp2_y;

//Cooldown del tp
extern time_t teleport_cooldown_hasta;

//Funciones de validacion
int dentro(int x, int y);
int bloque_vacio(int **mapa, int x, int y);

//Funciones de tanques
void inicializar_tanque(Tanque *t, int id, int x_inicial, int y_inicial);
void dibujar_tanque(int **mapa, Tanque *t);
void borrar_tanque(int **mapa, Tanque *t);
void mover_tanque(Tanque *t, char direccion, int **mapa);

//TP
void aplicar_tp(Tanque *t);
void escanear_tp(int **mapa);


int colisiona_tanque(Tanque *t, int bx, int by);

void disparar(Bala *b, Tanque *t);
void actualizar_bala(Bala *b, int **mapa, Tanque *enemigo, Tanque *tirador);

int evaluar_victoria(Tanque *t1, Tanque *t2, int turnos);

//Guardado de partida
int guardar_partida(const char *archivo, int **mapa, Tanque *t1, Tanque *t2, Bala *b1, Bala *b2, int turnos);
int cargar_partida(const char *archivo, int **mapa, Tanque *t1, Tanque *t2, Bala *b1, Bala *b2, int *turnos);


#endif
