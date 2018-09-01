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
	proceso -> name = calloc(1, sizeof(token) + 1);
	strcpy(proceso -> name, token);
}


void printing_data(Proceso * pr, int count){
	if (pr -> ID == 0){
		printf("\n\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
		printf("COUNT NUMBER %i\n", count);
		printf("NAME %s\n", pr -> name);
		printf("CPU BURST %i\n", pr -> num_etapas);
		printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n\n");
	}

}


void reduce_time(Proceso * proceso){
	printf("TIEMPO RESTANTE DEL PROCESO %s: %i\n", proceso -> name, proceso -> time_list -> head -> tiempo);
	proceso -> time_list -> head -> tiempo --;
	// ESTADISTICAS
	proceso -> waiting_time++;
	sleep(3);
}

void remove_waiting_queue_to_ready_queue(Queue * waiting_queue, Queue * ready_queue, int tiempo_actual){
	Proceso * pr = pop(waiting_queue); // QUITO AL PROCESO DE LA COLA WAITING ////////////////////////////////////////////////// EEEEEEEEEEERRRRRRRRRRRRROOOOOOOOOOORRRRRRRRRRRRRRR
	printf("[t = %i] El proceso %s ha pasado a estado READY.\n", tiempo_actual, pr -> name);
	Queue * time_list = pr -> time_list;
	pop_time(time_list); // ELIMINO EL TIEMPO DE EJECUCION DEL PROCESO DE LA LISTA DE TIEMPOS
	printf("NEXT TIME %i\n", time_list -> head -> tiempo);
	push(ready_queue, pr); // AGREGO EL PROCESO A LA COLA READY
}

void check_waiting(Queue * waiting_queue, Queue * ready_queue, int tiempo_actual){
	if (waiting_queue -> size != 0){
		printf("[CHECKING AND UPDATING WAITING QUEUE]\n");
		Nodo * current = waiting_queue -> head;
		while (current != waiting_queue -> tail){
			Proceso * waiting_process = current -> proceso; // PROCESO QUE SE ESTA TRABAJANDO ACTUALMENTE
			printing_data(waiting_process, 2);
			int tiempo = waiting_process -> time_list -> head -> tiempo;
			printf("1. NOMBRE PROCESO EN COLA: %s TIEMPO %i\n", waiting_process -> name, tiempo);

			if (current == waiting_queue -> head){ // SI SOY EL PRIMER ELEMENTO DE LA COLA (HEAD)
				if (tiempo > 1){
					waiting_process -> time_list -> head -> tiempo --;
					waiting_process -> waiting_time ++;
				}
				else if (tiempo == 1){
					remove_waiting_queue_to_ready_queue(waiting_queue, ready_queue, tiempo_actual);
				}
			}
			else{ // SI ESTOY EN CUALQUIER POSICION DEL CENTRO DE LA COLA
				if (tiempo > 1){
					waiting_process -> time_list -> head -> tiempo --;
					waiting_process -> waiting_time ++;
				}
				else if (tiempo == 1){
					Nodo * previous = current -> previous;
					Nodo * next = current -> next;
					Proceso * current_process = current -> proceso;
					printf("2. NOMBRE PROCESO EN COLA: %s TIEMPO %i\n", current_process -> name, tiempo);
					Queue * time_list = current_process -> time_list;
					pop_time(time_list);
					printf("NEXT TIME %i\n", time_list -> head -> tiempo);
					// PUSH DEL PROCESO A COLA READY
					push(ready_queue, current_process);
					printf("[t = %i] El proceso %s ha pasado a estado READY.\n", tiempo_actual, current_process -> name);
					previous -> next = next;
					next -> previous = previous;
					free(current);
				}
			}
			current = current -> next;
		}
		Proceso * waiting_process = current -> proceso;
		printing_data(waiting_process, 3);
		int tiempo = waiting_process -> time_list -> head -> tiempo;
		// SI SOY EL TAIL NO VOY A HABER ITERADO EN EL WHILE ANTERIOR
		if (current == waiting_queue -> head){
			printf("3. NOMBRE PROCESO EN COLA: %s TIEMPO: %i\n", waiting_process -> name, tiempo);
			if (tiempo > 1){
				waiting_process -> time_list -> head -> tiempo --;
				waiting_process -> waiting_time ++;
			}
			else if (tiempo == 0 || tiempo == 1){
				remove_waiting_queue_to_ready_queue(waiting_queue, ready_queue, tiempo_actual);
			}
		}
		else{
			printf("TRUE FALSE %i\n", current == waiting_queue -> tail);
			//sleep(1);
			if (tiempo > 1){
				waiting_process -> time_list -> head -> tiempo --;
				waiting_process -> waiting_time ++;
			}
			else if (tiempo == 1){
				Nodo * previous = current -> previous;
				Nodo * next = current -> next;
				Proceso * current_process = current -> proceso;
				printf("4. NOMBRE PROCESO EN COLA: %s TIEMPO %i\n", current_process -> name, tiempo);
				Queue * time_list = current_process -> time_list;
				pop_time(time_list);
				printf("NEXT TIME %i\n", time_list -> head -> tiempo);
				// PUSH DEL PROCESO A COLA READY
				push(ready_queue, current_process);
				printf("[t = %i] El proceso %s ha pasado a estado READY.\n", tiempo_actual, current_process -> name);
				previous -> next = next;
				next -> previous = previous;
				free(current);
			}
		}
	}
	else{
		printf("[EMPTY WAITING QUEUE]\n");
	}
}

int processing_time(Proceso * pr, int quantum){
	int tiempo = pr -> time_list -> head -> tiempo;
	if (tiempo > quantum){
		pr -> quantum_count++;
		pr -> time_list -> head -> tiempo -= quantum;
		return quantum;
	}
	else{
		pr -> time_list -> head -> tiempo = 0;
		return tiempo;
	}
}

void sumar_espera_cola_ready(Queue * queue_ready){
	if (queue_ready -> size != 0){
		printf("%i\n", queue_ready -> size);
		printf("\n\nSUMANDO A LA COLA READY\n\n");
		Nodo * current = queue_ready -> head;
		int count = 0;
		printf("ANTES %i\n", current -> proceso -> waiting_time);
		while (count < queue_ready -> size){
			Proceso * current_proceso = current -> proceso;
			current -> proceso -> waiting_time ++;
			printf("DENTRO %i\n", current -> proceso -> waiting_time);
			current = current -> next;
			count ++;
		}
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

		//push_time(proceso -> time_list, atoi(token));
		while (token != NULL){
			//printf("TIEMPOS: %i\n", atoi(token));
			push_time(proceso -> time_list, atoi(token));
			token = strtok(NULL, " ");
		}

		print_queue(proceso -> time_list);
		push_insert(queue_procesos, proceso);
	}
	free(line);

	int t = 0;
	if(t){

		Proceso * p = pop(queue_procesos);
		Queue * q = p -> time_list;
		while (q -> size != 0){
			pop_time(q);
		}

		printf("\n");
		p = pop(queue_procesos);
		q = p -> time_list;
		while (q -> size != 0){
			pop_time(q);
		}
		printf("\n");
		p = pop(queue_procesos);
		q = p -> time_list;
		while (q -> size != 0){
			pop_time(q);
		}
		sleep(10);

	}

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

	while (queue_finished -> size != num/* && tiempo_actual < 35*/){
		printf("\n###########################################\n###########################################\n[TIEMPO ACTUAL %i] - [FINISHED QUEUE SIZE %i]\n\n", tiempo_actual, queue_finished -> size);
		if (queue_procesos -> size != 0){ // SI LA COLA DE PROCESOS TIENE ELEMNTOS
			printf("[CHECKING NEW PROCESSES]------------------------------------------------\n");

			printf("[PROCESS QUEUE COUNT %i]\n", queue_procesos -> size);
			if (queue_procesos -> head -> proceso -> tiempo_llegada == tiempo_actual){ // SI EL TIEMPO DE LLEGADA ES IGUAL AL TIEMPO ACTUAL
				Proceso * proceso_ready = pop(queue_procesos);
				printing_data(proceso_ready, 1);
				push(queue_ready, proceso_ready);
				printf("[t = %i] El proceso %s ha sido creado.\n", proceso_ready -> tiempo_llegada, proceso_ready -> name);
				printf("[t = %i] El proceso %s ha pasado a estado READY.\n", proceso_ready -> tiempo_llegada, proceso_ready -> name);
				proceso_ready -> status = 0; // ESTA EN ESTADO READY
			}
		}

		printf("\n[CHECKING WAITING QUEUE]------------------------------------------------\n");
		check_waiting(queue_waiting, queue_ready, tiempo_actual); // REVISO COLA WAITING PARA MOVER DE WAITING A READY.



		printf("\n[CHECKING CPU USAGE]----------------------------------------------------\n");

		//int tiempo_actual = 0;
		//int CPU_usada = 0; // 0 FALSE 1 TRUE
		//int tiempo_en_CPU = 0;
		//Proceso * proceso_en_CPU;
		if (!CPU_usada){ // SI LA CPU ESTA VACIA
			printf("========= EMPTY CPU =========\n");
			if (queue_ready -> size != 0){ // SI HAY PROCESOS EN COLA READY
				proceso_en_CPU = pop(queue_ready);
				printing_data(proceso_en_CPU, 4);

				printf("SIGUIENTE PROCESO READY => %s\n", proceso_en_CPU -> name);

				CPU_usada = 1;
				proceso_en_CPU -> CPU_count ++;
				printf("================> CPU BURSTS %i\n", proceso_en_CPU -> num_etapas);
				//proceso_en_CPU -> num_etapas --;
				printf("================> CPU BURSTS REMAINING %i\n", proceso_en_CPU -> num_etapas);

				if (proceso_en_CPU -> response_time == -1){
					proceso_en_CPU -> response_time = tiempo_actual - (proceso_en_CPU -> tiempo_llegada);
				}

				tiempo_en_CPU = processing_time(proceso_en_CPU, quantum);
				printf("PROCESSING TIME %i\n", tiempo_en_CPU);
				printf("[t = %i] El proceso %s ha pasado a estado RUNNING.\n", tiempo_actual, proceso_en_CPU -> name);
			}
			else{
				printf("[t = %i] IDLE (CPU vacia)\n", tiempo_actual);
			}
		}
		else{ // SI LA CPU ESTA OCUPADA
			printf("========= USED CPU =========\n");
			if (tiempo_en_CPU > 1){
				tiempo_en_CPU --;
				sumar_espera_cola_ready(queue_ready);
			}
			else if (tiempo_en_CPU == 1){
				int tiempo_restante = proceso_en_CPU -> time_list -> head -> tiempo;
				if (tiempo_restante == 0){
					if (proceso_en_CPU -> num_etapas == 1	){ // FINISHED PROCESS
						push(queue_finished, proceso_en_CPU);
						//pop_time(proceso_en_CPU -> time_list);
						proceso_en_CPU -> turnaround_time = tiempo_actual - (proceso_en_CPU -> tiempo_llegada);
						proceso_en_CPU -> num_etapas --;
						printf("[t = %i] El proceso %s ha pasado a estado FINISHED.\n", tiempo_actual, proceso_en_CPU -> name);
						printing_data(proceso_en_CPU, 5);
					}
					else{ // WAITING PROCESS
						printf("[t = %i] El proceso %s ha pasado a estado WAITING.\n", tiempo_actual, proceso_en_CPU -> name);
						pop_time(proceso_en_CPU -> time_list);
						push(queue_waiting, proceso_en_CPU);
						proceso_en_CPU -> num_etapas --;
						proceso_en_CPU -> waiting_time ++;
						printing_data(proceso_en_CPU, 5);
					}
					if (queue_ready -> size != 0){ // SI ES QUE YA SACAMOS A UNO DE LA CPU, REVISAMOS SI HAY UNO NUEVO PARA INGRESAR
						proceso_en_CPU = pop(queue_ready);
						proceso_en_CPU -> CPU_count ++;
						printing_data(proceso_en_CPU, 6);
						//proceso_en_CPU -> num_etapas --;
						if (proceso_en_CPU -> response_time == -1){
							proceso_en_CPU -> response_time = tiempo_actual - (proceso_en_CPU -> tiempo_llegada);
						}
						tiempo_en_CPU = processing_time(proceso_en_CPU, quantum);
						printf("PROCESSING TIME %i\n", tiempo_en_CPU);
						printf("[t = %i] El proceso %s ha pasado a estado RUNNING.\n", tiempo_actual, proceso_en_CPU -> name);
					}
					else{
						printf("[t = %i] IDLE (CPU vacia)\n", tiempo_actual);
						CPU_usada = 0;
					}
				}
				else{
					push(queue_ready, proceso_en_CPU);
					printf("[t = %i] El proceso %s ha pasado a estado READY.\n", tiempo_actual, proceso_en_CPU -> name);

					//proceso_en_CPU -> num_etapas --;

					printing_data(proceso_en_CPU, 7);
					if (queue_ready -> size != 0){
						proceso_en_CPU = pop(queue_ready);
						proceso_en_CPU -> CPU_count ++;
						printing_data(proceso_en_CPU, 8);
						//proceso_en_CPU -> num_etapas --;
						if (proceso_en_CPU -> response_time == -1){
							proceso_en_CPU -> response_time = tiempo_actual - (proceso_en_CPU -> tiempo_llegada);
						}
						tiempo_en_CPU = processing_time(proceso_en_CPU, quantum);
						printf("PROCESSING TIME %i\n", tiempo_en_CPU);
						printf("[t = %i] El proceso %s ha pasado a estado RUNNING.\n", tiempo_actual, proceso_en_CPU -> name);
					}
					else{
						printf("[t = %i] IDLE (CPU vacia)\n", tiempo_actual);
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
		//sleep(2);
	}

	//// PRINTINT STATISTICS EN CONSOLA
	Proceso * process_printing;
	while (queue_finished -> size != 0){
		process_printing = pop(queue_finished);
		printf("\n\n%s STATISTICS:\nCPU_count %i\nQUANTUM_count %i\nturnaround_time %i\nresponse_time %i\nwaiting_time %i\n\n", process_printing -> name, process_printing -> CPU_count, process_printing -> quantum_count, process_printing -> turnaround_time, process_printing -> response_time, process_printing -> waiting_time);
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
