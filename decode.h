#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types
#include "common.h"

/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _decodeInfo
{
    char *stego_image_name;
    char *output_file_name;
    FILE *fptr_addr_stego;
    FILE *fptr_output_file;
    FILE *fptr_stego;
    int extn_size;
	int file_size;
    char image_data[MAX_IMAGE_BUF_SIZE];
    char decode_data[MAX_SECRET_BUF_SIZE];
    char extn_output_file[MAX_FILE_SUFFIX];
}decodeInfo;


/* Decoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], decodeInfo *dncInfo);

/* Perform the decoding */
Status do_decoding(decodeInfo *dncInfo);

/* Store Magic String */
Status decode_magic_string(const char *magic_string, decodeInfo *dncInfo); 

/*Decode extn size*/
Status decode_extn_size(decodeInfo *dncInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(int extn_size, decodeInfo *dncInfo);

/* Decode secret file size */
Status decode_secret_file_size(decodeInfo *dncInfo);

/* Decode secret file data*/
Status decode_secret_file_data(int file_size, decodeInfo *dncInfo);

/* Decode function, which does the real decoding */
Status decode_data_from_image(char *data, int size, FILE *fptr_src_image, decodeInfo *dncInfo);

/* Decode a byte of LSB of image data array */
Status decode_byte_from_lsb(char *data, char *image_buffer);

#endif

