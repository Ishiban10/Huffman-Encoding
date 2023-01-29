#include "node.h"
#include "pq.h"
#include "huffman.h"
#include "io.h"
#include "defines.h"
#include "code.h"
#include "stack.h"
#include "header.h"
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

//struct for the stack
struct Stack {
    uint32_t top;
    uint32_t capacity;
    Node **items;
};


//function to create the stack 
Stack *stack_create(uint32_t capacity) {
    Stack *s = (Stack *) malloc(sizeof(Stack));

    if (s != NULL) {
        s->top = 0;
        s->capacity = capacity;
        s->items = (Node **) calloc(capacity + 1, sizeof(Node *));
    } else {
        return NULL;
    }
    return s;
}

//deletes the stack
void stack_delete(Stack **s) {
    if ((*s) != NULL) {
        free((*s)->items);
        free(*s);
        *s = NULL;
    }
} 


//checks if the stack is empty
bool stack_empty(Stack *s) {
    if (s != NULL) {
        if (s->top == 0) { //checks if the top of the stack is 0, then return true
            return true;
        } else {
            return false;
        }
    }
    return true;
}


//checks if stack is full
bool stack_full(Stack *s) {
    if (s != NULL) {
        if (s->top == s->capacity) { //checks if top of stack is equal to max capacity, then return true 
            return true;
        } else {
            return false;
        }
    }
    return false;
}


//returns size of the stack
uint32_t stack_size(Stack *s) {
    if (s != NULL) { //just returns the top of stack 
        return s->top;
    }
    return 0;
}


//pushes node onto the stack 
bool stack_push(Stack *s, Node *n) {
    if (s != NULL) {
        if (stack_full(s)) { //first checks if the stack if full
            return false;
        } else {
            s->items[s->top] = n; //adding items to the stack
            s->top += 1;
            return true;
        }
    }
    return false;
}


//popping node from the stack 
bool stack_pop(Stack *s, Node **n) {
    if (s != NULL) {
        if (stack_empty(s)) { //first checks if the stack is empty
            return false;
        } else {
            s->top -= 1;  //popping the node 
            *n = s->items[s->top];
            return true;
        }
    }
    return false;
}


//function to print the stack
void stack_print(Stack *s) {
    if (s != NULL) {
        for (uint32_t i = 0; i < s->top; i++) {
            node_print(s->items[i]);
        }
    }
}


