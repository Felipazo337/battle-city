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
			if(m[ny+i][nx+j]==VACIO) (*c)++;
			m[ny+i][nx+j]= material;	
		}
	}
}
void espacios(int n){
	for(int k= 0;k<n;k++) printf("\n");
}
