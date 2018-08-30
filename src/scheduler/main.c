#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include "./queue.h"

struct Proceso
{


};

typedef struct Proceso Proceso;

int main(int argc, char** argv)
{

	int quantum = 3;

	if (argc > 4)
	{
		printf("Modo de uso: ./scheduler input.txt output.csv 5\n");
		return 0;
	}
	else if (argc == 4){
		quantum = atoi(argv[3]);
		printf("%i\n", quantum);
	}

	printf("%i\n", quantum);
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

	return 0;
}
