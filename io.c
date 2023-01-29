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
#include <string.h>

//globals for read and write bytes
uint64_t bytes_read = 0;
uint64_t bytes_written = 0;

//some static variables used by the functions 
static uint8_t static_buffer_read[BLOCK];
static uint8_t static_buffer_write[BLOCK];
static int bit_index = 0;
static int bit_index2 = 0;


//function to read_bytes 
int read_bytes(int infile, uint8_t *buf, int nbytes) {

    uint64_t local_b_read = 0; //to store value returned from read
    uint64_t total_b = 0; // total bytes read
    
    for (int i = 0; i < nbytes; i++) {
    	if ((local_b_read = read(infile, buf + total_b, nbytes-total_b)) > 0) {
    		total_b += local_b_read;
    		if ((int) total_b == nbytes) { //if the total bytes read is eqaul to nbytes, break
    			break;
    		}
    	} else {
    		break;
    	}	
    }
    
    bytes_read += total_b; //add the total_bytes read from this function to the global bytes_read
    return total_b; //return bytes read from this function
 
}

//function to write bytes
int write_bytes(int outfile, uint8_t *buf, int nbytes) {


    //local bytes write so far 
    uint64_t local_b_write = 0;
    //last bytes write to check for when to break out of loop
    uint64_t total_w = 0;
    
    for (int i = 0; i < nbytes; i++) {
    	if ((local_b_write = write(outfile, buf + total_w, nbytes - total_w)) > 0) {
    		total_w += local_b_write;
    		if ((int) total_w == nbytes) { //if written bytes equals nbytes, break
    			break;
    		} 
    	} else {
    		break;
    	}
    }
    bytes_written += total_w; //add bytes written from this function to global bytes written
    
    return total_w; //return bytes written from this function
 

}

//function to read a bit
bool read_bit(int infile, uint8_t *bit) {
    int last = -1;
    uint8_t one = 1;
    uint64_t curr_num_bytes_read; 
    if (bit_index == 0) {
        curr_num_bytes_read = read_bytes(infile, static_buffer_read, BLOCK); //reads bytes if the bit index is 0
        if (curr_num_bytes_read < BLOCK) {
        	last = 8 * curr_num_bytes_read + 1;
        }
    }
    uint32_t byte = bit_index / 8; //gets byte
    uint32_t bit_location = bit_index % 8; //get bit location

    *bit = (static_buffer_read[byte] >> (bit_location & one)); //passes found bit into the bit pointer
    //bit_index += 1;
    bit_index = (bit_index + 1) % (BLOCK * 8);  //incrementing the bit index
    if (bit_index == last) { 
        return false;
    } else {
    	return true;
    }
}


//function to write the code
void write_code(int outfile, Code *c) {
    uint8_t got_bit;
    uint8_t one = 1;
    uint32_t byte;
    uint32_t bit_location;
    for (uint32_t i = 0; i < code_size(c); i++) { //iterates through each element in the code
        got_bit = code_get_bit(c, i); //gets the bit at the index
        if (got_bit == 1) {
            byte = bit_index2 / 8; //gets byte
            bit_location = bit_index2 % 8;  //gets bit location within byte
            static_buffer_write[byte] |= (one << bit_location); //sets bit at location
        } else {
            byte = bit_index2 / 8;  //gets byte
            bit_location = bit_index2 % 8;  //gets bit location within byte
            static_buffer_write[byte] &= ~(one << bit_location); //clears bit at location
        }

        bit_index2 = (bit_index2 + 1) % (BLOCK * 8);  //increments the bit index
        
        //everytime the bit index is reset back to 0, write bytes in the buffer
        if (bit_index2 == 0) {
            write_bytes(outfile, static_buffer_write, BLOCK);
            // zero out the buffer
            memset(static_buffer_write, 0, sizeof(static_buffer_write));
        }

    }
    
}

//function for flushing out extra codes
void flush_codes(int outfile) {
    //first checks if the bit_index is greater than 0 to see if I middle and there are bits that need to be written
    if (bit_index2 > 0) {
        uint64_t num_bytes = bit_index2 % (BLOCK * 8) / 8;
        //checks if the bit index is at a full byte
        if (bit_index2 % 8 > 0) {
            num_bytes += 1;
        }
        
        write_bytes(outfile, static_buffer_write, num_bytes); //writes out any bytes
    }
}



