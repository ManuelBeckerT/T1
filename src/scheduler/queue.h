#pragma once
#include <stdint.h>

typedef struct Proceso Proceso;

struct Proceso
{
	int ID;
	char *name;
	int tiempo_llegada;
	int num_etapas;
	int status; // 0 READY - 1 RUNNING - 2 WAITING - 3 FINISHED

	struct Queue * time_list;
	// ESTADISTICAS
	int CPU_count;
	int quantum_count;
	int turnaround_time;
	int response_time;
	int waiting_time;

};

struct Nodo
{
  Proceso * proceso;
	int tiempo; // time >=1
	int tipo; // A es 0 y B es 1
  struct Nodo * next;
  struct Nodo * previous;
};

/** Estructura de una lista ligada */
struct Queue
{
  // TODO: Completar esto
  int size;
  struct Nodo * head;
  struct Nodo * tail;
};

typedef struct Nodo Nodo;
typedef struct Queue Queue;

Queue* queue_init();

void print_queue(Queue * queue);

void push_insert(Queue* queue, Proceso *element);

/** Inserta un elemento al final de la lista */
void push(Queue* queue, Proceso *element);
Proceso * pop(Queue* queue);

void queue_destroy(Queue* queue);

void push_time(Queue* queue, int tiempo);

int pop_time(Queue* queue);

void queue_destroy_time(Queue* queue);
