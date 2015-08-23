OBJ = cformat.o in_out.o
SOURCE = in_out.c cformat.c

cformat: $(OBJ)
	gcc -o cformat $(OBJ)
in_out.o : in_out.c
	gcc -g -c in_out.c
cformat.o : cformat.c 
	gcc -g -c cformat.c
clean:
	rm cformat *.o
