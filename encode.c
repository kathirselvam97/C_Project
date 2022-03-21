#include <stdio.h>
#include <string.h>
#include "encode.h"   //Declare all the required header files 
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

/* Read names from user and validate
 * Input: command line args
 * Output: e-success or e_failure
 * Description:  1) Validate command line arguments
 *               2) Check args[2] is .bmp and store the name
 *               3) Check argv[3] is .txt and store that in a variable
 *               4) Check argv[4] is entered or not
 *               5) If entered then store the file name in a variable
 *               6) Else create the file and store that in a variable
 *               5) Return e_success or failure
 */

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
	if ( strcmp(strstr(argv[2], "."), ".bmp") == 0 )
		encInfo -> src_image_fname = argv[2];
    else
		return e_failure;

	if ( strcmp(strstr(argv[3], "."), ".txt") == 0 )
		encInfo -> secret_fname = argv[3];
    else
		return e_failure;

    if(argv[4] != NULL )
		encInfo -> stego_image_fname = argv[4];
    else
		encInfo -> stego_image_fname = "steno.bmp";
	
	return e_success;
}

/* Check capacity
 * Output: e-success or e_failure
 * Description:  1) Call the function and get the size of source image
 *               2) Call the function and get the secret file size
 *               3) Check image capacity size is greater then the sum of secret file size and other bytes 
 *               4) Return e_success or failure
 */

Status check_capacity(EncodeInfo *encInfo)
{
	encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);
	encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);
	if(encInfo -> image_capacity > ((2 + 4 + 4 + 4 + encInfo -> size_secret_file)*8))
		return e_success;
	else
		return e_failure;
}

/* Get secret file size
 * Output: Size of secret file
 * Description:  1) Declare the function pointer
 *               2) Make the pointer points to end of file
 *               3) Now use ftell and find the size of file 
 */

uint get_file_size(FILE *fptr)
{
	fseek(fptr, 0, SEEK_END);
	return ftell(fptr);
}

/* Copy_Header
 * Input: Source image
 * Output: e-success or e_failure
 * Description:  1) Make the pointer points to 0th position in source image
 *               2) Declare the char array of size 32
 *               3) Read 54 bytes from source image and store that in char array
 *               4) Wite that 54 bytes from source image in destiation image
 *               4) Return e_success or failure
 */

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
	fseek(fptr_src_image, 0, SEEK_SET);
	char str[54]; 
	fread ( str, 54, 1, fptr_src_image);
	fwrite ( str, 54, 1, fptr_dest_image);
	return e_success;
}

/* Encode magic string
 * Input: Source image, stego image, magic string and its length
 * Output: e-success or e_failure
 * Description:  1) Call the function with inputs
 *               2) Return e_success or failure
 */

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
	encode_data_to_image(magic_string, strlen(magic_string), encInfo -> fptr_src_image, encInfo -> fptr_stego_image, encInfo);
	return e_success;
}

/* Encode secret file extension
 * Input: Source image, stego image, file extension and its length
 * Output: e-success or e_failure
 * Description:  1) Call the function with inputs
 *               2) Return e_success or failure
 */

Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
	encode_data_to_image(file_extn, strlen(file_extn), encInfo -> fptr_src_image, encInfo -> fptr_stego_image, encInfo);
	return e_success;
}

/* Encode secret file data
 * Input: secret file and its length
 * Output: e-success or e_failure
 * Description:  1) Declare char array
 *               2) Make the pointer points to 0th position
 *               3) Read bytes from secret file of secret file size and store that in char array
 *               4) Call the function to encode data
 *               4) Return e_success or failure
 */

Status encode_secret_file_data(EncodeInfo *encInfo)
{
	char str[encInfo -> size_secret_file];
	fseek(encInfo -> fptr_secret, 0, SEEK_SET);
	fread(str, encInfo -> size_secret_file, 1, encInfo -> fptr_secret);
	encode_data_to_image(str, strlen(str), encInfo -> fptr_src_image, encInfo -> fptr_stego_image, encInfo);
	return e_success;
}

/* Encode data to image
 * Input: source image, stego image, magic string and length
 * Description:  1) Read 8 bytes from source image
 *               2) Call the function and encode the data
 *               3) And write that encoded char into stego image
 *               4) Repeat that for length of magic string times
 */

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image, EncodeInfo *encInfo)
{
	int i;
	for(i = 0; i<size; i++)
	{
		fread(encInfo -> image_data, 8, 1, fptr_src_image);
	    encode_byte_to_lsb(data[i], encInfo -> image_data);
		fwrite(encInfo -> image_data, 8, 1, fptr_stego_image);
	}
}

/* Encode byte to lsb
 * Input: Recieves char from other function
 * Description:  1) A byte consists of 8 bit so set loop for 8
 *               2) Right shift the char and add with 1 to get msb
 *               3) Clear the lsb by adding it with 0xFE
 *               4) Store that bits into image buffer
 */

Status encode_byte_to_lsb ( char data, char *image_buffer)
{
	int i;
	for (i = 0; i < 8; i++)
	{                         //clear lsb                 //get msb
		image_buffer[i] = (image_buffer[i] & 0xFE ) | ((data >> (7 - i)) & 1);
	}
}

/* Encode extension size
 * Input: Size of extension
 * Output: e-success or e_failure
 * Description:  1) Declare the char array of size 32
 *               2) Read 32 bytes from src image and store it in char array
 *               3) Call the function and encode the data
 *               4) Write the encoded data into destination image
 *               4) Return e_success or failure
 */

Status encode_extn_size(int size, EncodeInfo *encInfo)
{
    char str[32];
	fread(str, 32, 1, encInfo -> fptr_src_image);
	encode_size_to_lsb(size, str);
	fwrite(str, 32, 1, encInfo -> fptr_stego_image);
	return e_success;
}

/* Encode secret file size
 * Input: secret file size
 * Output: e-success or e_failure
 * Description:  1) Declare char array
 *               2) Read 32 bytes from src image and store it in array
 *               3) Call the function and encode the data
 *               4) Write the encoded data into destination image
 *               4) Return e_success or failure
 */

Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)
{
    char str[32];
	fread(str, 32, 1, encInfo -> fptr_src_image);
	encode_size_to_lsb(file_size, str);
	fwrite(str, 32, 1, encInfo -> fptr_stego_image);
	return e_success;
}

/* Encode size to lsb
 * Input: size of secret file or size of extension
 * Description:  1) Get a loop for 32 bcz of given size
 *               2) Right shift the char and add with 1 to get msb
 *               3) Clear the lsb by adding it with 0xFE
 *               4) Store that bits into image buffer
 */

Status encode_size_to_lsb (int size, char *image_buffer)
{
	int i;
	for (i = 0; i < 32; i++)
	{                         //clear lsb                 //get msb
		image_buffer[i] = (image_buffer[i] & 0xFE) | ((size >> (31 - i)) & 1);
	}
}

/* Copy remaining from image data
 * Input: source and destination image
 * Output: Return e_success or e_failure
 * Description:  1) Declare the character
 *               2) Read the remaining character from src image till null
 *               3) Write that character into destination image
 *               4) Return e_success
 */

Status copy_remaining_img_data(FILE *fptr_src_image, FILE *fptr_dest_image)
{
	char ch;
	while(fread(&ch, 1, 1, fptr_src_image) > 0)
	{
		fwrite(&ch, 1, 1, fptr_dest_image);
	}
	return e_success;
}

/* Do encoding
 * Output: Return e_success or e_failure
 * Description:  1) Call the functions one by one
 *               2) Get the return value from that functions and display the results accordingly
 *               3) Return e_success or e_failure
 */
Status do_encoding(EncodeInfo *encInfo)
{
	if( open_files(encInfo) == e_success )
    {
		printf("Opening file is success\n");
		if(check_capacity(encInfo) == e_success)
		{
			printf("Check capacity is a success\n");
			if(copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
			{
				printf("Header is copied to destination image successfully\n");
				if(encode_magic_string(MAGIC_STRING, encInfo) == e_success)
				{
					printf("Encoded magic string successfully\n");
					strcpy(encInfo -> extn_secret_file, strstr(encInfo -> secret_fname, "."));
					if(encode_extn_size(strlen(encInfo -> extn_secret_file), encInfo) == e_success)
					{
						printf("Encoded extension size Successfully\n");
						if(encode_secret_file_extn(encInfo -> extn_secret_file, encInfo) == e_success)
						{
							printf("Encoded secret file extension successfully\n");
							if(encode_secret_file_size(encInfo -> size_secret_file, encInfo) == e_success)
							{
								printf("Encoded secret file size successfully\n");
								if(encode_secret_file_data(encInfo) == e_success)
								{
									printf("Encoded secret file data successfully\n");
									if(copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
									{
										printf("Copied remaining data successfully\n");
									}
									else
									{
										printf("Failed to copy remaining data\n");
										return e_failure;
									}
								}
								else
								{
									printf("Encoded secret file data failed\n");
								}
							}
							else
							{
									printf("Failed to encode secret file size\n");
									return e_failure;
						    }
						}
						else
						{
							printf("Failed to encode secret file extension\n");
						}
					}
					else
					{
						printf("Failed to encode the extension size\n");
					}
				}
				else
				{
					printf("Failed to Encode magic string\n");
				}

			}
			else
			{
				printf("Copying header to destination image failed\n");
            }
		}
		else
		{
			printf("Check capacity is a failure\n");
			return e_failure;
		}
    }
    else
    {
		printf("Opening file is failure\n");
        return e_failure;
    }
	return e_success;
}
