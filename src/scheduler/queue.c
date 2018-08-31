#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "./queue.h"

Queue* queue_init()
{
  Queue * first;
  first = malloc(sizeof(Queue));
  first -> head = malloc(sizeof(Nodo));
  first -> size = 0;
  return first;
}

void print_queue(Queue * queue){
  printf("###############\nPRINTING QUEUE\n###############\n");
  Nodo * current = queue -> tail;
  while (current -> previous != NULL){
    printf("%i\n", current -> proceso -> tiempo_llegada);
    current = current -> previous;
  }
}

void push_insert(Queue* queue, Proceso *element)
{
  printf("\nINSERTANDO A QUEUE: ID %i TLL %i\n", element -> ID, element -> tiempo_llegada);
  Nodo * current;
  if (queue -> size == 0){
    current = queue -> head;
    current -> proceso = element;
    current -> next = calloc(1, sizeof(Nodo));
    current -> previous = NULL;
    queue -> size++;
    queue -> tail = current;
  }
  else if (queue -> size == 1){
    current = queue -> tail;
    int val = element -> tiempo_llegada;

    Nodo * new_nodo = calloc(1, sizeof(Nodo));
    new_nodo -> proceso = element;
    if (val < current -> proceso -> tiempo_llegada){
      Nodo * old_head = queue -> head;
      queue -> head = new_nodo;
      new_nodo -> next = old_head;
      old_head -> previous = new_nodo;
      new_nodo -> previous = NULL;
    }
    else{
      Nodo * old_next = current -> next;
      current -> next = new_nodo;
      new_nodo -> previous = current;
      new_nodo -> next = old_next;
      old_next -> previous = new_nodo;
    }
    queue -> size++;
  }
  else{
    current = queue -> tail;
    int val = element -> tiempo_llegada;
    while (current -> proceso -> tiempo_llegada > val && current != queue -> head){
      current = current -> previous;
    }
    if (current -> proceso -> tiempo_llegada < val){
      Nodo * new_nodo = calloc(1, sizeof(Nodo));
      new_nodo -> proceso = element;
      Nodo * old_next = current -> next;
      current -> next = new_nodo;
      new_nodo -> previous = current;
      new_nodo -> next = old_next;
      old_next -> previous = new_nodo;
    }
    else{
      Nodo * new_nodo = calloc(1, sizeof(Nodo));
      new_nodo -> proceso = element;
      Nodo * old_head = queue -> head;
      queue -> head = new_nodo;
      new_nodo -> next = old_head;
      old_head -> previous = new_nodo;
      new_nodo -> previous = NULL;
    }
    queue -> size++;
  }
  /***
  else{
    current = queue -> tail;
    int val = element -> tiempo_llegada;
    if (queue -> size == 1){
      Nodo * new_nodo = calloc(1, sizeof(Nodo));
      new_nodo -> proceso = element;
      if (val < current -> proceso -> tiempo_llegada){
        Nodo * old_head = queue -> head;
        queue -> head = new_nodo;
        new_nodo -> next = old_head;
        old_head -> previous = new_nodo;
        new_nodo -> previous = NULL;
      }
      else{
        Nodo * old_next = current -> next;
        current -> next = new_nodo;
        new_nodo -> previous = current;
        new_nodo -> next = old_next;
        old_next -> previous = new_nodo;
      }
      queue -> size++;
    }
    else{
      current = queue -> tail;
      int val = element -> tiempo_llegada;
      while (current -> proceso -> tiempo_llegada > val && current != queue -> head){
        current = current -> previous;
      }
      current = current -> next;
      Nodo * new_nodo = calloc(1, sizeof(Nodo));
      new_nodo -> proceso = element;
      if (current == queue -> head && val < current -> proceso -> tiempo_llegada){
        printf("ONE\n");
        Nodo * old_head = queue -> head;
        queue -> head = new_nodo;
        new_nodo -> next = old_head;
        old_head -> previous = new_nodo;
        new_nodo -> previous = NULL;
      }
      else{
        printf("THREE\n");
        Nodo * old_next = current -> next;
        current -> next = new_nodo;
        new_nodo -> previous = current;
        new_nodo -> next = old_next;
        old_next -> previous = new_nodo;
      }
      queue -> size++;
    }

  }
  ***/
  print_queue(queue);
}

/** Inserta un elemento al final de la lista */
void push(Queue* queue, Proceso *element)
{
  Nodo * current;
  if (queue -> size == 0){
    current = queue -> head;
    current -> proceso = element;
    current -> next = calloc(1, sizeof(Nodo));
    current -> previous = NULL;
    queue -> size++;
    queue -> tail = current;
  }
  else{
    current = queue -> tail -> next;
    current -> proceso = element;
    current -> next = calloc(1, sizeof(Nodo));
    current -> previous = queue -> tail;
    queue -> size++;
    queue -> tail = current;
  }
}

Proceso * pop(Queue* queue)
{
  if(queue -> size == 0){
    return NULL;
  }
  else{
    Proceso * output = queue -> head -> proceso;
    Nodo *current = queue -> head -> next;
    free(queue -> head);
    queue -> head = current;
    queue -> size--;
    return output;
  }
}

void queue_destroy(Queue* queue) // OJO CON ESTA FUNCION. REVISAR CONTENIDO
{
  Nodo * current = queue -> head;
  while (queue -> size != 0){
    Nodo *next = current -> next;
    //free(current -> proceso);
    printf("ORDEN %i ID %i\n", current -> proceso -> tiempo_llegada, current -> proceso -> ID);
    free(current -> proceso);
    free(current);
    current = next;
    queue -> size --;
  }
  if (queue -> size == 0){
    free(current);
    free(queue);
  }

}
