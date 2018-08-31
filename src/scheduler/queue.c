#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "./queue.h"

Queue* queue_init(){
  Queue * first;
  first = malloc(sizeof(Queue));
  first -> head = malloc(sizeof(Nodo));
  first -> size = 0;
  return first;
}

void print_queue(Queue * queue){
  //printf("###############\nPRINTING QUEUE\n");
  Nodo * current = queue -> tail;
  while (current -> previous != NULL){
    //printf("%i\n", current -> tiempo);
    current = current -> previous;
  }
}

void push_insert(Queue* queue, Proceso *element){
  Nodo * current;
  int val = element -> tiempo_llegada;
  if (queue -> size == 0){
    current = queue -> head;
    current -> proceso = element;
    current -> next = calloc(1, sizeof(Nodo));
    current -> previous = NULL;
    queue -> size++;
    queue -> tail = current;
  }
  else if (queue -> size == 1){
    current = queue -> head;
    Nodo * new_nodo = calloc(1, sizeof(Nodo));
    new_nodo -> proceso = element;
    if (val < current -> proceso -> tiempo_llegada){ // SI EL NUEVO PROCESO EMPIEZA ANTES
      Nodo * old_head = queue -> head;
      queue -> head = new_nodo;
      new_nodo -> previous = NULL;
      new_nodo -> next = old_head;
      old_head -> previous = new_nodo;
      queue -> tail = old_head;
    }
    else{ // SIE EL ANTIGUO PROCESO EMPIEZA ANTES O AL MISMO TIEMPO
      Nodo * old_next = current -> next;
      queue -> head -> next = new_nodo;
      new_nodo -> previous = queue -> head;
      new_nodo -> next = old_next;
      queue -> tail = new_nodo;
    }
    queue -> size++;
  }
  else{
    Nodo * new_nodo = calloc(1, sizeof(Nodo));
    new_nodo -> proceso = element;
    current = queue -> tail;
    while (current -> proceso -> tiempo_llegada > val && current != queue -> head){ // BUSCO HASTA ENCONTRAR UN NODO CON LA VALOR IGUAL O MENOR
      current = current -> previous;
    }
    if (current == queue -> tail){ // SI ME QUEDE EN LA TAIL
      Nodo * old_next = current -> next;
      current -> next = new_nodo;
      new_nodo -> previous = current;
      new_nodo -> next = old_next;
      queue -> tail = new_nodo;
    }
    else if (current == queue -> head){
      if (val < current -> proceso -> tiempo_llegada){ // SI EL NUEVO PROCESO EMPIEZA ANTES
        Nodo * old_head = queue -> head;
        queue -> head = new_nodo;
        new_nodo -> previous = NULL;
        new_nodo -> next = old_head;
        old_head -> previous = new_nodo;
      }
      else{ // SIE EL ANTIGUO PROCESO EMPIEZA ANTES O AL MISMO TIEMPO
        Nodo * old_next = current -> next;
        queue -> head -> next = new_nodo;
        new_nodo -> previous = queue -> head;
        new_nodo -> next = old_next;
        queue -> tail = new_nodo;
      }
    }
    else{
      Nodo * old_next = current -> next;
      current -> next = new_nodo;
      new_nodo -> previous = current;
      new_nodo -> next = old_next;
    }
    queue -> size++;
  }
}

/** Inserta un elemento al final de la lista */
void push(Queue* queue, Proceso *element){
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

Proceso * pop(Queue* queue){
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

void push_time(Queue* queue, int tiempo){
  Nodo * current;
  if (queue -> size == 0){
    current = queue -> head;
    current -> tipo = 0;
    current -> tiempo = tiempo;
    current -> next = calloc(1, sizeof(Nodo));
    current -> previous = NULL;
    queue -> size++;
    queue -> tail = current;
  }
  else{
    current = queue -> tail -> next;
    current -> tipo = 0;
    current -> tiempo = tiempo;
    current -> next = calloc(1, sizeof(Nodo));
    current -> previous = queue -> tail;
    queue -> size++;
    queue -> tail = current;
  }
}

int pop_time(Queue* queue){
  if(queue -> size == 0){
    return -1;
  }
  else{
    Nodo *current = queue -> head -> next;
    printf("OLD POP TIME %i\n", queue -> head -> tiempo);
    free(queue -> head);

    queue -> head = current;

    printf("NEW POP TIME %i\n\n", queue -> head -> tiempo);
    queue -> size--;
    return queue -> head -> tiempo;
  }
}

void queue_destroy_time(Queue* queue){
  Nodo * current = queue -> head;
  while (queue -> size != 0){
    Nodo *next = current -> next;
    free(current);
    current = next;
    queue -> size --;
  }
  if (queue -> size == 0){
    free(current);
    free(queue);
  }

}

void queue_destroy(Queue* queue){
  printf("DESTRUYENDO LA COLA\n");
  Nodo * current = queue -> head;
  while (queue -> size != 0){
    Nodo *next = current -> next;
    //free(current -> proceso);
    printf("ORDEN %i ID %i\n", current -> proceso -> tiempo_llegada, current -> proceso -> ID);
    queue_destroy_time(current -> proceso -> time_list);
    free(current -> proceso -> name);
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
