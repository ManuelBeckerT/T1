#pragma once
#include <stdint.h>


struct Proceso
{
	int ID;
	char *name;
	int tiempo_llegada;
	int num_etapas;
	int stage; // 0 READY - 1 RUNNING - 2 WAITING - 3 FINISHED
	// ESTADISTICAS
	int CPU_count;
	int quantum_count;
	int turnaround_time;
	int response_time;
	int waiting_time;

};

typedef struct Proceso Proceso;

struct Nodo
{
  Proceso * proceso;
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

/** Inserta un elemento al final de la lista */
void push(Queue* queue, Proceso *element);

// Retorna el ultimo elemento del stack
Proceso * pop(Queue* queue);

void stack_destroy(Queue* Queue);
