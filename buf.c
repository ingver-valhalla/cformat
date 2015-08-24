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

bool alloc_fbuf( FileBuf * buf, size_t size )
{
	if( !buf ) return false;

	buf->dat = (char *) calloc( size + 1, 1 ); /* +1 for EOF */
	if( !buf->dat ) {
		fprintf( stderr, "Can't allocate %lu bytes for file buffer\n",
		         size + 1 );
		return false;
	}
	buf->end = buf->dat + size;
	buf->dat[size] = EOF; /* buf->end points to EOF */
	buf->head = buf->dat;
	buf->size = size;
	return true;
}	

bool buf_to_file( FILE * fp, const FileBuf * buf )
{
	if( !fp || !buf ) {
		return false;
	}
	if( buf->size != fwrite( buf->dat, 1, buf->size, fp ) ) {
		return false;
	}
	return true;
}

bool read_to_buf( FILE * fp, FileBuf * buf )
{
	if( !fp || !buf || !buf->dat || !buf->size ) {
		return false;
	}
	if( buf->size != fread( buf->dat, 1, buf->size, fp ) ) {
		return false;
	}
	return true;
}

bool free_fbuf( FileBuf * buf)
{
	free( buf->dat );
	buf->dat = NULL;
	buf->end = NULL;
	buf->head = NULL;
	buf->size = 0;
}
