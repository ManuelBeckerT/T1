#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include "random.h"
#include "./stack.h"
#include "./super_stack.h"

struct Casilla
{
	int row;
	int col;
	int type;
	int exists;
	int position;
};

typedef struct Casilla Casilla;


struct Tablero
{
	int height;
	int width;

	int ship_count;
	int asteroid_count;

	Casilla ** casillas;

	Casilla ** naves;
	Casilla ** asteroides;

	uint64_t capacidad;
	uint64_t table_size;
	Data ** hash_table;

	uint64_t * ship_randoms;
	uint64_t * asteroid_randoms;

};

typedef struct Tablero Tablero;

Casilla * checkCell(Tablero * tablero, int row, int col, int width){
	Casilla * casilla = tablero -> casillas[row*width + col];
	int type = casilla -> type;
	if (type < 4){
		if (type < 3){
			casilla -> type += 1;
			return casilla;
		}
		casilla -> type = 0;
		return casilla;
	}
	else if (type == 4 && casilla -> exists == 1){
		casilla -> exists = 0;
		return casilla;
	}
	else{
		return NULL;
	}
}

Casilla * checkDirection(Tablero * tablero, int row, int col, int type){
	int width = tablero -> width;
	int height = tablero -> height;
	if (type == 0){
		while (row >= 0){
			if (row == 0){
				return NULL;
			}
			row -= 1;
			Casilla * retorno = checkCell(tablero, row, col, width);
			if (retorno != NULL){
				return retorno;
			}
		}
	}
	else if (type == 1){
		while (col < width){
			col += 1;
			if (col == width){
				return NULL;
			}
			Casilla * retorno = checkCell(tablero, row, col, width);
			if (retorno != NULL){
				return retorno;
			}
		}
	}
	else if (type == 2){
		while (row < height){
			row += 1;
			if (row == height){
				return NULL;
			}
			Casilla * retorno = checkCell(tablero, row, col, width);
			if (retorno != NULL){
				return retorno;
			}
		}
	}
	else{
		while (col >= 0){
			if (col == 0){
				return NULL;
			}
			col -= 1;
			Casilla * retorno = checkCell(tablero, row, col, width);
			if (retorno != NULL){
				return retorno;
			}
		}
	}
	return NULL;
}

Casilla * disparar(Tablero * tablero, int row, int col){
	Casilla ** board = tablero -> casillas;
	int width = tablero -> width;
	Casilla * nave = board[row*width + col];
	int type = nave -> type;
	if (type < 4){
		Casilla * retorno = checkDirection(tablero, row, col, type);
		return retorno;
	}
	else{
		return NULL;
	}
}

void changeData(Data * data, Casilla * casilla){
	int type = casilla -> type;
	int position = casilla -> position;
	if (type < 4){
		uint64_t * ships = data -> ships;
		ships[position] = type;
	}
	else{
		uint64_t * asteroids = data -> asteroids;
		asteroids[position] = 0;
		data -> asteroid_count -= 1;
	}
}

void printData(Data * data, int ship_count, int asteroid_count){
	printf("#############################\n        STATE DATA\n");
	printf("SHIPS\n");
	for (int i = 0; i < ship_count; i++){
		printf("%lu ", data -> ships[i]);
	}
	printf("\nASTEROIDS\n");
	for (int i = 0; i < asteroid_count; i++){
		printf("%lu ", data -> asteroids[i]);
	}
	printf("\nASTEROID COUNT ");
	printf("%lu\n", data -> asteroid_count);
	if (data -> parent != NULL){
		printf("PARENT %lu\n", data -> parent -> hash);

	}
	else{
		printf("ESTADO INCIAL\n");
	}
	printf("HASH %lu\n", data -> hash);
	printf("\n#############################\n\n");

}

void undoState(Data * data, Casilla * casilla){
	int type = casilla -> type;
	int position = casilla -> position;
	if (type < 4){
		if (type == 0){
			casilla -> type = 3;
			data -> ships[position] = 3;
		}
		else{
			casilla -> type -= 1;
			data -> ships[position] -= 1;
		}
	}
	else{
		data -> asteroids[position] = 1;
		casilla -> exists = 1;
		data -> asteroid_count += 1;
	}
}

int previousType(Casilla * casilla){
	int type = casilla -> type;
	if (type < 4){
		if (type == 0){
			return 3;
		}
		else{
			return type - 1;
		}
	}
	else{
		int exists = casilla -> exists;
		if (exists == 0){
			return 1;
		}
		else{
			printf("NO DEBERIA PASAR\nNO DEBERIA PASAR\nNO DEBERIA PASAR\nNO DEBERIA PASAR\nNO DEBERIA PASAR\nNO DEBERIA PASAR\nNO DEBERIA PASAR\nNO DEBERIA PASAR\nNO DEBERIA PASAR\n");
			return 0;
		}
	}
}

Data * crearData(Data * original, int row, int col, int ship_count, int asteroid_count){
	//printf("\n\nCREATING NEW DATA\n");
	Data * estado = calloc(1, sizeof(Data));
	estado -> row = row;
	estado -> col = col;
	estado -> parent = original;
	estado -> asteroid_count = original -> asteroid_count;

	uint64_t * info_ships = calloc(ship_count, sizeof(uint64_t));
	uint64_t * info_asteroids = calloc(asteroid_count, sizeof(uint64_t));

	for (int i = 0; i < ship_count; i++){
		uint64_t val = original -> ships[i];
		//printf("%lu ", val);
		info_ships[i] = val;
	}
	//printf("\n");
	for (int i = 0; i < asteroid_count; i++){
		uint64_t val = original -> asteroids[i];
		info_asteroids[i] = val;
		//printf("%lu ", val);
	}
	//printf("\n");
	estado -> asteroids = info_asteroids;
	estado -> ships = info_ships;

	//printf("\nCHECKING CREAR DATA\n");
	//for (int i = 0; i < ship_count; i++){
	//	printf("%lu ", estado -> ships[i]);
	//}
	//printf("\n");
	//for (int i = 0; i < asteroid_count; i++){
	//	printf("%lu ", estado -> asteroids[i]);
	//}
	//printf("\n");

	return estado;
}

uint64_t updateHash(uint64_t hash, int type, int old_type, int new_type, int position, int ship_count, int asteroid_count, uint64_t * ships_randoms, uint64_t * asteroids_randoms){
	uint64_t retorno = hash;
	if (type < 4){
		//printf("UPDATE SHIP HASH\n");
		uint64_t ran = ships_randoms[old_type*ship_count + position];
		retorno ^= ran;
		uint64_t new_ran = ships_randoms[new_type*ship_count + position];
		retorno = retorno^new_ran;
	}
	else{
		//printf("UPDATE ASTEROID HASH\n");
		uint64_t ran = asteroids_randoms[old_type*asteroid_count + position];
		retorno = retorno^ran;
		uint64_t new_ran = asteroids_randoms[new_type*asteroid_count + position];
		retorno = retorno^new_ran;
	}
	return retorno;
}

uint64_t generateHash(Data * data, uint64_t * ships_randoms, uint64_t * asteroids_randoms, int ship_count, int asteroid_count){
	uint64_t * ships = data -> ships;
	uint64_t * asteroids = data -> asteroids;
	uint64_t retorno = 0;

	for (int i = 0; i < ship_count; i++){
		int type = ships[i];
		uint64_t ran = ships_randoms[type*ship_count + i];
		retorno ^= ran;
	}
	for (int i = 0; i < asteroid_count; i++){
		int exists = asteroids[i];
		uint64_t ran = asteroids_randoms[exists*asteroid_count + i];
		retorno ^= ran;
	}

	return retorno;
}

int checkState(Data * hash_data, Data * data, Tablero * tablero){
	uint64_t * ships1 = hash_data -> ships;
	uint64_t * asteroids1 = hash_data -> asteroids;
	uint64_t * ships2 = data -> ships;
	uint64_t * asteroids2 = data -> asteroids;
	int asteroid_count = tablero -> asteroid_count;
	int ship_count = tablero -> ship_count;
	for (int i = 0; i < ship_count; i++){
		if (ships1[i] != ships2[i]){
			return 0;
		}
	}
	for (int i = 0; i < asteroid_count; i++){
		if (asteroids1[i] != asteroids2[i]){
			return 0;
		}
	}
	return 1;
}

int insertHashTable(uint64_t hash, uint64_t slot, Data ** hash_table, Data * data, Tablero * tablero){
	//printf("\n***************************\n**      DEBUGGEANDO\n***************************\n");
	//printf("%lu %lu %p %p %p\n", hash, slot, hash_table, data, tablero);
	//printf("%p\n", hash_table[slot]);
	//printf("HELLO\n");
	if (hash_table[slot] == NULL){
		//printf("\n********************************************************************************************\n EMPTY SLOT\n\n");
		////printf("ORIGINAL HASH: %lu \n", hash);
		data -> hash = hash;
		////printf("HASH EN DATA %lu\n", data -> hash);

		////printf("MODULE HASH %lu\n", hash);
		hash_table[slot] = data;
		//printf("HASH EN HASH_TABLE %lu\n", hash_table[slot] -> hash);
		tablero -> capacidad += 1;
		////printf("\n#############\n");
		return 1;
	}
	else{
		//printf("\n#############\n COLLISION\n\n");
		if (checkState(hash_table[slot], data, tablero)){
			//printf("YA EXISTE ESTE ESTADO EN LA TABLA\nFREE DATA\n");
			free(data);
			return 0;
		}
		else{
			//printf("NEXT SLOT\n");
			uint64_t next_slot;
			uint64_t size = tablero -> table_size;
			if (slot < size){
				next_slot = slot + 1;
			}
			else{
				next_slot = 0;
			}
			//printf("OLD HASH %lu NEW HASH %lu\n", slot, next_slot);
			//sleep(2);
			insertHashTable(hash, next_slot, hash_table, data, tablero);
		}
		//printf("\n#############\n");
	}
	//printf("\n***************************\n**      DEBUGGEANDO\n***************************\n");
}

void checkTableSize(Tablero * tablero){
	Data ** hash_table = tablero -> hash_table;
	uint64_t size = tablero -> table_size;
	uint64_t capacidad = tablero -> capacidad;

	uint64_t acceptablesize = size*3/4;
	////printf("TABLES SIZE: %lu\nACCEPTABLE CAPACITY: %lu\n", size, acceptablesize);
	if (capacidad > acceptablesize){
		////printf("\nNUEVA HASH TABLE\n");
		tablero -> table_size = size*3;
		uint64_t new_size = tablero -> table_size;
		////printf("OLD SIZE: %lu NEW SIZE: %lu\n", size, new_size);

		// INSTANCIO UNA NUEVA TABLA DE HASH DE TAMANO NEW SIZE
		Data ** new_hash_table = calloc(new_size, sizeof(Data));
		//for (int i = 0; i < new_size; i++){
			////printf("%p\n", new_hash_table[i]);
		//}

		////printf("\nITERANDO SOBRE LA TABLA DE HASH ORIGINAL %lu\n", size);
		for (int i = 0; i < size; i ++){
			if (hash_table[i] != NULL){
				uint64_t hash = hash_table[i] -> hash;
				////printf("OLD HASH: %lu ", hash%size);

				hash %= new_size;
				////printf("NEW HASH %lu\n", hash);
				new_hash_table[hash] = hash_table[i];
				////printf("%p %p\n", hash_table[i], new_hash_table[hash]);
				////printf("NEW POSITION IN NEW HASH TABLE %p\n", new_hash_table[hash]);
				////printf("%lu\n", new_hash_table[hash] -> hash);

			}
			else{
				////printf("VACIO\n");
			}
		}
		free(hash_table);

	}
	else{
		////printf("OK\n");
	}
}

void loadTablero(Data * data, Tablero * tablero){

	int ship_count = tablero -> ship_count;
	int asteroid_count = tablero -> asteroid_count;
	Casilla ** naves = tablero -> naves;
	Casilla ** asteroides = tablero -> asteroides;
	uint64_t * ships = data -> ships;
	uint64_t * asteroids = data -> asteroids;
	for (int i = 0; i < ship_count; i++){
		uint64_t val = ships[i];
		//printf("%lu ", val);
		naves[i] -> type = val;
	}
	//printf("\n");
	for (int i = 0; i < asteroid_count; i++){
		uint64_t val = asteroids[i];
		//printf("%lu ", val);
		asteroides[i] -> exists = val;
	}
	//printf("\n");
}

Super_Stack * BFS(Stack * cola, Tablero * tablero, int * inserts){
	int ship_count = tablero -> ship_count;
	int asteroid_count = tablero -> asteroid_count;
	while (cola -> size != 0){
		//printf("\n\n//////////////////////////////////////////////\nCOLA SIZE %i\n", cola -> size);
		Data * current_state = pop(cola);
		//printf("COLA SIZE AFTER POP %i\n", cola -> size);

		//printf("\nTABLERO PRE LOADED\n");
		//for (int row = 0; row < tablero -> height; row++){
		//	printf("     ");
		//	for (int col = 0; col < tablero -> width; col++){
		//		printf("%i ", tablero -> casillas[row*(tablero -> width) + col] -> type);
		//	}
		//	printf("\n");
		//}
		//printf("\n");

		loadTablero(current_state, tablero);
		//printData(current_state, ship_count, asteroid_count);

		//printf("\nLOADED TABLERO\n");
		//for (int row = 0; row < tablero -> height; row++){
		//	printf("     ");
		//	for (int col = 0; col < tablero -> width; col++){
		//		printf("%i ", tablero -> casillas[row*(tablero -> width) + col] -> type);
		//	}
		//	printf("\n");
		//}
		//printf("\n");

		Casilla ** naves = tablero -> naves;

		for (int count = 0; count < ship_count; count++){
			//printf("\n===================\nNAVE %i\n", count);
			Casilla * nave = naves[count];
			//printf("%p\n", nave);
			int row = nave -> row;
			int col = nave -> col;
			//printf("NAVE ROW: %i COL: %i\n", row, col);
			Casilla * disparada = disparar(tablero, row, col);
			//printf("%p\n", disparada);
			if (disparada == NULL){
				//printf("MISSED SHOT\n");
			}
			else{
				//printf("DISPARO: %i, %i\n",  row, col);
				if (disparada -> type == 4){
					//printf("ASTEROIDE DESTRUIDO: ");
					//printf("%i %i\n", tablero -> asteroides[disparada -> position] -> row, tablero -> asteroides[disparada -> position] -> col);
				}
				else{
					//printf("ROTACION DE NAVE: ");
					//printf("%i %i\n", tablero -> naves[disparada -> position] -> row, tablero -> naves[disparada -> position] -> col);
				}
				// CURRENT_STATE AHORA TIENE TODA LA DATA DEL NUEVO ESTADO
				changeData(current_state, disparada);

				//printf("\nTABLERO DESPUES DEL DISPARO\n");
				//for (int row = 0; row < tablero -> height; row++){
				//	printf("     ");
				//	for (int col = 0; col < tablero -> width; col++){
				//		printf("%i ", tablero -> casillas[row*(tablero -> width) + col] -> type);
				//	}
				//	printf("\n");
				//}
				//printf("\n");

				Data * new_state = crearData(current_state, row, col, ship_count, asteroid_count);

				//printf("\nSHIPS AND ASTEROIDS FROM CURRENT STATE\nSHIPS\n");
				//for (int i = 0; i < ship_count; i++){
				//	printf("%lu ", new_state -> ships[i]);
				//}
				//printf("\nASTEROIDS\n");
				//for (int i = 0; i < asteroid_count; i++){
				//	printf("%lu ", new_state -> asteroids[i]);
				//}
				//printf("\n");

				//sleep(5);
				if (new_state -> asteroid_count == 0){
					//printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ SOLUCION ENCONTRADA\n");
					Super_Stack * stack = super_stack_init();
					while (new_state -> parent != NULL){
						stack_push(stack, new_state);
						//printf("%lu, %lu\n", new_state -> col, new_state -> row);
						new_state = new_state -> parent;
					}
					return stack;
				}
				int previous_type = previousType(disparada);
				int current_type = disparada -> type;
				int type = current_type;
				if (current_type == 4){
					if (previous_type == 1){
						current_type = 0;
					}
				}
				int position = disparada -> position;
				//printf("PREVIOUS %i CURRENT %i\n", previous_type, current_type);
			  uint64_t newstate_hash = updateHash(current_state -> hash, type, previous_type, current_type, position, ship_count, asteroid_count, tablero -> ship_randoms, tablero -> asteroid_randoms);
				new_state -> hash = newstate_hash;
				//printf("NEW STATE\n");
				//printData(new_state, ship_count, asteroid_count);

				checkTableSize(tablero);
				uint64_t slot = newstate_hash%(tablero -> table_size);
				//printf("%lu %lu\n", slot, tablero -> table_size);
				//printf("CURRENT NUMBER OF INSERTS: %i\n", *inserts);
				int valid = insertHashTable(newstate_hash, slot, tablero -> hash_table, new_state, tablero);
				//printf("PREVIOUS TYPE: %i NEW TYPE: %i\n", previous_type, current_type);
				if (valid == 1){
					*inserts += 1;
					push(cola, new_state);
					//printf("INSERTADO COLA SIZE %i\n", cola -> size);
				}


				undoState(current_state, disparada);
				//sleep(1);
			}
			//printf("\n********************************************************************************************\n\nNEXT NAVE\n");
		}
	}
	return NULL;
}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		////printf("Modo de uso: ./solver test.txt\n");
		return 0;
	}

	FILE* input_file = fopen(argv[1], "r");

	/* Falló la apertura del archivo */
	if(!input_file)
	{
		////printf("¡El archivo %s no existe!\n", argv[1]);
		return 2;
	}

	int height;
	int width;

	/* Leemos las dimensiones del  a partir del archivo */
	fscanf(input_file, "%d %d", &height, &width);
	//printf("SIZE OF THE BOARD %i %i\n\n", height, width);

	Tablero * tablero = malloc(sizeof(Tablero));
	tablero -> width = width;
	tablero -> height = height;

	// INICIALIZO LA TABLA DE HASH
	tablero -> table_size = 1132451; //7831 o 1132451
	tablero -> capacidad = 0;
	uint64_t size = tablero -> table_size;
	Data ** hash_table = malloc(size*sizeof(Data));
	tablero -> hash_table = hash_table;


	int asteroid_count = 0;
	int ship_count = 0;
	Casilla * board[height][width];
	/* Leemos el resto del archivo fila por fila */
	for(int row = 0; row < height; row++)
	{
		for(int col = 0; col < width; col++)
		{
			/* La dirección de cada celda */
			int tipo;

			Casilla * casilla = malloc(sizeof(Casilla));
	    casilla -> row = row;
	    casilla -> col = col;

			/* Leemos el grado de la celda */
			fscanf(input_file, "%d", &tipo);

			// Guardamos el valor de su posición
			if(tipo < 4){
	      casilla -> type = tipo;
	      ship_count += 1;
			}
			else if (tipo == 4){
	      casilla -> type = tipo;
				casilla -> exists = 1;
	      asteroid_count += 1;
			}
			else{
	      casilla -> type = tipo;
			}
			board[row][col] = casilla;
		}
	}
	tablero -> asteroid_count = asteroid_count;
	tablero -> ship_count = ship_count;
	tablero -> casillas = *board;

	fclose(input_file);

	// Codigo

	Casilla * naves[ship_count];
	Casilla * asteroides[asteroid_count];

	uint64_t ships[ship_count];
	uint64_t asteroids[asteroid_count];
	int count_s = 0;
	int count_a = 0;
	for (int i = 0; i < asteroid_count; i++){
		asteroids[i] = 1;
	}

	// CARGANDO POSICIONES A SHIPS / ASTEROIDS INICIALMENTE
	for (int row = 0; row < height; row ++){
		for (int col = 0; col < width; col ++){
			if (board[row][col] -> type < 4){
				uint64_t val = board[row][col] -> type;
				ships[count_s] = val;
				naves[count_s] = board[row][col];
				board[row][col] -> position = count_s;
				count_s += 1;
			}
			else if (board[row][col] -> type == 4){
				asteroides[count_a] = board[row][col];
				board[row][col] -> position = count_a;
				count_a += 1;
			}
		}
	}

	// GENERAMOS MATRICES DE RANDOMS
	uint64_t ships_randoms[4][ship_count];
	for (int row = 0; row < 4; row++){
		for (int col = 0; col < ship_count; col++){
			uint64_t ran = get_random_number();
			ships_randoms[row][col] = ran;
		}
	}
	uint64_t asteroids_randoms[2][asteroid_count];
	for (int row = 0; row < 2; row ++){
		for (int col = 0; col < asteroid_count; col++){
			uint64_t ran = get_random_number();
			asteroids_randoms[row][col] = ran;
		}
	}

	// ASIGNANOD VARIABLES RANDOM AL TABLERO
	tablero -> asteroid_randoms = *asteroids_randoms;
	tablero -> ship_randoms = *ships_randoms;

	// ASIGNANDO ARREGLO DE NAVES Y ASTEROIDS CASILLA AL TABLERO
	tablero -> naves = naves;
	tablero -> asteroides = asteroides;

	// CREANDO DATA PRIMER ESTADO

	Data * data = malloc(sizeof(Data));
	data -> parent = NULL;
	data -> ships = ships;
	data -> asteroids = asteroids;
	data -> asteroid_count = asteroid_count;

	for (int i = 0; i < asteroid_count; i++){
		asteroids[i] = 1;
	}

	// #########################################################
	// #######     INICIO BFS IMPLEMENTACION          ##########
	// #########################################################

	uint64_t hash = generateHash(data, tablero -> ship_randoms, tablero -> asteroid_randoms, ship_count, asteroid_count);
	////printf("ORIGINAL HASH: %lu\n", hash);
	Data ** table = tablero -> hash_table;

	//printData(data, ship_count, asteroid_count);

	uint64_t original_slot = hash%(tablero -> table_size);
	//printf("%lu %lu\n", hash, original_slot);
	//sleep(5);

	//printf("\n\nINSERTING DATA INTO HASH TABLE\n");
	insertHashTable(hash, original_slot, table, data, tablero);

	//sleep(1);
	Stack * cola = stack_init();
	push(cola, data);

	//printf("\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n                        STARTING BFS\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n\n");

	Super_Stack * stack = NULL;

	int inserts = 0;
	stack = BFS(cola, tablero, &inserts);

	//printf("TOTAL INSERTS:  %i\n", inserts);
	if (stack != NULL){
		int stack_size = stack -> size;
		//printf("NUMERO DE PASOS: %i\n", stack_size);
		for (int i = 0; i < stack_size; i++){
			Data * val = stack_pop(stack);
			printf("%lu, %lu\n", val -> col, val -> row);
		}
	}
	else{
		printf("NULL\n");
	}

	// Liberar Memoria

	free(tablero -> hash_table);
	free(tablero);

	//	super_stack_destroy(stack);
	stack_destroy(cola);
	super_stack_destroy(stack);

	for (int row = 0; row < height; row++){
		for (int col = 0; col < width; col++){
			free(board[row][col]);
		}
	}

	return 0;
}
