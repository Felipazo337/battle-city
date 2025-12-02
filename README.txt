# battle-city
Tarea semestral
PARA COMPILAR:
Aclarar tener instalado SDL2 y GCC para poder compilar
 gcc -o battlecity.exe ver.c logica.c Entrada.c -lSDL2 -lSDL2_image -lm 
PARA EJECUTAR:
 ./battlecity.exe
AVISO: NO CERRAR LA TERMINAL, ahi se verán datos como el turno en el que se encuentra, vidas y kills de cada jugador, además se mostrarán
datos importantes al guardar/cargar partida y al ganar un jugador.

Controles:
Player 1: WASD para movimiento y ESPACIO para disparar
Player 2: Controles por flechas para movimiento y boton enter para disparar

Condiciones de victoria:
-Matar 3 veces al rival antes que él te mate a ti.
-Ser el que más veces a matado al otro en 300 turnos.
Se contará como turno cada disparo/movimiento de cualquier jugador

Para guardar partida presionar escape durante el juego para abir el menu de pausa el cual será manejado por las flechas del teclado y usar enter para elegir, al seleccionar guardar, volver a la terminal y seguir instrucciones.
