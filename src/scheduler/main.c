#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include "./queue.h"
#include <string.h>

int count_lines(char * archivo){
	FILE* input_file = fopen(archivo, "r");
	int count = 0;  // Line counter (result)
	char c;  // To store a character read from file
	// Check if file exists
	if (input_file == NULL)
	{
	    printf("Could not open file %s", archivo);
	    return 0;
	}
	// Extract characters from file and store in character c
	for (c = getc(input_file); c != EOF; c = getc(input_file))
	    if (c == '\n') // Increment count if this character is newline
	        count = count + 1;

	// Close the file
	fclose(input_file);
	return count;
}

void proceso_init(Proceso * proceso, char * token){
	proceso -> CPU_count = 0;
	proceso -> quantum_count = 0;
	proceso -> response_time = -1;
	proceso -> turnaround_time = 0;
	proceso -> waiting_time = 0;
	proceso -> status = 0; // READY
	proceso -> name = calloc(1, sizeof(token));
	strcpy(proceso -> name, token);
}

void reduce_time(Proceso * proceso){
	printf("%s TIME: %i\n", proceso -> name, proceso -> time_list -> head -> tiempo);
	proceso -> time_list -> head -> tiempo --;
	// ESTADISTICAS
	proceso -> waiting_time++;
}

void remove_waiting_queue_to_ready_queue(Queue * waiting_queue, Queue * ready_queue, int tiempo_actual){
	Proceso * pr = pop(waiting_queue); // QUITO AL PROCESO DE LA COLA WAITING
	printf("[t = %i] El proceso %s ha pasado a estado READY.\n", tiempo_actual, pr -> name);
	Queue * time_list = pr -> time_list;
	pop(time_list); // ELIMINO EL TIEMPO DE EJECUCION DEL PROCESO DE LA LISTA DE TIEMPOS
	push(ready_queue, pr); // AGREGO EL PROCESO A LA COLA READY
}

int check_waiting(Queue * waiting_queue, Queue * ready_queue, int tiempo_actual){
	if (waiting_queue -> size != 0){
		Nodo * current = waiting_queue -> head;
		if (current == waiting_queue -> tail){
			Proceso * pr = current -> proceso;
			int tiempo = pr -> time_list -> head -> tiempo;
			if (tiempo == 0){ // SI SE ACABA EL TIEMPO DE WAITING -> PASA A ESTADO READY
				remove_waiting_queue_to_ready_queue(waiting_queue, ready_queue, tiempo_actual);
			}
			else{
				reduce_time(current -> proceso);
			}
		}
		else{
			while (current != waiting_queue -> tail){
				Proceso * pr = current -> proceso;
				int tiempo = pr -> time_list -> head -> tiempo;
				if (tiempo == 0){ // SI SE ACABA EL TIEMPO DE WAITING -> PASA A ESTADO READY
					remove_waiting_queue_to_ready_queue(waiting_queue, ready_queue, tiempo_actual);
				}
				else{
					reduce_time(current -> proceso);
				}
			}
			Proceso * pr = current -> proceso;
			int tiempo = pr -> time_list -> head -> tiempo;
			if (tiempo == 0){ // SI SE ACABA EL TIEMPO DE WAITING -> PASA A ESTADO READY
				remove_waiting_queue_to_ready_queue(waiting_queue, ready_queue, tiempo_actual);
			}
			else{
				reduce_time(current -> proceso);
			}
		}
	}
	else{
		printf("NO HAY PROCESOS EN ESTADO WAITING\n");
	}
	return 0;
}

int processing_time(Proceso * pr, int quantum){
	int tiempo = pr -> time_list -> head -> tiempo;
	if (tiempo >= quantum){
		printf("TIEMPO CPU %i - TIEMPO ASIGNADO %i\n", tiempo, quantum);
		pr -> quantum_count++;
		pr -> time_list -> head -> tiempo -= quantum;
		return quantum;
	}
	else{
		printf("TIEMPO CPU %i - TIEMPO ASIGNADO %i\n", tiempo, tiempo);
		pr -> time_list -> head -> tiempo = 0;
		return tiempo;
	}
}

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

	printf("QUANTUM: %i\n", quantum);

	FILE* input_file = fopen(argv[1], "r");

	int num = count_lines(argv[1]);
	printf("NUMERO DE LINEAS %i\n\n", num);

	/* Falló la apertura del archivo */
	if(!input_file)
	{
		printf("¡El archivo %s no existe!\n", argv[1]);
		return 2;
	}

	//######################################
	//###    LECTURA DEL ARCHIVO TXT     ###
	//######################################

	// Basado en esta página https://rosettacode.org/wiki/Read_a_file_line_by_line
	// LECTURA INPUT.TXT linea por linea
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	if (input_file == NULL)
		exit(EXIT_FAILURE);

	Queue * queue_procesos = queue_init();

	int position = 0;
	while ((read = getline(&line, &len, input_file)) != -1){
		Proceso * proceso = malloc(sizeof(Proceso));
		proceso -> ID = position;
		position++;


		printf("%s", line);
		char* token = strtok(line, " ");
		proceso_init(proceso, token);
		//printf("NAME: %s\n", proceso -> name);
		token = strtok(NULL, " ");

		proceso -> tiempo_llegada = atoi(token);
		token = strtok(NULL, " ");
		//printf("TIEMPO LLEGADA: %i\n", proceso -> tiempo_llegada);
		proceso -> num_etapas = atoi(token);
		token = strtok(NULL, " ");
		//printf("CPU BURST: %i\n", proceso -> num_etapas);
		proceso -> time_list = queue_init();

		push_time(proceso -> time_list, atoi(token));
		while (token != NULL){
			//printf("TIEMPOS: %i\n", atoi(token));
			push_time(proceso -> time_list, atoi(token));
			token = strtok(NULL, " ");
		}

		print_queue(proceso -> time_list);
		push_insert(queue_procesos, proceso);
	}
	free(line);

	printf("\n\n### SIZE QUEUE PROCESOS %i ###\n\n", queue_procesos -> size);

	//######################################
	//##   SIMULACION DE LOS PROCESOS     ##
	//######################################

	Queue * queue_ready = queue_init();
	Queue * queue_waiting = queue_init();
	Queue * queue_finished = queue_init();

	int tiempo_actual = 0;
	int CPU_usada = 0; // 0 FALSE 1 TRUE
	int tiempo_en_CPU = 0;
	Proceso * proceso_en_CPU;


	while (queue_finished -> size != num){
		printf("\n###############################\nQUEUE FINISHED SIZE %i PROCESS COUNT %i\n", queue_finished -> size, num);
		//sleep(1);
		if (queue_procesos -> size != 0){ // SI LA COLA DE PROCESOS TIENE ELEMNTOS
			if (queue_procesos -> head -> proceso -> tiempo_llegada == tiempo_actual){ // SI EL TIEMPO DE LLEGADA ES IGUAL AL TIEMPO ACTUAL
				Proceso * proceso_ready = pop(queue_procesos);
				push(queue_ready, proceso_ready);
				printf("[t = %i] El proceso %s ha sido creado.\n", proceso_ready -> tiempo_llegada, proceso_ready -> name);
				proceso_ready -> status = 0; // ESTA EN ESTADO READY

			}
		}

		check_waiting(queue_waiting, queue_ready, tiempo_actual); // REVISO COLA WAITING PARA MOVER DE WAITING A READY.

		if (!CPU_usada){ // SI LA CPU ESTA VACIA
			printf("CPU VACIA\n");
			if (queue_ready -> size != 0){ // SI HAY PROCESOS EN LA COLA READY
				proceso_en_CPU = pop(queue_ready); // PROCESO A ENTRAR A LA CPU
				proceso_en_CPU -> num_etapas --; //UNA ETAPA MENOS

				CPU_usada = 1; // ESTA OCUPADA
				tiempo_en_CPU = processing_time(proceso_en_CPU, quantum);

				// ESTADISTICAS
				proceso_en_CPU -> CPU_count ++;
				if (proceso_en_CPU -> response_time == -1){
					proceso_en_CPU -> response_time = tiempo_actual - (proceso_en_CPU -> tiempo_llegada);
				}
				printf("[t = %i] El proceso %s ha pasado a estado RUNNING.\n", tiempo_actual, proceso_en_CPU -> name);
			}
			else{
				printf("[t = %i] IDLE (CPU vacia)\n", tiempo_actual);
			}
		}
		else{ // SI LA CPU ESTA OCUPADA
			if (tiempo_en_CPU > 0){ // AUN NO SE ACABA SU TIEMPO DE EJECUCION
				tiempo_en_CPU --;
			}
			else{
				if (proceso_en_CPU -> num_etapas == 0){
					printf("[t = %i] El proceso %s ha pasado a estado FINISHED.\n", tiempo_actual, proceso_en_CPU -> name);
					push(queue_finished, proceso_en_CPU);
					printf("QUEUE FINISHED SIZE %i\n", queue_finished -> size);
					//sleep(10);
					if (queue_ready -> size != 0){
						proceso_en_CPU = pop(queue_ready);
						tiempo_en_CPU = processing_time(proceso_en_CPU, quantum);
						proceso_en_CPU -> num_etapas --; //UNA ETAPA MENOS
						proceso_en_CPU -> CPU_count ++; //UNA ETAPA MENOS
						if (proceso_en_CPU -> response_time == -1){
							proceso_en_CPU -> response_time = tiempo_actual - (proceso_en_CPU -> tiempo_llegada);
						}
					}
					else{
						CPU_usada = 0;
					}
				}
				else{
					int tiempo = proceso_en_CPU -> time_list -> head -> tiempo;
					if (tiempo != 0){
						printf("[t = %i] El proceso %s ha pasado a estado READY.\n", tiempo_actual, proceso_en_CPU -> name);
						push(queue_ready, proceso_en_CPU);
					}
					else{
						printf("[t = %i] El proceso %s ha pasado a estado WAITING.\n", tiempo_actual, proceso_en_CPU -> name);
						push(queue_waiting, proceso_en_CPU);
					}
					if (queue_ready -> size != 0){
						proceso_en_CPU = pop(queue_ready);
						tiempo_en_CPU = processing_time(proceso_en_CPU, quantum);
						proceso_en_CPU -> num_etapas --; //UNA ETAPA MENOS
						proceso_en_CPU -> CPU_count ++; //UNA ETAPA MENOS
						if (proceso_en_CPU -> response_time == -1){
							proceso_en_CPU -> response_time = tiempo_actual - (proceso_en_CPU -> tiempo_llegada);
						}
					}
					else{
						CPU_usada = 0;
					}
				}
			}
		}

		// CONSIDERAR QUE EL TIEMPO DE WAITING ES TAMBIEN EL TIEMPO QUE PASAN EN LA COLA DE READY
		// CONSIDERAR QUE EL TIEMPO DE WAITING ES TAMBIEN EL TIEMPO QUE PASAN EN LA COLA DE READY
		// CONSIDERAR QUE EL TIEMPO DE WAITING ES TAMBIEN EL TIEMPO QUE PASAN EN LA COLA DE READY
		// CONSIDERAR QUE EL TIEMPO DE WAITING ES TAMBIEN EL TIEMPO QUE PASAN EN LA COLA DE READY


		tiempo_actual ++;
		//sleep(1);
	}

	//// PRINTINT STATISTICS EN CONSOLA
	Proceso * process_printing;
	while (queue_finished -> size != 0){
		process_printing = pop(queue_finished);
		printf("%s STATISTICS:\nCPU_count %i\nQUANTUM_count %i\nturnaround_time %i\nresponse_time %i\nwaiting_time %i\n\n", process_printing -> name, process_printing -> CPU_count, process_printing -> quantum_count, process_printing -> turnaround_time, process_printing -> response_time, process_printing -> waiting_time);
	}


	// LIBERAR MEMORIA DESPUES DE LA EJECUCION
	printf("\n");

	queue_destroy(queue_ready);
	queue_destroy(queue_waiting);
	queue_destroy(queue_finished);
	queue_destroy(queue_procesos);
	fclose(input_file);

	return 0;
}
