#include <stdio.h>
#include <stdlib.h>

#include "allDefinitions.h"

stack* new_node(void* data)
{
	stack* node = (stack*) malloc(sizeof(stack));
	node->data = data;
	return node;
}

void push_stack(stack **head, void* new_data)
{
	stack* node = new_node(new_data);
	node->next = *head;
	*head = node;
}

void* pop_stack(stack **head)
{
	if(*head == NULL) return NULL;
	stack* node = *head;
	*head = (*head)->next;
	void* ret = node->data;
	free(node);
	return ret;
}

void freeStack(stack* head){ 
	void* ret = NULL;
	while((ret=pop_stack(&head))!=NULL){
		free(ret);	
	}
}
