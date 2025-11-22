#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#define MURO 2
//#define	agua
#define VACIO 0
#define IRROMPIBLE 1
//#define arbustos
//#define jugador
#define TBLOQUE 3
#define FILAS 44
#define COLUMNAS 41
#define JUGADOR1 6
#define JUGADOR2 7
//#define NARCH 
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
	for(int i= -1; i<2; i++){
		for(int j= -1; j<2; j++){
			if(m[ny+i][nx+j]!=material) (*c)++;
			m[ny+i][nx+j]= material;	
		}
	}
}
void espacios(int n){
	for(int k= 0;k<n;k++) printf("\n");
}
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

	//muros:cantidad de muros que se han puesto, 
	//rx y ry son las coordenadas aleatorias donde se pondrán los muros en x e y respectivamente
	//aux servirá para comparar que las coordenadas aleatorias no se repitan
	int muros= 0, rx= 0, ry= 0, aux=0, intentos=0;

	//se rellena el mapa con muros hasta una cantidad mínima
	while(muros <= (FILAS-2)*(COLUMNAS-2)*(3.6/10.0) && intentos<35){

		//generacion aleatoria de coordenadas dando margen de dos bloques
		random(&rx, COLUMNAS-TBLOQUE*2-1, TBLOQUE*2+1);
		random(&ry, FILAS-TBLOQUE*2-1, TBLOQUE*2+1);

		//para asegurarse que da al centro del bloque
		rx= ceil(rx/3.0)*TBLOQUE-1;
		ry= ceil(ry/3.0)*TBLOQUE-1;

		//las coordenadas (en bloques) deben ser pares
		if((rx+1)/TBLOQUE&1 || (ry+1)/TBLOQUE&1){intentos++; continue;}

		//validacion coordenadas dentro del margen
		if ((rx>=COLUMNAS-TBLOQUE*2) || (ry>=FILAS-TBLOQUE*2) || (rx-TBLOQUE*2-1<=0) || (ry-TBLOQUE*2-1<=0)){intentos++; continue;}

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
		if(mapa[aly[r]][alx[r]]==VACIO && mapa[cordx][cordy]==VACIO){
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
	rellenar(TBLOQUE-1, 9*TBLOQUE-1, &muros, mapa, IRROMPIBLE);
	rellenar(2+5*TBLOQUE, 6*TBLOQUE-1, &muros, mapa, IRROMPIBLE);
	rellenar(2+5*TBLOQUE,TBLOQUE-1, &muros, mapa, IRROMPIBLE);
	rellenar(2+8*TBLOQUE,TBLOQUE-1, &muros, mapa, IRROMPIBLE);
	rellenar(COLUMNAS-3,5*TBLOQUE-1,&muros,mapa,IRROMPIBLE);
	rellenar(COLUMNAS-3,9*TBLOQUE-1,&muros,mapa,IRROMPIBLE);

	//Imprimir el mapa solo para comprobar
	for(int i=0; i<FILAS; i++){
		for(int j=0;j<COLUMNAS;j++) printf("%d",mapa[i][j]);
		printf("\n");
	}
	espacios(3);
	/*FILE *fp;
	fp=fopen(narch,"w");*/
	printf("Muros: %d-%d, Compara:%f",(muros/9),muros,(FILAS-2)*(COLUMNAS-2)*(3.6/10.0));
	for(int i=0; i<FILAS; i++){
        free(mapa[i]);
    }
    free(mapa);
	return 0;
}
