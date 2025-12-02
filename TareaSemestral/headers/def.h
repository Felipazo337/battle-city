#ifndef DEF_H
#define DEF_H


#define VACIO 0
#define MURO 1
#define IRROMPIBLE 3
#define BUSH 4
#define AGUA 5
#define JUGADOR1 6
#define JUGADOR2 7
#define TP1 8
#define TP2 9
#define TBLOQUE 1
#define FILAS 14
#define COLUMNAS 15

void bordearmapa(int x,int y,int **m,int material);

void rndm(int *x, int max, int min);

void rellenar(int nx,int ny,int *c,int **m, int material);




#endif
