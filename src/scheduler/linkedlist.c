// importo el archivo linkedlist.h
#include "linkedlist.h"
// Libreria estandar de C
#include <stdlib.h>
#include <stdio.h>

//////////////////////////////////////////////////////////////////////////
//                             Funciones                                //
//////////////////////////////////////////////////////////////////////////

// TODO: debes completar las funciones que estan a continuacion
// Puedes crear otras funciones aca para el
// funcionamiento interno del arreglo dinamico

/** Crea una lista inicialmente vacia y retorna el puntero */
LinkedList* linkedlist_init()
{
  LinkedList * first;
  first = malloc(sizeof(LinkedList));
  first -> head = malloc(sizeof(Nodo));
  first -> size = 0;
  //printf("INIT: Lista creada\n");
  return first;
}

/** Inserta un elemento al final de la lista */
void append(LinkedList* list, Proceso * element)
{
  Nodo * current;
  if (list -> size == 0){
    current = list -> head;
    current -> proceso = element;
    current -> previous = NULL;
    current -> next = calloc(1, sizeof(Nodo));
    list -> size++;
    list -> tail = current;

  }
  else{
    current = list -> tail -> next;
    current -> proceso = element;
    current -> next = calloc(1, sizeof(Nodo));
    current -> previous = list -> tail;
    list -> tail = current;
    list -> size++;
  }
}

/** Inserta el elemento dado en la posicion indicada */
void push_insert(LinkedList* list, Proceso *element)
{
  if(list -> size == 0){
    append(list, element);
  }
  else{
    Nodo * insertion = malloc(sizeof(Nodo));
    Nodo * current = list -> head;
    if(current -> proceso -> tiempo_llegada > element -> tiempo_llegada){
      insertion -> proceso = element;
      insertion -> next = current;
      current -> previous = insertion;
      list -> head = insertion;
      insertion -> previous = NULL;
      list -> size++;
    }
    else{
      while (current -> proceso -> tiempo_llegada <= element -> tiempo_llegada && current != NULL){
        current = current -> next;
      }
      insertion -> proceso = element;
      insertion -> next = current;
      insertion -> previous = current -> previous;
      current -> previous = insertion;
    }
  }
}


/** Libera todos los recursos asociados a la lista */
void linkedlist_destroy(LinkedList* list) // OJO CON ESTA FUNCION. REVISAR CONTENIDO
{
  Nodo * current = list -> head;

  if (list -> size == 0){
    free(current);
    //free(list -> tail);
    free(list);
  }
  else{
    while(current -> next != NULL){
      Nodo * deletion = current;
      current = current -> next;
      free(deletion);
    }
    free(current);
    //free(list -> tail);
    free(list);
  }
}
