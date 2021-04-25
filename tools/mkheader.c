
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SZ 3

int main(int argc, char* argv[]) {
   FILE* header_file = NULL,
      * input_file = NULL;
   char* header_filename = NULL,
      * input_filename = NULL,
      * array_name = NULL,
      * array_sz_txt = NULL;
   size_t read_sz = 0;
   unsigned char buffer[BUFFER_SZ];
   int i = 0;

   if( 3 > argc ) {
      printf( "%s [input_filename] [header_filename] {array_name} {array_sz}", argv[0] );
      return 1;
   }

   for( i = 1 ; argc > i ; i++ ) {
      if( NULL == input_filename ) {
         input_filename = argv[i];
      } else if( NULL == header_filename ) {
         header_filename = argv[i];
      } else if( NULL == array_name ) {
         array_name = argv[i];
      } else if( NULL == array_sz_txt ) {
         array_sz_txt = argv[i];
      }
   }

   /* If no size specified, hard-code it to empty. */
   if( NULL == array_sz_txt ) {
      array_sz_txt = "";
   }

   input_file = fopen( input_filename, "rb" );
   header_file = fopen( header_filename, "w" );

   if( NULL == input_file ) {
      fprintf( stderr, "unable to open %s; aborting\n", input_filename );
   }

   if( NULL == header_file ) {
      fprintf( stderr, "unable to open %s; aborting\n", header_filename );
   }
   
   if( NULL != array_name ) {
      fprintf( header_file, "unsigned char %s[%s] = {\n",
         array_name, array_sz_txt );
   }

   /* Read the input file, converting it into hex. */
   while( 0 < (read_sz = fread( buffer, 1, BUFFER_SZ, input_file )) ) {
      for( i = 0 ; read_sz > i ; i++ ) {
         fprintf( header_file, "0x%X,", buffer[i] );
      }
   }

   if( NULL != array_name ) {
      fprintf( header_file, "};\n" );
   }
   
   fclose( header_file );

   return 0;
}

