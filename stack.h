#define STACK

typedef struct stack_t {
	struct stack_t* next;
	void* data;
} stack;

stack* new_node(void* data);
void push_stack(stack** head, void* new_data);
void* pop_stack(stack** head);
void freeStack(stack* head);
