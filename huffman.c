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





static Code c; //initializing the code



//function to build the tree
Node *build_tree(uint64_t hist[ALPHABET]) {
    //printf("hello\n");
    Node *root;  //initilaize left right and root nodes
    Node *left;
    Node *right;
    //bool dl;
    //bool dr;
    //bool en;
    //bool ep;
    PriorityQueue *pq = pq_create(ALPHABET); //initilize the priority queue
	
    //creates the priority queue	
    //printf("hello");
    for (uint32_t i = 0; i < ALPHABET; i ++) {
        if (hist[i] > 0) {  //check if there's a letter at this position
            Node *n = node_create(i, hist[i]);  //creates the node
            enqueue(pq, n);  //enqeues onto the priority queue
            //if (en == false) {
            //	printf("enqueue from histogram broke\n");
            //}
        }
    }

    //builds tree using priority queue
    //printf("hello");
    while (pq_size(pq) != 1) {
        Node *parent;
        dequeue(pq, &left);
        //if (dl == false) {
        //	printf("dequeue left broke\n");
        //}
        dequeue(pq, &right);
        //if (dr == false) {
        //	printf("dequeue right broke\n");
        //}
        //printf("hello\n");
        parent = node_join(left, right);
        //node_print(left);
        //node_print(right);
        //node_print(parent);
        //node_print(parent->left);
        //node_print(parent->right);
        enqueue(pq, parent);
        //ep = enqueue(pq, parent);
        //node_print(parent);
        //node_print(parent->left);
        //node_print(parent->right);
        
        //if (ep == false) {
        //	printf("enqueue parent broke\n");
        //}
    }

    dequeue(pq, &root); //dequeues root node into the intiliazed root
    //if (dm == false) {
    //	printf("Last d broke\n");
    //}
    //node_print(parent);
    pq_delete(&pq); //delete the priority queue
    
    //node_print(parent->left);
    //node_print(parent->right);

    return root;
}

//function to build the codes
void build_codes(Node *root, Code table[ALPHABET]) {
    //if I go down left side it's a 0, right side is a 1, either a 1 or a 0 is pushed into the code
    //this function will need to traverse the tree until I reach a leaf
    //when it reaches a leaf, I need to assign the symbol to be equal to the code
    if (c.top == 0) {
    	c = code_init();
    }
    
    //psuedo code given for building the codes (this will be recursive)
    uint8_t popped;

    if (root != NULL) {
        if (root->left == NULL && root->right == NULL) {
            table[root->symbol] = c;
        } else {
            code_push_bit(&c, 0);
            //printf("push\n");
            //printf("printing left\n");
            //node_print(root->left);
            //printf("printing right\n");
            build_codes(root->left, table);
            //printf("pop\n");
            code_pop_bit(&c, &popped);

            code_push_bit(&c, 1);
            build_codes(root->right, table);
            code_pop_bit(&c, &popped);
        }
    }
}


void dump_tree(int outfile, Node *root) {
    //printf("dump file\n");
    //node_print(root);
    //printf("done node print\n");
    //this function will do a post order traversal of the tree and will write L for the leaf 
    //node along with its symbol to the outfile, only writes I for intermediate nodes to the
    //outfile
    uint8_t letter_l = 'L';
    uint8_t letter_i = 'I';

    //psuedo code given for this as well
    if (root != NULL) {
        dump_tree(outfile, root->left);
        dump_tree(outfile, root->right);
	
	//printf("check\n");
        if (root->left == NULL && root->right == NULL) {
            write_bytes(outfile, &letter_l, 1);
            write_bytes(outfile, &root->symbol, 1);
            //printf("check2\n");
        } else {
            write_bytes(outfile, &letter_i, 1);
            //printf("check3\n");
        }
    }
}


//function for rebuilding the tree
Node *rebuild_tree(uint16_t nbytes, uint8_t tree[static nbytes]) {
    Node *root;
    Stack *s = stack_create(nbytes);

    for (uint16_t i = 0; i < nbytes; i++) {
        if (tree[i] == 'L') {
            Node *n = node_create(tree[i + 1], 0); //pushes leaf nodes onto the stack 
            stack_push(s, n);
        } else if (tree[i] == 'I'){
            Node *right;   
            Node *left;
            stack_pop(s, &right);
            stack_pop(s, &left);
            Node *parent = node_join(left, right); //pushes a joined node of left and right nodes if it's an interior node on to the stack 
            stack_push(s, parent);
        }
    }

    stack_pop(s, &root); //pops the root node into the initialized nodes
    stack_delete(&s); //deletes the stack 
    return root;
}

//function to delete the tree
void delete_tree(Node **root) {
    if ((*root) != NULL) {
        delete_tree(&(*root)->left);
        delete_tree(&(*root)->right);

        if ((*root)->left == NULL && (*root)->right == NULL) {
            node_delete(&(*root));
        }
    }
}

