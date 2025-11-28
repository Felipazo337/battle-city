#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "headers/def.h"
#include "headers/funcmapa.h"
int main(){
	srand(time(NULL));

	//se crea el mapa vacío en heap
	int **mapa=malloc(FILAS*sizeof(*mapa)); 
	if(mapa==NULL){
		printf("Error de asignación de memoria");
		return 1;
	}
	for(int i= 0; i<FILAS; i++){
    	mapa[i]=calloc(COLUMNAS,sizeof(int));
		if(mapa[i]==NULL){
			printf("Error al asignar memoria a la fila %d",i);
        	for(int j= 0; j<i; j++) free(mapa[j]);
        	free(mapa);
        	return 1;
    	}
    }

	//los bordes del mapa se rellenan
	bordearmapa(COLUMNAS,FILAS,mapa,IRROMPIBLE);

	//muros:cantidad de muros que se han puesto, mr: porcentaje de muros en el mapa
	//rx y ry son las coordenadas aleatorias donde se pondrán los muros en x e y respectivamente
	//aux servirá para comparar que las coordenadas aleatorias no se repitan
	int muros= 0, rx= 0, ry= 0, aux=0, intentos=0,mr;
	random(&mr,35,23);
	//se rellena el mapa con muros hasta una cantidad mínima
	while(muros <= FILAS*COLUMNAS*(mr/100.0) && intentos<1000){

		//generacion aleatoria de coordenadas dando margen de dos bloques
		random(&rx, COLUMNAS-4, 3);
		random(&ry, FILAS-4, 3);

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
		random(&r,3,0); 
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
	random(&r,24,12);
	while(arbustos<r && intentos<300){
		random(&rx, COLUMNAS-2, 1);
		random(&ry, FILAS-2, 1);
		if(mapa[ry][rx]==VACIO) rellenar(rx,ry,&arbustos,mapa,BUSH);
		intentos++;
	}
	
	//se coloca una cantidad aleatoria de bloques de agua
	//aguas es contador verificador y r es la cantidad de bloques
	int aguas=0;
	intentos=0;
	random(&r,15,9);
	while(aguas<r && intentos<300){
		random(&rx, COLUMNAS-3, 3);
		random(&ry, FILAS-3, 3);
		if(mapa[ry][rx]==VACIO) rellenar(rx,ry,&aguas,mapa,AGUA);
		intentos++;
	}

	//se asignan las posiciones de los jugadores y tps;
	mapa[2][2]=TP1;
	mapa[FILAS-3][COLUMNAS-3]=TP2;
	mapa[2][COLUMNAS-3]=JUGADOR1;
	mapa[FILAS-3][2]=JUGADOR2;

	FILE *fp;
	fp = fopen("mapa.txt", "w");
	if (fp == NULL) {
	    printf("No se pudo guardar el mapa :(");
	    return 1;
	}
	//Se guarda el mapa en un txt ingresado
	for(int i=0; i<FILAS; i++){
		for(int j=0;j<COLUMNAS;j++) fprintf(fp,"%d ",mapa[i][j]);
		fprintf(fp,"\n");
	}
	
	for(int i=0; i<FILAS; i++) free(mapa[i]);
	free(mapa);
	printf("%d",muros);
	return 0;
}
