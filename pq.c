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


//changed pq implementation to insertion sort since heap was losing children

//struct for the priority queue
struct PriorityQueue {
    Node *head;
    Node *tail;
    uint32_t size;
    uint32_t capacity;
    Node **queue;
};

//helper function for insertion sort
void insertion_sort(PriorityQueue *q, Node *n) {
	if (q != NULL) { //checks if the queue is null
	    uint64_t top = q->size; //sets top of the queue
	    q->queue[top] = n; //sets top of the queue to be equal to the node
	    Node *temp = q->queue[top];  // temp node for saving the top
	    while (top > 0 && temp->frequency < q->queue[top - 1]->frequency) { //iterates through nodes checking it current top node is less than node one less's frequency
		
		//switches nodes
		q->queue[top] = q->queue[top - 1]; 
		q->queue[top - 1] = temp;
		top--;
	    }
	    //sets head and tail
	    q->head = q->queue[0];
	    q->tail = q->queue[q->size]; 
	}
}

//helper function for shifting items to the left after insert
void shift_down(PriorityQueue *q) {
    uint64_t iter = 1;
  
    for (uint64_t i = 0; i < pq_size(q); i++) {
        q->queue[i] = q->queue[iter]; 
        iter += 1;
    }
    q->size -= 1;
    q->head = q->queue[0];
    q->tail = q->queue[q->size - 1]; 
}

//creates the priority queue
PriorityQueue *pq_create(uint32_t capacity) { 
    PriorityQueue *q = (PriorityQueue *) malloc(sizeof(PriorityQueue));
    
    if (q != NULL) {
	    q->queue = (Node **) calloc(capacity + 1, sizeof(Node *)); 
	    q->head = q->tail = NULL;
	    q->capacity = capacity;
	    q->size = 0; 
    }
    return q;
}


//deletes the priority queue
void pq_delete(PriorityQueue **q) { 
     if (*q != NULL) {
     	    //freeing allocated memory and setting pointers to NULL	
	    free((*q)->queue);
	    (*q)->queue = NULL;
	    free(*q);
	    *q = NULL;
     }
}

//checks if the queue is empty 
bool pq_empty(PriorityQueue *q) { 
	if (q != NULL) {
		//if top of queue is 0, empty
		if (q->size == 0) {
			return true;
		} else {
			return false;
		}
	}
        return true ;
}

//checks if the queue is full
bool pq_full(PriorityQueue *q) { 

	if (q != NULL) {
		//if top of queue is equal to max capacity, full
		if (q->size == q->capacity) {
			return true;
		} else {
			return false;
		}
	}
        return false;
}

//returns size of the queue
uint32_t pq_size(PriorityQueue *q) { 
    if (q != NULL) {	
        return q->size;
    }
    return 0;
}


//enqueueing function
bool enqueue(PriorityQueue *q, Node *n) {
     if (q != NULL) {
       //checks if the queue is full first
	    if (pq_full(q) == true) {
		return false;
	    }
       //calls insertion sort if the queue size if greater than 1
	    if (q->size >= 1) { 
		insertion_sort(q, n);
		q->size++;
	    } else if (pq_empty(q) == true) { //else sets the first node eqaul to the inputted node
		q->queue[q->size] = n;
		q->head = q->queue[0];
		q->tail = q->queue[q->size];
		q->size += 1;
		//return true;
	    }
    	    return true;
      } 
      return false;
}


//dequeueing function
bool dequeue(PriorityQueue *q, Node **n) {
     if (q != NULL) {
     //checks if the queue is empty first
	    if (pq_empty(q) == true) { 
		return false;
	    }
	    //if the size of the queue is 1, sets the head and tail of linked list to be NULL
	    if (q->size == 1) { 
		*n = q->head;
		q->head = NULL;
		q->tail = NULL;
		q->size--;
	    }
	    //calls the shift down function if the size of queue is greater than 1
	    if (q->size > 1) {
		*n = q->head; 
		shift_down(q); 
	    }

	    return true;
     } 
     return false;
}

//function for printing out the queue
void pq_print(PriorityQueue *q) { 
      if (q != NULL) {
	    for (uint64_t i = 0; i < pq_size(q); i++) {
		node_print(q->queue[i]);
	    }
	    printf("\nsize: %u\n", q->size);
      }
}




/*
//Priority Queue Struct
struct PriorityQueue {
    uint32_t capacity;
    uint32_t num_nodes;
    Node **queue;
};


//helper functions for enqueing and dequeing and building the min heap of nodes
static uint32_t num_nodes = 0;


static void heapify(PriorityQueue *q, uint32_t n_nodes, uint32_t i) {
    if (n_nodes > 1) {
        uint32_t smallest = i;
        uint32_t left = (2*i)+1;
        uint32_t right = (2*i)+2;

        if ((left < n_nodes) && ((q->queue[left]->frequency) < (q->queue[smallest]->frequency))) {
            smallest = left;
        } 
        if ((right < n_nodes) && ((q->queue[right]->frequency) < (q->queue[smallest]->frequency))) {
            smallest = right;
        }

        if (smallest != i) {
            Node *temp = q->queue[smallest];
            q->queue[smallest] = q->queue[i];
            q->queue[smallest] = temp;

            heapify(q, n_nodes, smallest);
        }
    }
}



//static uint32_t num_nodes = 0;

static uint32_t l_child(uint32_t n) {
	return (2*n)+1;
}

static uint32_t r_child(uint32_t n) {
	return (2*n)+2;
}

static uint32_t parent(uint32_t n) {
	return (n-1)/2;
}


static void up_heap(Node **queue, uint32_t n) {
	while (n > 0 && queue[n]->frequency < queue[parent(n)]->frequency) {
		Node *temp = queue[n];
		queue[n] = queue[parent(n)];
		queue[parent(n)] = temp;
		n = parent(n);
	}
}


static void down_heap(Node **queue, uint32_t heap_size) {
	uint32_t n = 0;
	uint32_t smaller = 0;
	
	while (l_child(n) < heap_size) {
		if (r_child(n) == heap_size) {
			smaller = l_child(n);
		} else {
			if (queue[l_child(n)]->frequency < queue[r_child(n)]->frequency) {
				smaller = l_child(n);
			} else {
				smaller = r_child(n);
			}
		}
		if (queue[n]->frequency < queue[smaller]->frequency) {
			break;
		}
		
		Node *temp = queue[smaller];
		queue[smaller] = queue[n];
		queue[n] = temp;
		n = smaller;
	}
}

static Node **build_heap(Node **queue, uint32_t n_elements) {

	Node **heap = calloc(n_elements, sizeof(Node *));
	
	for (uint32_t i = 0; i < n_elements; i++) {
		heap[i] = queue[i];
		up_heap(heap, i);
	}
	
	return heap;
	
}

static void heap_sort(Node **queue, uint32_t n_elements) {
	Node ** heap = build_heap(queue, n_elements);
		
	//Node **sorted = calloc(n_elements, sizeof(Node *));
	
	for (uint32_t i = 0; i < n_elements; i++) {
		queue[i] = heap[0];
		heap[0] = heap[n_elements - i - 1];
		down_heap(heap, (n_elements-i));
	} 
	free(heap);
}


PriorityQueue *pq_create(uint32_t capacity) {
    PriorityQueue *pq = (PriorityQueue *) malloc(sizeof(PriorityQueue));

    if (pq != NULL) {
        pq->capacity = capacity;
        pq->queue = (Node **) calloc(capacity, sizeof(Node *));
    } else {
        return NULL;
    }
    return pq;
}

void pq_delete(PriorityQueue **q) {
    if ((*q) != NULL) {
        for (uint32_t i = 0; i < (*q)->capacity; i++) {
            node_delete(&(*q)->queue[i]);
        }
        free((*q)->queue);
        (*q)->queue = NULL;
        free((*q));
        (*q) = NULL;
    }
}

bool pq_empty(PriorityQueue *q) {
    if (q != NULL) {
        if (q->num_nodes == 0){
            return true;
        } else {
            return false;
        }
    }
    return false;
  
}

bool pq_full(PriorityQueue *q) {
    if (q != NULL) {
        if (q->num_nodes == q->capacity) {
        	return true;
        } else {
        	return false;
        }
    }   
    return false;
    
    if (num_nodes == q->capacity) {
        return true;
    } else {
        return false;
    }
    
    
}

uint32_t pq_size(PriorityQueue *q) {
    uint32_t counter = 0;
    if (q != NULL) {
        for (uint32_t i = 0; i < q->num_nodes; i++) {
            if (q->queue[i] != NULL) {
                counter += 1;
            }
        }
        return counter;
    }
    return 0;
}

bool enqueue(PriorityQueue *q, Node *n) {
    if (q != NULL) {
        if (pq_full(q) == false) {
            //Node *add = node_create(n->symbol, n->frequency);
            //add->right = n->right;
            //add->left = n->left;
            
            if (q->num_nodes == 0) {
                q->queue[0] = n;//add;
                q->num_nodes += 1;
            } else {
            	q->queue[q->num_nodes] = n;//add;
            	q->num_nodes += 1;
            	heap_sort(q->queue, q->num_nodes);

            }
            
            return true;
        } else {
            return false;
        }
    } 
    return false;
}

bool dequeue(PriorityQueue *q, Node **n) {
    if (q != NULL) {   
        if (pq_empty(q) == false) {
            //always want to dequeue the root since that has max priority
            //swap root element with last element
            //delete root element which is now in the last position, this will also be the element being returned
            //heapify the min heap to reorganize after switching root and last element

            Node *temp = q->queue[0];
            q->queue[0] = q->queue[q->num_nodes-1];
            q->queue[q->num_nodes-1] = temp;

            (*n) = q->queue[q->num_nodes-1];

            //node_delete(&q->queue[num_nodes-1]);
            q->num_nodes -= 1;
            
            heap_sort(q->queue, q->num_nodes);
            return true;
        } else {
            return false;
        }
    } 
    return false;
}

void pq_print(PriorityQueue *q) {
    if (q != NULL) {
        for (uint32_t i = 0; i < q->num_nodes; i++) {
            node_print(q->queue[i]);
        }
    }
}
*/


