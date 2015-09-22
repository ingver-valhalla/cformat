OBJ = in_out.o buf.o token.o
SOURCE = in_out.c cformat.c token.c

cformat: $(OBJ) parser.o cformat.o 
	gcc -g -o cformat $(OBJ) cformat.o
	
test_token: $(OBJ) test_token.o
	gcc -o test_token $(OBJ) test_token.o
test_token.o: test_token.c
	gcc -g -Wall -c test_token.c

test_parser: $(OBJ) parser.o test_parser.o
	gcc -o test_parser $(OBJ) parser.o test_parser.o
test_parser.o: test_parser.c
	gcc -g -Wall -c test_parser.c

in_out.o: in_out.c in_out.h 
	gcc -g -Wall -c in_out.c
buf.o: buf.c buf.h
	gcc -g -Wall -c buf.c
token.o: token.c token.h
	gcc -g -Wall -c token.c
parser.o: parser.c parser.h
	gcc -g -Wall -c parser.c
cformat.o: cformat.c 
	gcc -g -Wall -c cformat.c

clean:
	rm cformat test_token test_parser *.o
