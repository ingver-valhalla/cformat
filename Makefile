OBJ = cformat.o in_out.o buf.o
SOURCE = in_out.c cformat.c

cformat: $(OBJ)
	gcc -o cformat $(OBJ)
in_out.o: in_out.c in_out.h
	gcc -g -c in_out.c
buf.o: buf.c buf.h
	gcc -g -c buf.c
cformat.o: cformat.c 
	gcc -g -c cformat.c
clean:
	rm cformat *.o
