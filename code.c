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


//function to initialize the code
Code code_init(void) {
    Code new_code;
    new_code.top = 0;
    for (int i = 0; i < MAX_CODE_SIZE; i ++) {
        new_code.bits[i] = 0;
    }
    return new_code;
}


//returns the size of the code
uint32_t code_size(Code *c) {
    if (c != NULL) {
    	//return the top of the code
        return c->top;
    }
    return 1;
}

//function to check if the code is empty
bool code_empty(Code *c) {
    if (c != NULL) {
        if (c->top == 0) {
            return true;
        } else {
            return false;
        }
    }
    return true;
}


//function to check if the code is full
bool code_full(Code *c) {
    if (c != NULL) {
        if (c->top == MAX_CODE_SIZE) {
            return true;
        } else {
            return false;
        }
    }
    return false;
}

//function to set a bit
bool code_set_bit(Code *c, uint32_t i) { 
    uint8_t one = 1;
    if (c != NULL) {
        if (i > c->top) {
            return false;
        } else {
        //get the byte and bit location
            uint32_t byte = i / 8;
            uint32_t bit = i % 8;
            c->bits[byte] |= one << bit;  //setting the bit
            //c->bits[i] = 1;
            return true;
        }
    }
    return false;
}


//function to clear the bit
bool code_clr_bit(Code *c, uint32_t i) {
    uint8_t one = 1;
    if (c != NULL) {
        if (i > c->top) {
            return false;
        } else {
        //getting byte and bit location
            uint8_t byte = i / 8;
            uint8_t bit = i % 8;
            c->bits[byte] &= ~(one << bit); //clearing the bit
            //c->bits[i] = 0;
            return true;
        }
    }
    return false;
}

//function to get the bit
bool code_get_bit(Code *c, uint32_t i) {
    uint8_t one = 1;
    if (c != NULL) {
        if (i > c->top) {
            return false;
        } else {
        
        //getting byte and bit location
            uint32_t byte = i / 8;
            uint32_t bit = i % 8;
	    
            uint8_t got_bit = ((c->bits[byte] >> bit) & one); //getting the bit
            //printf("getting bit\n");
            //uint8_t got_bit = c->bits[i];//((c->bits[byte] & (one << bit))) != 0;
            //printf("got bit\n");
            if (got_bit == 1) {
                return true; //returns true if the bit is 1
            } else {
                return false;  //returns false if the bit is 0
            }
        }
    }
    return false;
}

//function to push the bit 
bool code_push_bit(Code *c, uint8_t bit) {
    if (c != NULL) {
        if (code_full(c)) {
            return false;
        } else {
            if (bit) {
            	//printf("set\n");
                code_set_bit(c, c->top); //sets the bit at the top of stack if the bit is a 1
                //printf("after set\n");
                c->top += 1;
            } else {
            	//printf("clear in push\n");
            	code_clr_bit(c, c->top);
            	//printf("after clearn in push\n"); //clears the bit at the top of the stack if the bit is 0
            	c->top += 1;
            }	
            
            return true;    
        }
    }
    return false;
}


//function for popping a bit from the code
bool code_pop_bit(Code *c, uint8_t *bit) {
    //printf("popping\n");
    if (c != NULL) {
        if (code_empty(c)) {
            return false;
        } else {
            c->top -= 1; //subtract one from the top to get the location of top bit
            /*
            if (code_get_bit(c, c->top)) {
                *bit = 1;
            } else if (code_get_bit(c, c->top) == false) {
                *bit = 0;
            }
            */
            *bit = code_get_bit(c, c->top); //get the bit at the top of stack
            code_clr_bit(c, c->top);  //clear the bit after it's got
            return true;
        }
    }
    return false;
}


//function for printing the code
void code_print(Code *c) {
    if (c != NULL) {
        for (uint32_t i = 0; i < c->top; i++) {
   
            if (code_get_bit(c, i)) {
                printf("%u ", 1);
            } else if (code_get_bit(c, i) == false) {
                printf("%u ", 0);
            }
          
        }
        printf("\n");
    }
}


