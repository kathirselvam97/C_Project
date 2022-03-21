/*
 Name             : Kathiravan s
 Date             : 12-03-2022
 Project no       : 1
 Project title    : LSB image Stenography
 Sample input     : ./a.out -e beautiful.bmp secret.txt stego.bmp
 Sample output    : Encoded image with contents of secret.txt
 Sample input     : ./a.out -d stego.bmp 
 Sample output    : decoded.txt file with same content of secret.txt
 Sample input     : ./a.out -e beautiful.bmp
 Sample output    : Invalid option!...
                    Use:
                    For encoding: ./a.out -e beautiful.bmp secret.txt stego.bmp
                    For decoding: ./a.out -d stego.bmp decode.txt
*/

#include <stdio.h>
#include "encode.h"
#include "types.h"    //Declare all the required header files
#include "decode.h"
#include <string.h>

/* Main function 
 * Input: Command line arguments
 * Output: Return success messages or error messages
 * Description:  1) Check for number of arguments if arguments if greater then 2 then
 *               2) Checks it is encode or decode bcz it needs atleast 3 operands to perform operation
 *               3) Call the function and check argv[1] is -e or -d
 *               4) If encode then call function and validate the given arguments are valid
 *               5) If arguments are valid then perform encoding by calling the function
 *               6) If decode then call function and validate the given arguments are valid
 *               5) If arguments are valid then perform decoding by calling the function
 *               8) Based on return value print the messsages
 */
int main(int argc, char **argv)
{
	if (argc >= 3)
	{
		if (check_operation_type(argv)  == e_encode)
	    {
			printf("You have Selected encoding\n");
			EncodeInfo encInfo;
		    if( read_and_validate_encode_args(argv, &encInfo) == e_success)
			{
				printf("Read and validate encode arguments is a success\n");
			    printf("...Encoding started...\n");
				if ( do_encoding(&encInfo) == e_success )
			    {
					printf("...Encoding completed...\n");
				}
			    else
			    {
					printf("...Encoding failed...\n");
					return -1;
                }
			}
			else
			{
				printf("Read and validate encode arguments is a failure\n");
			    return -1;
		    }	
		}
		else if (check_operation_type(argv) == e_decode)
	    {
			printf("You have Selected decoding\n");
			decodeInfo dncInfo;
		    if( read_and_validate_decode_args(argv, &dncInfo) == e_success)
			{
				printf("Read and validate decode arguments is a success\n");
			    printf("...Decoding started...\n");
				if ( do_decoding(&dncInfo) == e_success )
			    {
					printf("...Decoding completed...\n");
				}
			    else
			    {
					printf("...Decoding failed...\n");
					return -1;
                }
			}
			else
			{
				printf("Read and validate decode arguments is a failure\n");
			    return -1;
		    }	
	    }
		else
	    {
			printf("Invalid option!...\nUse:\nFor encoding: ./a.out -e beautiful.bmp secret.txt stego.bmp\nFor decoding: ./a.out -d stego.bmp decode.txt\n");
		    return -1;
	    }
	}
	else
	{	
		printf("Invalid number of args!...\nUse:\nFor encoding: ./a.out -e beautiful.bmp secret.txt stego.bmp\nFor decoding: ./a.out -d stego.bmp decode.txt\n");
		return -1;
	}
	return 0;
}

OperationType check_operation_type(char *argv[])
{
	if( strcmp(argv[1], "-e") == 0 )
		return e_encode;
	else if( strcmp(argv[1], "-d") == 0 )
		return e_decode;
	else
		return e_unsupported;
}
