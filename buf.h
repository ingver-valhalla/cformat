// buf.h

#ifndef BUF_H
  #define BUF_h

#include <stdlib.h>

typedef struct FileBuf {
	char    * dat;     
	char    * end;     /* position after last character in a buffer */
	char    * head;    /* current position in a buffer */
	size_t    size;
} FileBuf;

/* new_fbuf: returns initialized FileBuf */
FileBuf new_fbuf();

/* alloc_fbuf: allocates size bytes of memory for a file buffer */
int alloc_fbuf( FileBuf * buf, size_t size );

/* free_fbuf: frees memory allocated for buffer */
int free_fbuf( FileBuf * buf );

/* buf_to_file: writes FileBuf to file */
int buf_to_file( FILE * fp, const FileBuf * buf );

/* read_to_buf: reads size bytes from file to buffer.
 * Buffer is supposed to have allocated memory  */
int read_to_buf( FILE * fp, FileBuf * buf );

#endif // BUF_H
