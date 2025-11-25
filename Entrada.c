#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
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
	random(&mr,37,28);
	//se rellena el mapa con muros hasta una cantidad mínima
	while(muros <= (TBLOQUE*13)*(TBLOQUE*14)*(mr/100.0) && intentos<1000){

		//generacion aleatoria de coordenadas dando margen de dos bloques
		random(&rx, COLUMNAS-TBLOQUE*2-1, TBLOQUE*2+1);
		random(&ry, FILAS-TBLOQUE*2-1, TBLOQUE*2+1);

		//para asegurarse que da al centro del bloque
		rx= ceil(rx/3.0)*TBLOQUE-1;
		ry= ceil(ry/3.0)*TBLOQUE-1;

		//las coordenadas (en bloques) deben ser pares
		if( (((rx+1)/TBLOQUE)&1) || (((ry+1)/TBLOQUE) & 1) ){
			intentos++;
			continue;}

		//validacion coordenadas dentro del margen
		if ((rx>=COLUMNAS-TBLOQUE*2) || (ry>=FILAS-TBLOQUE*2) || (rx-TBLOQUE*2-1<=0) || (ry-TBLOQUE*2-1<=0)){
			intentos++;
			continue;}

		//Crea el bloque de muro 3x3 en la coordenada
		if (mapa[ry][rx]==VACIO) rellenar(rx,ry,&muros,mapa,MURO);

		//Eleccion aleatoria del primer par de coordenadas en las 4 direcciones cardinales +2 por medio de alx[r] y aly[r]
		int alx[4]= {rx, rx, rx+2*TBLOQUE, rx-2*TBLOQUE};
		int aly[4]= {ry+2*TBLOQUE, ry-2*TBLOQUE, ry, ry};
		int r= rand()%4; aux=r;

		//Se guarda la coordenada entre alx/aly y rx/ry
		int cordx= alx[r]+(rx-alx[r])/2;
		int cordy= aly[r]+(ry-aly[r])/2;

		//Se rellena en caso de ser necesario(junto con cordx y cordy)
		if(mapa[aly[r]][alx[r]]==VACIO && mapa[cordy][cordx]==VACIO){
			rellenar(alx[r], aly[r], &muros, mapa, MURO);
			rellenar(cordx, cordy, &muros, mapa, MURO);
		}

		//Elección aleatoria del segundo par de coordenadas
		while(r==aux) r=rand()%4;

		cordx= alx[r]+(rx-alx[r])/2;
		cordy= aly[r]+(ry-aly[r])/2;

		//se rellena la segunda coordenada
		if(mapa[aly[r]][alx[r]]==VACIO){
			rellenar(alx[r], aly[r], &muros, mapa, MURO);
			if(mapa[cordy][cordx]==VACIO) rellenar(cordx, cordy, &muros, mapa, MURO);
		intentos++;
		}
	}
	//Se colocan algunos muros irrompibles para evitar pasillos vacíos
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

	//se coloca una cantidad aleatoria de arbustos
	//arbustos es contador verificador y r es la cantidad de bloques	
	int arbustos=0,r;
	intentos=0;
	random(&r,6,2);
	while(arbustos<9*r*4 && intentos<300){
		random(&rx, COLUMNAS-TBLOQUE, TBLOQUE);
		random(&ry, FILAS-TBLOQUE, TBLOQUE);
		rx= ceil(rx/3.0)*TBLOQUE-1;
		ry= ceil(ry/3.0)*TBLOQUE-1;
		if(mapa[ry][rx]==VACIO) rellenar(rx,ry,&arbustos,mapa,BUSH);
		intentos++;
	}
	
	//se coloca una cantidad aleatoria de bloques de agua
	//aguas es contador verificador y r es la cantidad de bloques
	int aguas=0;
	intentos=0;
	random(&r,4,0);
	while(aguas<9*r*3 && intentos<300){
		random(&rx, COLUMNAS-TBLOQUE*3, TBLOQUE*2);
		random(&ry, FILAS-TBLOQUE*2, TBLOQUE*3);
		rx= ceil(rx/3.0)*TBLOQUE-1;
		ry= ceil(ry/3.0)*TBLOQUE-1;
		if(mapa[ry][rx]==VACIO) rellenar(rx,ry,&aguas,mapa,AGUA);
		intentos++;
	}


	rellenar(TBLOQUE*13-1,TBLOQUE-1,&arbustos,mapa,VACIO);
	rellenar(TBLOQUE-1,TBLOQUE*14-1,&arbustos,mapa,VACIO);
	mapa[1*TBLOQUE-1][COLUMNAS-TBLOQUE]=JUGADOR1;

	char txt[10];
	scanf("%s",txt);
	FILE *fp;
	fp = fopen(txt, "w");
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
	return 0;
}
