// buf.c 

#include <stdio.h>
#include "buf.h"


FileBuf new_fbuf()
{
	FileBuf buf;
	buf.dat     = NULL;
	buf.end     = NULL;
	buf.head    = NULL;
	buf.size    = 0;
    
	return buf;
}

int alloc_fbuf( FileBuf * buf, size_t size )
{
	if( !buf ) return 0;

	buf->dat = (char *) calloc( size + 1, 1 ); /* +1 for EOF */
	if( !buf->dat ) {
		fprintf( stderr, "Can't allocate %lu bytes for file buffer\n",
		         size + 1 );
		return 0;
	}
	buf->end = buf->dat + size;
	buf->dat[size] = EOF; /* buf->end points to EOF */
	buf->head = buf->dat;
	buf->size = size;
	return 1;
}	

int free_fbuf( FileBuf * buf )
{
	if( !buf ) return 0;

	if( buf->dat ) {
		free( buf->dat );
		buf->dat = NULL;
	}
	buf->end = NULL;
	buf->head = NULL;
	buf->size = 0;
	return 1;
	
}

int buf_to_file( FILE * fp, const FileBuf * buf )
{
	if( !fp || !buf ) {
		return 0;
	}
	if( buf->size != fwrite( buf->dat, 1, buf->size, fp ) ) {
		return 0;
	}
	return 1;
}

int read_to_buf( FILE * fp, FileBuf * buf )
{
	if( !fp || !buf || !buf->dat || !buf->size ) {
		return 0;
	}
	if( buf->size != fread( buf->dat, 1, buf->size, fp ) ) {
		return 0;
	}
	return 1;
}
