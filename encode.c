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

//struct stat declaration for the fstat function
struct stat buffer_stat;

//helper functions to help organize different steps easier

//helper function to read the infile into histogram
static void histogram_create(int infile, uint64_t *hist) {
    uint8_t temp_buff[BLOCK];
    int num_bytes_read;
    
    while ((num_bytes_read = read_bytes(infile, temp_buff, BLOCK)) > 0) {
    	for (int i = 0; i < num_bytes_read; i++) {
    	    hist[temp_buff[i]] += 1;
    	}
    }
}


//helper function to count number of symbols 
static int count_symbols(uint64_t *hist) {
    int final = 0;
    for (int i = 0; i < ALPHABET; i++) {
    	if (hist[i] > 0) {
    	    final += 1;
    	}
    }
    return final;
}



//helper function for reading the infile the second time for writing codes
static void read_codes_2(int infile, int outfile, Code code_table[ALPHABET]) {
    uint8_t temp_buff[BLOCK];
    int num_bytes_read;
	
    lseek(infile, 0, SEEK_SET);
    while ((num_bytes_read = read_bytes(infile, temp_buff, BLOCK)) > 0) {
    	for (int i = 0; i < num_bytes_read; i++) {
    	    write_code(outfile, &code_table[temp_buff[i]]);
    	}
    }

}

//helper function to print verbose output
static void verbose_print(int infile, Header header) {
    fstat(infile, &buffer_stat);
    
    //fprintf(stderr, "Uncompressed file size: %" PRId64 " bytes\n", header.file_size);
    fprintf(stderr, "Uncompressed file size:%lu bytes\n", header.file_size);

    uint64_t compressed_size = bytes_written - header.file_size;
    //fprintf(stderr, "Compressed file size: %" PRId64 " bytes\n", compressed_size);
    fprintf(stderr, "Compressed file size:%lu bytes\n", compressed_size);

    uint64_t space_saving = 100 * (1 - ((double) compressed_size / (double) header.file_size));
    fprintf(stderr, "Space saved: %0.21lu%c\n", space_saving, '%');
    //fprintf(stderr, "Space saved:%lu bytes\n", space_saving);
}

//helper function to print the help manual output
static void helpman_print() {
    fprintf(stderr, "SYNOPSIS\n");
    fprintf(stderr, "	Program to encode an inputted filing using Huffman compression\n");
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

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {

        switch (opt) {

            case 'h':
                helpman = true;
                break;

            case 'i':   
            	infile = open(optarg, O_RDONLY);
                if (infile == -1) { 
                    fprintf(stderr, "Couldn't open this file.\n");
                    close(infile);
                    close(outfile);
                    return 1;
                } else  {
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



    //initilizes the histogram to be all 0's
    uint64_t hist[ALPHABET] = {0};
    
    //hist[0] = 1;
    //hist[1] = 1;
    /*
    if (hist[0] == 0) {
        hist[0] = 1;
    } 
    if (hist[1] == 1) {
        hist[1] = 1;
    }  
    */
    histogram_create(infile, hist);
    if (hist[0] == 0) {
        hist[0] = 1;
    } 
    if (hist[1] == 0) {
        hist[1] = 1;
    } 
    
    
    //need to set first two bits of the histogram eqaul to 1
    //checks if the first two items are 0, and if they are, I change them to 1
	
    

    //need to build tree
    Node *root = build_tree(hist);
    
    Code code_table[ALPHABET];
    
    //build codes
    build_codes(root, code_table);


    //nt actual_file = file_seeking(temporary, infile);

    //creates the header
    Header header;
    
    //setting magic
    header.magic = MAGIC;
    
    //infile permissions
    fstat(infile, &buffer_stat);
    
    //storing permissions in the header
    header.permissions = buffer_stat.st_mode; 
    
    //setting outfile's permissions
    fchmod(outfile, header.permissions);

    //number of unique suymbols in huffman tree is the number of leaves
    //can use count_leaves function to get number of unique symbols
    uint16_t unique_symbols = count_symbols(hist);

    //sets tree size in header
    header.tree_size = (3 * unique_symbols) - 1;

    //infile size 
    header.file_size = buffer_stat.st_size;
    
    //header.magic = MAGIC;
    //fstat(infile, &buff_stat);
    //header_create(infile, outfile, header, hist);

    //need to write header to outfile
    write_bytes(outfile, ((uint8_t *) &header), sizeof(header));

    //need to write tree to the outfile
    dump_tree(outfile, root);
    printf("\n");

    //reading the file for a second time
    read_codes_2(infile, outfile, code_table);

    //flushing remaining codes
    flush_codes(outfile);

    //prints out the verbose output 
    if (verbose) {
        verbose_print(infile, header);
    }



    //need to delete the tree and close in and out files
    delete_tree(&root);
    close(infile);
    close(outfile);
    return 0;
}
