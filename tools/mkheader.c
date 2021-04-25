
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_INITIAL_SZ 3

int main(int argc, char* argv[]) {
   FILE* header_file = NULL;
   char* header_contents = NULL,
      * header_contents_new = NULL;
   size_t
      read_sz = 0,
      total_sz = 0,
      copied_sz = 0,
      header_contents_sz = 0,
      buffer_sz = BUFFER_INITIAL_SZ;
   char buffer[BUFFER_INITIAL_SZ];

   if( 4 != argc ) {
      printf( "%s [filename] [array_name] [array_sz]", argv[0] );
      return 1;
   }

   header_contents_sz = BUFFER_INITIAL_SZ;
   header_contents = calloc( BUFFER_INITIAL_SZ + 1, 1 );

   while( 0 < (read_sz = read( 0, buffer, buffer_sz )) ) {
      if( header_contents_sz < total_sz + read_sz ) {
         header_contents_new = realloc( 
            header_contents,
            total_sz + read_sz + 1 );
         if( NULL == header_contents_new ) {
            fprintf( stderr, "reallocation failure\n" );
            free( header_contents );
            return 2;
         }
         header_contents = header_contents_new;
         header_contents_sz = total_sz + read_sz;
      }

      strncpy(
         &(header_contents[total_sz]),
         buffer,
         BUFFER_INITIAL_SZ );

      total_sz += read_sz;
   }
   header_contents[total_sz] = '\0';

   header_file = fopen( argv[1], "w" );
   if( NULL == header_file ) {
      fprintf( stderr, "unable to open %s; aborting\n", argv[1] );
   }
   fprintf( header_file, "unsigned char %s[%s] = {\n%s\n};",
      argv[2], argv[3], header_contents );
   fclose( header_file );

   return 0;
}

