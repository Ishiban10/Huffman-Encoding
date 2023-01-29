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

#define OPTIONS "hvi:o:"

//stat struct for fstat
struct stat buffer_stat;
 
//helper function to organize each step in a better way



//helper function to read the infile one bit at a time
static void read_tree_one_bit(int infile, int outfile, Node *root, Header *header) {
    uint64_t length_read = 0;
    Node *temp = root;
    uint8_t bit;

    while (length_read != header->file_size) {
    	

	// reads the bit into the bit value and makes sure if runs only if valid bit
	if (read_bit(infile, &bit)) {
	        //checks if I need to go down left or right side depending on bit value
	        if (bit == 0 && temp->left != NULL) {
		    temp = temp->left;
		} else if (bit == 1 && temp->right != NULL) {
		    temp = temp->right;
		}
		
		//first check if I'm at leaf node
		if (temp->left == NULL && temp->right == NULL) {
		    write_bytes(outfile, &temp->symbol, sizeof(uint8_t));
		    
		    temp = root;
		    
		    length_read += 1;
		}
		
		
	}
    }
}

//helper function to print verbose output
static void verbose_print(int infile, Header *header) {
    fstat(infile, &buffer_stat);

    fprintf(stderr, "Compressed file size:%lu bytes\n", bytes_read);

    fprintf(stderr, "Decompressed file size:%lu bytes\n", header->file_size);

    uint64_t space_saving = 100 * (1 - (bytes_read / header->file_size));
    fprintf(stderr, "Space saved:%lu%c bytes\n", space_saving, '%');
}


//helper function to print the help manual output
static void helpman_print() {
    fprintf(stderr, "SYNOPSIS\n");
    fprintf(stderr, "	Program to decode an inputted filing using Huffman compression\n");
    fprintf(stderr, "USAGE\n");
    fprintf(stderr, "	./encode [-h] [-v] [-i infile] [-o outfile]\n");
    fprintf(stderr, "OPTIONS\n");
    fprintf(stderr, "	-h	Displays program synopsis and usage\n");
    fprintf(stderr, "	-v 	Enables verbose output\n");
    fprintf(stderr, "	-i 	Specify the input file (default: stdin)\n");
    fprintf(stderr, "	-o	Specify the output file (default: stdout)\n");

}

// remember to return 1 !!!!!!


int main(int argc, char *argv[]) {

    int opt = 0;
    int infile = STDIN_FILENO;
    int outfile = STDOUT_FILENO;
    bool verbose = false;
    bool helpman = false;
    //Header header;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {

        switch (opt) {

            case 'h':
                helpman = true;
                break;

            case 'i':  
            	infile = open(optarg, O_RDONLY | O_CREAT); 
                if (infile == -1) { 
                    fprintf(stderr, "Couldn't open this file.\n");
                    close(infile);
                    close(outfile);
                    return 1;
                } else {
                    fstat(infile, &buffer_stat);
                }
                break;

            case 'o':
            	outfile = open(optarg, O_WRONLY | O_CREAT);
                if (outfile == -1) { 
                    fprintf(stderr, "Couldn't open this file.\n");
                    close(infile);
                    close(outfile);
                    return 1;
                } 
          
                break;

            case 'v':
                verbose = true;
                break;

            default:
            	helpman_print();
            	close(infile);
            	close(outfile);
                return 1;
        }

        
    }

    if (helpman) {
        helpman_print();
        return 0;
    }

    Header header;
    
    //reading the header
    //header_read(infile, &header);
    read_bytes(infile, ((uint8_t *) &header), sizeof(Header));
    
    //checking if the magic value is the same
    if (header.magic != MAGIC) {
        fprintf(stderr, "An invalid file was passed in\n");
        close(infile);
        close(outfile);
        return 1;
    }
    

    //setting outfile permissions
    //outfile_permission_set(&header, outfile);
    fchmod(outfile, header.permissions);
    
    //initialize an array for rebuilding tree
    uint8_t tree_dump[header.tree_size];
    
    //zero out the tree_dump array
    for (uint64_t i = 0; i < header.tree_size; i++) {
        tree_dump[i] = 0;
    }
    
    //reading in dumped tree into the rebuild array
    //read_dumped_tree(infile, tree_dump, &header);
    read_bytes(infile, tree_dump, header.tree_size);

    //need to reconstruct the tree using rebuild_tree
    Node *root = rebuild_tree(header.tree_size, tree_dump);

    //reading the infile one bit at a time 
    read_tree_one_bit(infile, outfile, root, &header);

    //checking for verbose output
    if (verbose) {
        verbose_print(infile, &header);
    }

    


    //deleting tree and closing all files
    delete_tree(&root);
    close(infile);
    close(outfile);

    return 0;
}


