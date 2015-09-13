OBJ = in_out.o buf.o token.o
SOURCE = in_out.c cformat.c token.c

cformat: $(OBJ) cformat.o 
	gcc -g -o cformat $(OBJ) cformat.o
token_test: $(OBJ) token_test.o
	gcc -g -o token_test $(OBJ) token_test.o
token_test.o: token_test.c
	gcc -g -Wall -c token_test.c
in_out.o: in_out.c in_out.h 
	gcc -g -Wall -c in_out.c
buf.o: buf.c buf.h
	gcc -g -Wall -c buf.c
token.o: token.c token.h
	gcc -g -Wall -c token.c
cformat.o: cformat.c 
	gcc -g -Wall -c cformat.c
clean:
	rm cformat token_test *.o
