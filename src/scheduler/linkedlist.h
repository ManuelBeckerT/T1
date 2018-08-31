// Esta linea sirve para que el codigo de este archivo solo se importe una vez
#pragma once

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

struct nodo
{
  Proceso * proceso;
  struct nodo * next;
  struct nodo * previous;
};

/** Estructura de una lista ligada */
struct linked_list
{
  // TODO: Completar esto
  int size;
  struct nodo * head;
  struct nodo * tail;
};

// Aqui le estoy poniendo un nombre mas simple a la lista para no tener que
// referirme a la lista como struct linked_list
/** Estructura de una lista ligada */
typedef struct nodo Nodo;
typedef struct linked_list LinkedList;



//////////////////////////////////////////////////////////////////////////
//                             Funciones                                //
//////////////////////////////////////////////////////////////////////////

//OJO: No se debe modificar nada de esto

/** Crea una lista inicialmente vacia y retorna el puntero */
LinkedList* linkedlist_init();

/** Inserta un elemento al final de la lista */
void append(LinkedList* list, Proceso * element);

/** Inserta el elemento dado en la posicion indicada */
void push_insert(LinkedList* list, Proceso * element);

/** Libera todos los recursos asociados a la lista */
void linkedlist_destroy(LinkedList* list);
