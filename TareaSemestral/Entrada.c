
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "headers/def.h"
#include "headers/def.h"

void bordearmapa(int x,int y,int **m,int material){
    for(int i= 0; i<y; i++){
        for(int j= 0; j<x; j++){
            if((j==0) || (i==0) || (i==y-1) || (j==x-1)) m[i][j]= material;
        }
    }
}
void rndm(int *x, int max, int min){
    (*x)= (rand()%(max-min+1))+min;
}
void rellenar(int nx,int ny,int *c,int **m, int material){
    if(m[ny][nx]==VACIO) (*c)++;
    m[ny][nx]= material;
}


void generar_mapa_completo(int **mapa_destino){
    srand(time(NULL));

    //se crea el mapa vacío en heap
    int **mapa=malloc(FILAS*sizeof(*mapa)); 
    if(mapa==NULL){
        printf("Error de asignación de memoria");
        return;
    }
    for(int i= 0; i<FILAS; i++){
        mapa[i]=calloc(COLUMNAS,sizeof(int));
        if(mapa[i]==NULL){
            printf("Error al asignar memoria a la fila %d",i);
            for(int j= 0; j<i; j++) free(mapa[j]);
            free(mapa);
            return;
        }
    }

    //los bordes del mapa se rellenan
    bordearmapa(COLUMNAS,FILAS,mapa,IRROMPIBLE);

    //muros:cantidad de muros que se han puesto, mr: porcentaje de muros en el mapa
    //rx y ry son las coordenadas aleatorias donde se pondrán los muros en x e y respectivamente
    //aux servirá para comparar que las coordenadas aleatorias no se repitan
    int muros= 0, rx= 0, ry= 0, aux=0, intentos=0,mr;
    rndm(&mr,35,23);
    //se rellena el mapa con muros hasta una cantidad mínima
    while(muros <= FILAS*COLUMNAS*(mr/100.0) && intentos<1000){

        //generacion aleatoria de coordenadas dando margen de dos bloques
        rndm(&rx, COLUMNAS-4, 3);
        rndm(&ry, FILAS-4, 3);

        //las coordenadas (en bloques) deben ser pares
        if( (rx & 1) || (ry & 1) ){
            intentos++;
            continue;
        }

        //validacion coordenadas dentro del margen
        if ((rx>COLUMNAS-4) || (ry>FILAS-4) || (rx-3<0) || (ry-3<0)){
            intentos++;
            continue;
        }

        //Crea el muro en la coordenada
        if (mapa[ry][rx]==VACIO){
            rellenar(rx,ry,&muros,mapa,MURO);
        }

        //Eleccion aleatoria del primer par de coordenadas en las 4 direcciones cardinales +2 por medio de alx[r] y aly[r]
        int alx[4]= {rx, rx, rx+2, rx-2};
        int aly[4]= {ry+2, ry-2, ry, ry};
        int r;
        rndm(&r,3,0); 
        aux=r;

        //Se guarda la coordenada entre alx/aly y rx/ry
        int cordx= alx[r]+(rx-alx[r])/2;
        int cordy= aly[r]+(ry-aly[r])/2;

        //Se rellena en caso de ser necesario(junto con cordx y cordy)
        if(mapa[aly[r]][alx[r]]==VACIO && mapa[cordy][cordx]==VACIO){
            rellenar(alx[r],aly[r],&muros,mapa,MURO);
            rellenar(cordx,cordy,&muros,mapa,MURO);
        }

        //Elección aleatoria del segundo par de coordenadas
        while(r==aux) r=rand()%4;
        cordx= alx[r]+(rx-alx[r])/2;
        cordy= aly[r]+(ry-aly[r])/2;

        //se rellena la segunda coordenada
        if(mapa[aly[r]][alx[r]]==VACIO && mapa[cordy][cordx]==VACIO){
            rellenar(alx[r],aly[r],&muros,mapa,MURO);
            rellenar(cordx,cordy,&muros,mapa,MURO);
        }
    }
    //Se colocan algunos muros irrompibles para evitar pasillos vacíos

    rellenar(1, (COLUMNAS-1)/2-3, &muros, mapa, IRROMPIBLE);
    rellenar(1, (COLUMNAS-1)/2+1, &muros, mapa, IRROMPIBLE);
    rellenar((COLUMNAS-1)/2-2, (COLUMNAS-1)/2-2, &muros, mapa, IRROMPIBLE);
    rellenar((COLUMNAS-1)/2-2, 1, &muros, mapa, IRROMPIBLE);
    rellenar((COLUMNAS-1)/2-2, (COLUMNAS-1)/2+2, &muros, mapa, IRROMPIBLE);
    rellenar((COLUMNAS-1)/2+2, (COLUMNAS-1)/2+3, &muros, mapa, IRROMPIBLE);
    rellenar((COLUMNAS-1)/2+2, 1, &muros, mapa, IRROMPIBLE);
    rellenar((COLUMNAS-1)/2+2, (COLUMNAS-1)/2-1, &muros, mapa, IRROMPIBLE);
    rellenar(COLUMNAS-2,(COLUMNAS-1)/2,&muros,mapa,IRROMPIBLE);
    rellenar(COLUMNAS-2,(COLUMNAS-1)/2+4,&muros,mapa,IRROMPIBLE);

    //se coloca una cantidad aleatoria de arbustos
    //arbustos es contador verificador y r es la cantidad de bloques    
    int arbustos=0,r;
    intentos=0;
    rndm(&r,24,12);
    while(arbustos<r && intentos<300){
        rndm(&rx, COLUMNAS-2, 1);
        rndm(&ry, FILAS-2, 1);
        if(mapa[ry][rx]==VACIO) rellenar(rx,ry,&arbustos,mapa,BUSH);
        intentos++;
    }
    
    //se coloca una cantidad aleatoria de bloques de agua
    //aguas es contador verificador y r es la cantidad de bloques
    int aguas=0;
    intentos=0;
    rndm(&r,15,9);
    while(aguas<r && intentos<300){
        rndm(&rx, COLUMNAS-3, 3);
        rndm(&ry, FILAS-3, 3);
        if(mapa[ry][rx]==VACIO) rellenar(rx,ry,&aguas,mapa,AGUA);
        intentos++;
    }

    //se asignan las posiciones de los jugadores y tps;
    mapa[2][2]=TP1;
    mapa[FILAS-3][COLUMNAS-3]=TP2;

    for(int i = 0; i < FILAS; i++) {
        for(int j = 0; j < COLUMNAS; j++) {
            mapa_destino[i][j] = mapa[i][j];
        }
        free(mapa[i]);
    }
    free(mapa);
}
