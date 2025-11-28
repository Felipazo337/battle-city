
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "headers/def.h"
#include "headers/funcmapa.h"

void generar_mapa_completo(int **mapa_destino) {
    srand(time(NULL));

    int **mapa = malloc(FILAS * sizeof(*mapa));
    if(mapa == NULL) {
        printf("Error de asignación de memoria\n");
        for(int k=0; k<FILAS; k++) free(mapa_destino[k]);
        free(mapa_destino);
        return;
    }
    for(int i = 0; i < FILAS; i++) {
        mapa[i] = calloc(COLUMNAS, sizeof(int));
        if(mapa[i] == NULL) {
            printf("Error al asignar memoria a la fila %d\n", i);
            for(int j = 0; j < i; j++) free(mapa[j]);
            free(mapa);
            for(int k=0; k<FILAS; k++) free(mapa_destino[k]);
            free(mapa_destino);
            return;
        }
    }

    bordearmapa(COLUMNAS, FILAS, mapa, IRROMPIBLE);

    int muros = 0, rx = 0, ry = 0, aux = 0, intentos = 0, mr;
    random(&mr, 37, 28);

    while(muros <= (TBLOQUE*13)*(TBLOQUE*14)*(mr/100.0) && intentos < 1000) {
        random(&rx, COLUMNAS-TBLOQUE*2-1, TBLOQUE*2+1);
        random(&ry, FILAS-TBLOQUE*2-1, TBLOQUE*2+1);

        rx = ceil(rx/3.0)*TBLOQUE - 1;
        ry = ceil(ry/3.0)*TBLOQUE - 1;

        if( (((rx+1)/TBLOQUE)&1) || (((ry+1)/TBLOQUE) & 1) ) {
            intentos++;
            continue;
        }

        if ((rx >= COLUMNAS-TBLOQUE*2) || (ry >= FILAS-TBLOQUE*2) || (rx-TBLOQUE*2-1 <= 0) || (ry-TBLOQUE*2-1 <= 0)) {
            intentos++;
            continue;
        }

        if (mapa[ry][rx] == VACIO) rellenar(rx, ry, &muros, mapa, MURO);

        int alx[4] = {rx, rx, rx+2*TBLOQUE, rx-2*TBLOQUE};
        int aly[4] = {ry+2*TBLOQUE, ry-2*TBLOQUE, ry, ry};
        int r = rand()%4; aux = r;

        int cordx = alx[r] + (rx - alx[r])/2;
        int cordy = aly[r] + (ry - aly[r])/2;

        if(mapa[aly[r]][alx[r]] == VACIO && mapa[cordy][cordx] == VACIO) {
            rellenar(alx[r], aly[r], &muros, mapa, MURO);
            rellenar(cordx, cordy, &muros, mapa, MURO);
        }

        while(r == aux) r = rand()%4;
        cordx = alx[r] + (rx - alx[r])/2;
        cordy = aly[r] + (ry - aly[r])/2;

        if(mapa[aly[r]][alx[r]] == VACIO) {
            rellenar(alx[r], aly[r], &muros, mapa, MURO);
            if(mapa[cordy][cordx] == VACIO) rellenar(cordx, cordy, &muros, mapa, MURO);
        }
        intentos++;
    }

    // Muros irrompibles fijos
    rellenar(TBLOQUE-1, 5*TBLOQUE-1, &muros, mapa, IRROMPIBLE);
    rellenar(TBLOQUE-1, 10*TBLOQUE-1, &muros, mapa, IRROMPIBLE);
    rellenar(6*TBLOQUE-1, 6*TBLOQUE-1, &muros, mapa, IRROMPIBLE);
    rellenar(6*TBLOQUE-1,TBLOQUE-1, &muros, mapa, IRROMPIBLE);
    rellenar(6*TBLOQUE-1,8*TBLOQUE-1, &muros, mapa, IRROMPIBLE);
    rellenar(9*TBLOQUE-1,8*TBLOQUE-1, &muros, mapa, IRROMPIBLE);
    rellenar(9*TBLOQUE-1,TBLOQUE-1, &muros, mapa, IRROMPIBLE);
    rellenar(9*TBLOQUE-1,6*TBLOQUE-1, &muros, mapa, IRROMPIBLE);
    rellenar(COLUMNAS-TBLOQUE,5*TBLOQUE-1,&muros,mapa,IRROMPIBLE);
    rellenar(COLUMNAS-TBLOQUE,10*TBLOQUE-1,&muros,mapa,IRROMPIBLE);

    // Arbustos
    int arbustos = 0, r;
    intentos = 0;
    random(&r,6,2);
    while(arbustos < 9*r*4 && intentos < 300) {
        random(&rx, COLUMNAS-TBLOQUE, TBLOQUE);
        random(&ry, FILAS-TBLOQUE, TBLOQUE);
        rx = ceil(rx/3.0)*TBLOQUE - 1;
        ry = ceil(ry/3.0)*TBLOQUE - 1;
        if(mapa[ry][rx] == VACIO) rellenar(rx,ry,&arbustos,mapa,BUSH);
        intentos++;
    }

    // Agua
    int aguas = 0;
    intentos = 0;
    random(&r,4,0);
    while(aguas < 9*r*3 && intentos < 300) {
        random(&rx, COLUMNAS-TBLOQUE*3, TBLOQUE*2);
        random(&ry, FILAS-TBLOQUE*2, TBLOQUE*3);
        rx = ceil(rx/3.0)*TBLOQUE - 1;
        ry = ceil(ry/3.0)*TBLOQUE - 1;
        if(mapa[ry][rx] == VACIO) rellenar(rx,ry,&aguas,mapa,AGUA);
        intentos++;
    }

    rellenar(TBLOQUE*13-1,TBLOQUE-1,&arbustos,mapa,VACIO);
    rellenar(TBLOQUE-1,TBLOQUE*14-1,&arbustos,mapa,VACIO);

    // ← Aquí copiamos el mapa al destino
    for(int i = 0; i < FILAS; i++) {
        for(int j = 0; j < COLUMNAS; j++) {
            mapa_destino[i][j] = mapa[i][j];
        }
        free(mapa[i]);
    }
    free(mapa);
}