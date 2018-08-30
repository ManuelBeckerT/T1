#pragma once
#include <stdint.h>

struct Data
{
	// DIFERENCIADORES
	uint64_t *ships;
	uint64_t *asteroids;

	uint64_t asteroid_count;

	// POSICION ULTIMO DISPARO
	uint64_t row;
	uint64_t col;

	uint64_t hash;

	struct Data * parent;

};

typedef struct Data Data;

struct Nodo
{
  Data * val;
  struct Nodo * next;
  struct Nodo * previous;
};

/** Estructura de una lista ligada */
struct Stack
{
  // TODO: Completar esto
  int size;
  struct Nodo * head;
  struct Nodo * tail;
};

typedef struct Nodo Nodo;
typedef struct Stack Stack;

Stack* stack_init();

/** Inserta un elemento al final de la lista */
void push(Stack* stack, Data *element);

// Retorna el ultimo elemento del stack
Data * pop(Stack* stack);

void stack_destroy(Stack* stack);
