#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "headers/logica.h" 


//Marco el centro de los tp
int tp1_x = -1, tp1_y = -1;
int tp2_x = -1, tp2_y = -1;

//Cooldown global
time_t teleport_cooldown_hasta = 0;

int dentro(int x, int y){
	if(x < 0) return 0;
	if(x >= COLUMNAS) return 0;

	if(y < 0) return 0;
	if(y >= FILAS) return 0;

	return 1;
}

//Verifico que el bloque es vacio
int bloque_vacio(int **mapa, int x, int y){
    if(!dentro(x, y)) return 0;

    int v = mapa[y][x];

    //El tanque puede atravesar vacio y tps
    if (v == VACIO || v == TP1 || v == TP2 || v == BUSH) return 1;

    //Si no, no puede atravesarlo, 
    return 0;
}

//Dibujo el tanque en el mapa
void dibujar_tanque(int **mapa, Tanque *t){
    if (!dentro(t->x, t->y)) return;

    t->celda_debajo = mapa[t->y][t->x];
    mapa[t->y][t->x] = t->id;
}


//Borro el tanque en caso de TP o de ser necesario
void borrar_tanque(int **mapa, Tanque *t){
    if (!dentro(t->x, t->y)) return;

    if(tp1_x >= 0 && tp1_y >= 0 && t->x == tp1_x && t->y == tp1_y){ //Si el TP1 está dentro del mapa
        mapa[t->y][t->x] = TP1;
    } else if(tp2_x >= 0 && tp2_y >= 0 && t->x == tp2_x && t->y == tp2_y){ //Si el TP2 esta dentro del mapa
        mapa[t->y][t->x] = TP2;
    } else {
        mapa[t->y][t->x] = t->celda_debajo;
    }

}


//Aplicar la teletransportacion 
void aplicar_tp(Tanque *t){
    time_t now = time(NULL); //Inicio el cooldown en 0
    if (now < teleport_cooldown_hasta) return; //Si el tp está en cooldown no se puede usar

    //Reviso el TP1 -> TP2
    if(tp1_x >= 0 && tp1_y >= 0 && t->x == tp1_x && t->y == tp1_y){
        if(tp2_x >= 0 && tp2_y >= 0){
            //Teletransporto el tanque de TP1 -> TP2
            t->x = tp2_x; 
            t->y = tp2_y;
            teleport_cooldown_hasta = now + COOLDOWN_TP; //Aplico cooldown
        }
        return;
    }

    //Reviso el TP2 -> TP1
    if(tp2_x >= 0 && tp2_y >= 0 && t->x == tp2_x && t->y == tp2_y){
        if(tp1_x >= 0 && tp1_y >= 0){
            //Teletransporto el tanque de TP2 -> TP1
            t->x = tp1_x;
            t->y = tp1_y;
            teleport_cooldown_hasta = now + COOLDOWN_TP; //Aplico cooldown
        }
    }
}

void mover_tanque(Tanque *t, char dir, int **mapa){
    //Guardo la posicion del tanque
    int nx = t->x;
    int ny = t->y;

//Veo la direccion de los movimientos (norte, sur, etc) y aplico movimiento y direccion
	if(dir == 'w') ny = t->y - 1, t->direccion = 'N';
	else if (dir == 's') ny = t->y + 1, t->direccion = 'S';
	else if (dir == 'a') nx = t->x - 1, t->direccion = 'O';
	else if (dir == 'd') nx = t->x + 1, t->direccion = 'E';
	else return;
	
	if(!dentro(nx, ny)) return;
//Compruebo que el destino sea un bloque vacio y no este ocupado
	if(bloque_vacio(mapa, nx, ny)){   
		borrar_tanque(mapa, t);//Elimino el rastro
		t->x = nx;
		t->y = ny;//Actualizo la ubicacion del tanque

		aplicar_tp(t);//Aplico tp en caso de ser necesario
		dibujar_tanque(mapa, t);//La ubico en el mapa
	}

}


int colisiona_tanque(Tanque *t, int bx, int by){
    return(dentro(bx, by) && bx == t->x && by == t->y); //Si las coordenadas de la bala son iguales a las del tanque retorna 1, y 0 de caso contrario
}


void disparar(Bala *b, Tanque *t){
	if(b->activa) return; //Si la bala está activa, no se puede disparar

//La bala copia las stats del tanque que las dispara
	b->activa = 1;
	b->direccion = t->direccion;
	b->quien = t->id; //Guardo quien disparó la bala

    //La bala hereda la posicion del tanque
	int bx = t->x;
	int by = t->y; 

	if(dentro(bx, by)){ //Si está dentro, se dispara
		b->x = bx;
		b->y = by;
	} else {
		b->activa = 0; //Si no, no se dispara
	}
		
}

void actualizar_bala(Bala *b, int **mapa, Tanque *enemigo, Tanque *tirador){
	if (!b->activa) return;//Si la bala esta inactiva, no existe en el mapa


	int nx = b->x;
	int ny = b->y;

//Dependiendo de su direccion se mueve por el mapa
	if(b->direccion == 'N') ny--;
	if(b->direccion == 'S') ny++;
	if(b->direccion == 'E') nx++;
	if(b->direccion == 'O') nx--;

//Verifico que este en el mapa, y no se escape por los bordes
	if(!dentro(nx, ny)){
		b->activa = 0;
		return;
	}

    //Copio la celda que atraviesa la bala
	int celda = mapa[ny][nx];

	if(celda == MURO){ //Si es muro, se rompe y pasa a ser VACIO
		mapa[ny][nx] = VACIO;
		b->activa = 0; //Se desactiva la bala
		return;
	}

	if(celda == IRROMPIBLE){ //Si es IRROMPIBLE
		b->activa = 0; //Solo se desactiva la bala
		return;
	}

	if(colisiona_tanque(enemigo, nx, ny)){ //Si la coordenada de la bala colisiona con tanque
		b->activa = 0; //Se desactiva la bala
		enemigo->vidas--; //El enemigo pierde una vida
		if(tirador) tirador->kills++; //El que disparo, obtiene una kill

		borrar_tanque(mapa, enemigo); //Se borra el tanque muerto

		//Respawn del tanque
		if(enemigo->id == JUGADOR1){
			enemigo->x = 1;
			enemigo->y = 1;
		} else {
			enemigo->x = COLUMNAS - 2;
			enemigo->y = FILAS - 2;
		}

		dibujar_tanque(mapa, enemigo); //Se dibujan los tanques con las nuevas coordenadas
		return;
	}

	b->x = nx;
	b->y = ny;
}



//Condiciones de victoria
//Retornara:
	// 0 -> nadie gano
	// 1 -> gana jugador 1
	// 2 -> gana jugador 2
	// 3 -> empate
int evaluar_victoria(Tanque *t1, Tanque *t2, int turnos){

	if(t1->kills >= KILLS_PARA_GANAR) return 1;
	if(t2->kills >= KILLS_PARA_GANAR) return 2;
	

	if(turnos >= MAX_TURNOS){
		if(t1->kills > t2->kills) return 1;
		if(t2->kills > t1->kills) return 2;
		return 3;
	}
	return 0;

}


int guardar_partida(const char *archivo, int **mapa, Tanque *t1, Tanque *t2, Bala *b1, Bala *b2, int turnos) {
    FILE *fp = fopen(archivo, "w");
    if (!fp) return 0;

    //Línea 1: Turnos
    fprintf(fp, "%d\n", turnos);
    
    //Líneas 2 a FILAS+1: Mapa
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            fprintf(fp, "%d", mapa[i][j]);
            if (j < COLUMNAS - 1) fprintf(fp, " ");
        }
        fprintf(fp, "\n");
    }

    // Línea FILAS+2: Tanque 1
    fprintf(fp, "%d %d %c %d %d %d %d\n", 
        t1->x, t1->y, t1->direccion, t1->vidas, t1->kills, t1->id, t1->celda_debajo);
    
    // Línea FILAS+3: Tanque 2
    fprintf(fp, "%d %d %c %d %d %d %d\n", 
        t2->x, t2->y, t2->direccion, t2->vidas, t2->kills, t2->id, t2->celda_debajo);
    
    // Línea FILAS+4: Bala 1
    fprintf(fp, "%d %d %c %d %d\n", 
        b1->x, b1->y, b1->direccion, b1->activa, b1->quien);
    
    // Línea FILAS+5: Bala 2
    fprintf(fp, "%d %d %c %d %d\n", 
        b2->x, b2->y, b2->direccion, b2->activa, b2->quien);
    
    // Línea FILAS+6: TPs
    fprintf(fp, "%d %d %d %d\n", tp1_x, tp1_y, tp2_x, tp2_y);
    
    // Línea FILAS+7: Cooldown
    fprintf(fp, "%ld\n", (long)teleport_cooldown_hasta);

    fclose(fp);
    return 1;
}

int cargar_partida(const char *archivo, int **mapa, Tanque *t1, Tanque *t2, Bala *b1, Bala *b2, int *turnos) {
    FILE *fp = fopen(archivo, "r");
    if (!fp) return 0;

    // Leer turnos
    if (fscanf(fp, "%d", turnos) != 1) {
        fclose(fp);
        return 0;
    }

    // Leer mapa
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            if (fscanf(fp, "%d", &mapa[i][j]) != 1) {
                fclose(fp);
                return 0;
            }
        }
    }

    // Leer tanque 1
    if (fscanf(fp, "%d %d %c %d %d %d %d", 
           &t1->x, &t1->y, &t1->direccion, &t1->vidas, &t1->kills, &t1->id, &t1->celda_debajo) != 7) {
        fclose(fp);
        return 0;
    }

    // Leer tanque 2
    if (fscanf(fp, "%d %d %c %d %d %d %d", 
           &t2->x, &t2->y, &t2->direccion, &t2->vidas, &t2->kills, &t2->id, &t2->celda_debajo) != 7) {
        fclose(fp);
        return 0;
    }

    // Leer bala 1
    if (fscanf(fp, "%d %d %c %d %d", 
           &b1->x, &b1->y, &b1->direccion, &b1->activa, &b1->quien) != 5) {
        fclose(fp);
        return 0;
    }

    // Leer bala 2
    if (fscanf(fp, "%d %d %c %d %d", 
           &b2->x, &b2->y, &b2->direccion, &b2->activa, &b2->quien) != 5) {
        fclose(fp);
        return 0;
    }

    // Leer TPs
    if (fscanf(fp, "%d %d %d %d", &tp1_x, &tp1_y, &tp2_x, &tp2_y) != 4) {
        escanear_tp(mapa);
    }

    // Leer cooldown
    long cooldown_temp;
    if (fscanf(fp, "%ld", &cooldown_temp) == 1) {
        teleport_cooldown_hasta = (time_t)cooldown_temp;
    } else {
        teleport_cooldown_hasta = 0;
    }

    fclose(fp);

    //Buscamos las posiciones de los tanques en la partida guardada para eliminarlos y que pasen a ser jugables
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            if (mapa[i][j] == JUGADOR1 || mapa[i][j] == JUGADOR2) {//Si la celda == JUGADOR1 o JUGADOR2 los borramos
                // Verificar si hay un TP debajo para dejarlo activo
                if (tp1_x >= 0 && tp1_y >= 0 && j == tp1_x && i == tp1_y) {
                    mapa[i][j] = TP1;
                } else if (tp2_x >= 0 && tp2_y >= 0 && j == tp2_x && i == tp2_y) {
                    mapa[i][j] = TP2;
                } else {
                    mapa[i][j] = VACIO; //Borramos el tanque guardado 
                }
            }
        }
    }

    //Y ahora dibujo los tanques LIMPIOS en sus posiciones correctas
    dibujar_tanque(mapa, t1);
    dibujar_tanque(mapa, t2);

    return 1;
}

//Inicializa los tanques
void inicializar_tanque(Tanque *t, int id, int x_inicial, int y_inicial){
	t->x = x_inicial;
	t->y = y_inicial;
	t->direccion = 'N';
	t->id = id;
	t->kills = 0;
	t->vidas = MAX_VIDAS;
	t->celda_debajo = VACIO; //Debajo siempre es vacio
}

//Escanea los TPs en el mapa
void escanear_tp(int **mapa) {
    tp1_x = tp1_y = tp2_x = tp2_y = -1;
    
    for(int i = 0; i < FILAS; i++){
        for(int j = 0; j < COLUMNAS; j++){
            if(mapa[i][j] == TP1 && tp1_x == -1){
                tp1_x = j;
                tp1_y = i;
            } else if (mapa[i][j] == TP2 && tp2_x == -1){
                tp2_x = j;
                tp2_y = i;
            }
        }
    }
}