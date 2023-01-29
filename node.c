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
#include <ctype.h>
#include <inttypes.h>

//creates a nodes
Node *node_create(uint8_t symbol, uint64_t frequency) {

    Node *n = (Node *) malloc(sizeof(Node));

    if (n != NULL) {
        n->symbol = symbol;
        n->frequency = frequency;
        //n->left = NULL;
        //n->right = NULL;
    } 
    
    return n;
}

//deletes the node
void node_delete(Node **n) {
    if ((*n) != NULL) {
        free(*n);
        (*n) = NULL;
    } 
}

//joins a left and right node
Node *node_join(Node *left, Node *right) {
    if (!left || !right) {
    	printf("both not\n");
    }
    
    uint64_t total_freq = left->frequency + right->frequency; //calulates the frequency of both nodes

    Node *parent = node_create('$', total_freq); //creates the parent node

    parent->left = left; //sets the parent node's left child 
    parent->right = right; //sets the parent node's right child
    

    return parent;
}

//node print function
void node_print(Node *n) {
    if (n != NULL) {
        if (iscntrl(n->symbol) != 0 || isprint(n->symbol) == 0) {
            //printf("Symbol:0x%02" PRIx8 "\n", n->symbol);
	    printf("Symbol: 0x%02"PRIx8" \n", n->symbol);
        } else {
            printf("Symbol:%c\n", n->symbol);
            //printf("Symbol: 0x%02"PRIx8" \n", n->symbol);
        }
        printf("Frequency:%lu\n", n->frequency);
        
        //printing for the children
        /*
        if (n->right != NULL && n->left != NULL) {
        	if (iscntrl(n->left->symbol) != 0 || isprint(n->left->symbol) == 0) {
            		//printf("Symbol:0x%02" PRIx8 "\n", n->symbol);
            		//printf("Left Node Symbol:%d\n", n->left->symbol);
            		printf("Left Node Symbol: 0x%02"PRIx8" \n", n->symbol);
        	} else {
            		printf("Left Node Symbol:%c\n", n->left->symbol);
        	}	
        	printf("Left Node Frequency:%lu\n", n->left->frequency);
        	
        	if (iscntrl(n->right->symbol) != 0 || isprint(n->right->symbol) == 0) {
            		//printf("Symbol:0x%02" PRIx8 "\n", n->symbol);
            		//printf("Right Node Symbol:%d\n", n->right->symbol);
            		printf("Right Node Symbol: 0x%02"PRIx8" \n", n->symbol);
        	} else {
            		printf("Right Node Symbol:%c\n", n->right->symbol);
        	} 
        	printf("Right Node Frequency:%lu\n", n->right->frequency);
        }
        */
        
    }
}

//print comparing two nodes function 
bool node_cmp(Node *n, Node *m) {
    if (n != NULL && m != NULL) {
        if (n->frequency > m->frequency) {
            return true;
        } else {
            return false;
        }
    } else if (m == NULL && n != NULL) {
        return true;
    } else if (n == NULL && m != NULL) {
        return false;
    }
    return false;
}

//print for just the node symbol
void node_print_sym(Node *n) {
    if (n != NULL) {
        if (iscntrl(n->symbol) != 0 || isprint(n->symbol) == 0) {
            //printf("Symbol:0x%02" PRIx8 "\n", n->symbol);
            printf("Symbol:%d\n", n->symbol);
        } else {
            printf("Symbol:%c\n", n->symbol);
        }
    }
    return;
}
