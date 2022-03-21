#include <stdio.h>
#include <string.h>
#include "decode.h"   //Include required header files
#include "types.h"
#include "common.h"

/* Read names from user and validate
 * Input: Stego image name, output file name from cmd line args
 * Output: e-success or e_failure
 * Description:  1) Validate arguments
 *               2) Store the argv[2] as name
 *               3) If argv[3] is given store that in a variable
 *               4) Check the output be able to read or not
 *               5) Return e_success or failure
 */

Status read_and_validate_decode_args(char *argv[], decodeInfo *dncInfo)
{
	if ( strcmp(strstr(argv[2], "."), ".bmp") == 0 )
	{
		dncInfo -> stego_image_name = argv[2];
	    dncInfo -> fptr_addr_stego = fopen(dncInfo -> stego_image_name, "r");
        if ((dncInfo->fptr_addr_stego) == NULL)
        {
			printf("Unable to open stego file\n");
            return e_failure;
	    }
	}
    else
		return e_failure;
    if(argv[3] != NULL )
	{
		dncInfo -> output_file_name = argv[3];
		dncInfo -> fptr_output_file = fopen((dncInfo -> output_file_name), "w");
		if((dncInfo -> fptr_output_file) == NULL)
	    {
			printf("Unable to open output file to store data\n");
		    return e_failure;
	    }
	}
	else
		dncInfo -> output_file_name = NULL;
	return e_success;
}

/* Decoding magic string
 * Input: MAGIC_STRING
 * Output: e-success or e_failure
 * Description:  1) Open the stego file in read mode
 *               2) Set the position of cursor to 54th position
 *               3) Call the decode function perform decoding and check it is e_success
 *               4) Return e_success or failure
 */
Status decode_magic_string(const char *magic_string, decodeInfo *dncInfo)
{
	
	dncInfo -> fptr_addr_stego = fopen(dncInfo -> stego_image_name, "r");
    if ((dncInfo->fptr_addr_stego) == NULL)
    {
    	printf("Unable to open stego file\n");
        return e_failure;
	}
	fseek((dncInfo -> fptr_addr_stego), 0, SEEK_SET); 
	fseek((dncInfo -> fptr_addr_stego), 54, SEEK_SET);
	if(decode_data_from_image(magic_string, strlen(magic_string), dncInfo -> fptr_addr_stego, dncInfo) == e_success)
		return e_success;
	else
		return e_failure;
}

/* Decoding and validating magic string  
 * Input: MAGIC_STRING, strlen(MAGIC_STRING)
 * Output: e-success or e_failure
 * Description:  1) Open the stego image read 8 bytes
 *               2) call the function and decode the image 
 *               3) Check the character returned is equal to the magic string
 *               4) If string is equal return e_success else e_ failure
 */
Status decode_data_from_image(char *data, int size, FILE *fptr_addr_stego, decodeInfo *dncInfo)
{
	int i;		
	for(i = 0; i<size; i++)
	{
		fread(dncInfo -> image_data, 8, 1, dncInfo -> fptr_addr_stego);
	    if(decode_byte_from_lsb(dncInfo -> decode_data, dncInfo -> image_data) == e_success)
		{
			if(dncInfo -> decode_data[0] == data[i])
			{
				continue;
			}
			else
			{
				printf("Magic string not matched\n");
			    return e_failure;
			}
		}
		else
		{
			printf("Decoding byte from lsb is a failure\n");
			return e_failure;
		}
	}
	return e_success;
}

/* Finding file extension size  
 * Input: stego file
 * Output: Return file size, e-success or e_failure
 * Description:  1) Open the stego image read 32 bytes
 *               2) Initialize size variable to zero
 *               3) Use loop and decode that 32 byte of data and store it in variable
 *               4) Display the size of extension and return e_success or e_ failure
 */
Status decode_extn_size(decodeInfo *dncInfo)
{
    char str[32];
	fread(str, 32, 1, dncInfo -> fptr_addr_stego);
	if (dncInfo->fptr_addr_stego == NULL)
    {
		printf("Error: While reading the extn size from stego image file\n");
		return e_failure;
	}
	dncInfo -> extn_size = 0;
	for( int i = 0; i < 32; i++ )
	{
		dncInfo -> extn_size <<= 1;
		dncInfo -> extn_size |= str[i] & 0x01;
	}
	printf("Extension size: %d\n", dncInfo -> extn_size);
	return e_success;
}

/* Decoding file extension  
 * Input: stego file, file extension size
 * Output: Return e-success or e_failure
 * Description:  1) Open the stego image read 8 bytes
 *               2) call the function 
 *               3) Use loop and decode that 8 byte of data and store it in variable
 *               4) Repeat it for file extension size times
 *               4) Display file extension and return e_success or e_ failure
 */
Status decode_secret_file_extn(int extn_size, decodeInfo *dncInfo)
{
	for( int i = 0; i < extn_size; i++)
	{
		fread( dncInfo -> image_data, 8, 1, dncInfo -> fptr_addr_stego);
		if ( dncInfo -> fptr_addr_stego == NULL )
		{
			printf("Fetching secret file extension failed\n");
			return e_failure;
		}
		if ( decode_byte_from_lsb( dncInfo -> decode_data, dncInfo -> image_data) == e_success)
		{
			dncInfo -> extn_output_file[i] = dncInfo -> decode_data[0];
		}
	}
		printf("Extension of file is: %s\n", dncInfo -> extn_output_file);
	return e_success;
}

/* Creating output file  
 * Input: sturture member
 * Output: Return e-success or e_failure
 * Description:  1) Check if output file name has value or not
 *               2) If no then check extension is .txt
 *               3) Create the output file as decode.txt 
 *               4) If extension not matched then display error
 *               5) Open the file in write mode
 *               6) If output file name has value then just return e_success
 *               7) Return e_success or e_ failure
 */
Status create_output_file(decodeInfo *dncInfo)
{
	if(dncInfo -> output_file_name ==NULL)
	{
		if (strncmp(dncInfo->extn_output_file, ".txt", 4) == 0)
		{
			dncInfo->output_file_name = "decode.txt";
			printf("Output file not mentioned so %s is created as default output file\n", "decode.txt");
		}
		else
		{
			printf("Pass the output file in .txt format\n");
			return e_failure;
		}
		dncInfo -> fptr_output_file = fopen((dncInfo -> output_file_name), "w");
		if((dncInfo -> fptr_output_file) == NULL)
	    {
			printf("Unable to open output file to store data\n");
		    return e_failure;
	    }
	}
	else
	{
		printf("The output file is taken from  arguments\n");
	}
	return e_success;
}

/* Decoding secret file size  
 * Input: stego file
 * Output: Return e-success or e_failure
 * Description:  1) Open the stego image read 32 bytes
 *               2) Initialize file size variable to 0 
 *               3) Use loop and decode that 32 byte of data and store it in variable
 *               4) Display size of file and return e_success or e_ failure
 */
Status decode_secret_file_size(decodeInfo *dncInfo)
{
    char str[32];
	fread(str, 32, 1, dncInfo -> fptr_addr_stego);
	if((dncInfo -> fptr_addr_stego) == NULL)
	{
		printf("failed to open file to find size\n");
		return e_failure;
	}
	dncInfo -> file_size = 0;
	for( int i = 0; i< 32; i++)
	{
		dncInfo -> file_size <<= 1;
		dncInfo -> file_size |= str[i] & 0x01;
	}
	printf("Secret file size: %d\n", dncInfo -> file_size);
	return e_success;
}

/* Decoding secret file data 
 * Input: stego file, file size
 * Output: Return e-success or e_failure
 * Description:  1) Open the stego image read 8 bytes
 *               2) Call the function 
 *               3) Decode that 32 byte of data and store it in variable
 *               4) Repeat step for file size times and decode all data
 *               4) Return e_success or e_failure
 */
Status decode_secret_file_data(int file_size, decodeInfo *dncInfo)
{
	int i;
	for ( i = 0; i < (dncInfo -> file_size); i++ )
	{
		fread(dncInfo -> image_data, 8, 1, dncInfo -> fptr_addr_stego);
		if((dncInfo -> fptr_addr_stego) == NULL)
		{
			printf("Fetching data from file failed\n");
            return e_failure;
		}
		
		if(decode_byte_from_lsb(dncInfo -> decode_data, dncInfo -> image_data) == e_success)
		{
			fwrite(dncInfo -> decode_data, 1, 1, dncInfo -> fptr_output_file);
		}
	}
	return e_success;
}

/* Decoding secret file data 
 * Input: stego file, file size
 * Output: Return e-success or e_failure
 * Description:  1) Open the stego image read 8 bytes
 *               2) Call the function 
 *               3) Decode that 32 byte of data and store it in variable
 *               4) Repeat step for file size times and decode all data
 *               4) Return e_success or e_failure
 */
Status decode_byte_from_lsb(char *decode_data, char *image_buffer)
{
	int i;
	decode_data[0] = 0;
	for (i = 0; i <  MAX_IMAGE_BUF_SIZE; i++)
	{  
		decode_data[0] <<= 1;
        decode_data[0] |= (image_buffer[i] & 0x01);
	}
	return e_success;
}

/* Decoding Fucntions
 * Output: Return e-success or e_failure
 * Description:  1) Call the functions one by one
 *               2) Get the value from functions and check it 
 *               3) Return that to the test_encode file
 */
Status do_decoding(decodeInfo *dncInfo)
{
	if(decode_magic_string(MAGIC_STRING, dncInfo) == e_success)
	{
		printf("Decoded magic string successfully\n");
		if(decode_extn_size(dncInfo) == e_success)
		{
			printf("Decoded extension size Successfully\n");
			if(decode_secret_file_extn(dncInfo -> extn_size, dncInfo) == e_success)
			{
				printf("Decoded secret file extension successfully\n");
				if(create_output_file(dncInfo) == e_success)
				{
					printf("Output file created successfully\n");
				if(decode_secret_file_size(dncInfo) == e_success)
				{
					printf("Decoded secret file size successfully\n");
					if(decode_secret_file_data(dncInfo -> file_size, dncInfo) == e_success)
					{
						printf("Decoded secret file data successfully\n");
					}
					else
					{
						printf("Decoding secret file data failed\n");
		     		}
				}
				else
				{
					printf("Failed to decode secret file size\n");
				}
			}
				else
				{
					printf("Failed to create output file\n");
				}
			}

			else
			{
				printf("Failed to decode secret file extension\n");
			}
		}
		else
	    {
			printf("Failed to decode the extension size\n");
		}
	}
	else
	{
		printf("Failed to decode magic string\n");
	}
	return e_success;
}
