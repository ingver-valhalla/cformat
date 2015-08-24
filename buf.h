// buf.h

#ifndef BUF_H
  #define BUF_h

#include <stdlib.h>
#include <stdbool.h>

typedef struct FileBuf {
	char    * dat;     
	char    * end;     /* position after last character in a buffer */
	char    * head;    /* current position in a buffer */
	size_t    size;
} FileBuf;

/* new_fbuf: returns initialized FileBuf */
FileBuf new_fbuf();

/* alloc_fbuf: allocates size bytes of memory for a file buffer */
bool alloc_fbuf( FileBuf * buf, size_t size );

/* buf_to_file: writes FileBuf to file */
bool buf_to_file( FILE * fp, const FileBuf * buf );

/* read_to_buf: reads size bytes from file to buffer.
 * Buffer is supposed to have allocated memory  */
bool read_to_buf( FILE * fp, FileBuf * buf );

/* free_fbuf: frees memory allocated for buffer */
bool free_fbuf( FileBuf * buf );

#endif // BUF_H
