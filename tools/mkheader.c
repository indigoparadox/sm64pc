
#include <stdio.h>
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
   char brace_open_char = '{',
      brace_close_char = '}';
   size_t read_sz = 0;
   unsigned char buffer[BUFFER_SZ];
   int i = 0,
      start_copy_at = 0, /* 1 if the input_filename starts with a slash. */
      external_mode = 0, /* Write input_filename instead of its contents. */
      force_hex_output = 0; /* Write input filename as hex. Adds a 0x00 terminator. */

   for( i = 1 ; argc > i ; i++ ) {
      if( 0 == strncmp( argv[i], "-e", 2 ) ) {
         external_mode = 1;
         brace_open_char = '"';
         brace_close_char = '"';
      } else if( 0 == strncmp( argv[i], "-x", 2 ) ) {
         force_hex_output = 1;
         brace_open_char = '{';
         brace_close_char = '}';
      } else if( NULL == input_filename ) {
         input_filename = argv[i];
      } else if( NULL == header_filename ) {
         header_filename = argv[i];
      } else if( NULL == array_name ) {
         array_name = argv[i];
      } else if( NULL == array_sz_txt ) {
         array_sz_txt = argv[i];
      }
   }

   if( NULL == header_filename ) {
      printf( "%s [input_filename] [header_filename] {array_name} {array_sz}", argv[0] );
      return 1;
   }

   /* If no size specified, hard-code it to empty. */
   if( NULL == array_sz_txt ) {
      array_sz_txt = "";
   }

   if( !external_mode ) {
      input_file = fopen( input_filename, "rb" );

      if( NULL == input_file ) {
         fprintf( stderr, "unable to open input %s; aborting\n", input_filename );
         return 2;
      }
   }
   header_file = fopen( header_filename, "w" );

   if( NULL == header_file ) {
      fprintf( stderr, "unable to open output %s; aborting\n", header_filename );
      fclose( input_file );
      return 4;
   }
   
   if( NULL != array_name ) {
      fprintf( header_file, "unsigned char %s[%s] = %c",
         array_name, array_sz_txt, brace_open_char );
      if( '{' == brace_open_char ) {
         fprintf( header_file, "\n" );
      }
   }

   if( external_mode ) {
      if( '/' == input_filename[0] || '\\' == input_filename[0] ) {
         start_copy_at = 1;
      }

      /* Write the input filename. */
      if( force_hex_output ) {
         for( i = start_copy_at ; strlen( input_filename ) > i ; i++ ) {
            fprintf( header_file, "0x%X,", (unsigned char)(input_filename[i]) );
         }
         fprintf( header_file, "0x0," );
      } else {
         fprintf( header_file, "%s", &(input_filename[start_copy_at]) );
      }

   } else {

      /* Read the input file, converting it into hex. */
      while( 0 < (read_sz = fread( buffer, 1, BUFFER_SZ, input_file )) ) {
         for( i = 0 ; read_sz > i ; i++ ) {
            fprintf( header_file, "0x%X,", buffer[i] );
         }
      }

   }

   if( NULL != array_name ) {
      fprintf( header_file, "%c;\n", brace_close_char );
   }
   
   fclose( header_file );

   if( !external_mode ) {
      fclose( input_file );
   }

   return 0;
}

