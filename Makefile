CC = clang
CFLAGS = -Wall -Werror -Wextra -Wpedantic 

all: encode decode

nodetest: node.o nodetest.o
	$(CC) -o $@ $^

pqtest: node.o pq.o pqtest.o
	$(CC) -o $@ $^

codetest: code.o codetest.o
	$(CC) -o $@ $^

iotest: code.o io.o iotest.o
	$(CC) -o $@ $^

huffmantest: node.o pq.o huffman.o io.o code.o stack.o huffmantest.o
	$(CC) -o $@ $^

encode: node.o pq.o huffman.o io.o code.o stack.o encode.o
	$(CC) -o $@ $^

decode: node.o pq.o huffman.o io.o code.o stack.o  decode.o
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f node nodetest pq pqtest huffman io iotest code stack encode decode *.o

spotless:
	rm -f node pq huffman io code stack encode decode *.o

format:
	clang-format -i -style=file *.[ch]
