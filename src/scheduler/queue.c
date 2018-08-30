#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "./queue.h"

Stack* stack_init()
{
  Stack * first;
  first = malloc(sizeof(Stack));
  first -> head = malloc(sizeof(Nodo));
  first -> size = 0;
  return first;
}

/** Inserta un elemento al final de la lista */
void push(Stack* stack, Data *element)
{
  Nodo * current;
  if (stack -> size == 0){
    current = stack -> head;
    current -> val = element;
    current -> next = calloc(1, sizeof(Nodo));
    current -> previous = NULL;
    stack -> size++;
    stack -> tail = current;
  }
  else{
    current = stack -> tail -> next;
    current -> val = element;
    current -> next = calloc(1, sizeof(Nodo));
    current -> previous = stack -> tail;
    stack -> size++;
    stack -> tail = current;
  }
}

Data * pop(Stack* stack)
{
  if(stack -> size == 0){
    return NULL;
  }
  else{
    Data * output = stack -> head -> val;
    Nodo *current = stack -> head -> next;
    free(stack -> head);
    stack -> head = current;
    stack -> size--;
    return output;
  }
}

void stack_destroy(Stack* stack) // OJO CON ESTA FUNCION. REVISAR CONTENIDO
{
  Nodo * current = stack -> head;
  while (stack -> size != 0){
    Nodo *next = current -> next;
    free(current);
    current = next;
    stack -> size --;
  }
  if (stack -> size == 0){
    free(current);
    free(stack);
  }

}
