#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

#define CODE_SIZE  12
#define TRUE 1
#define FALSE 0

#define MAX_NODE 4095
#define MAX_CHAR 256
#define EMPTY 0x0000

/*
* CSCI3280 Introduction toMultimedia Systems
*
* --- Declaration ---
*
* I declare that the assignment here submitted is original except for source
* material explicitly acknowledged. I also acknowledge that I am aware of
* University policy and regulations on honesty in academic work, and of the
* disciplinary guidelines and procedures applicable to breaches of such policy
* and regulations, as contained in the website
* http://www.cuhk.edu.hk/policy/academichonesty/
*
* Assignment 2
* Name : Ng Ting Yuk
* Student ID : 1155032539
* Email Addr : tingyukn@gmail.com
*/

/* function prototypes */
unsigned int read_code(FILE*, unsigned int); 
void write_code(FILE*, unsigned int, unsigned int); 
void writefileheader(FILE *,char**,int);
void readfileheader(FILE *,char**,int *);
void compress(FILE *lzw_file, char **input, int fileCount);
void decompress(FILE *lzw_file, char *output, int fileCount);
void AddNode(unsigned short currentNode,unsigned char InChar);
unsigned char WriteDict(FILE *out, unsigned short cW);

unsigned short dictIndex;
unsigned short dictionary[MAX_NODE][MAX_CHAR + 1];
unsigned char Map[MAX_NODE - MAX_CHAR];
unsigned char StringBuf[MAX_NODE - MAX_CHAR];

int main(int argc, char **argv)
{
    int printusage = 0;
    int	no_of_file;
    char **input_file_names;    
	char *output_file_names;
    FILE *lzw_file;

    if (argc >= 3)
    {
		if ( strcmp(argv[1],"-c") == 0)
		{		
			/* compression */
			lzw_file = fopen(argv[2] ,"wb");
        
			/* write the file header */
			input_file_names = argv + 3;
			no_of_file = argc - 3;
			writefileheader(lzw_file,input_file_names,no_of_file);
        	        	
			/* ADD CODES HERE */
			dictIndex = MAX_CHAR;
			memset(dictionary, EMPTY, sizeof(unsigned short) * (MAX_NODE) * (MAX_CHAR + 1));        	
			compress(lzw_file, input_file_names, no_of_file);

			fclose(lzw_file);        	
		} else
		if ( strcmp(argv[1],"-d") == 0)
		{	
			/* decompress */
			lzw_file = fopen(argv[2] ,"rb");
			
			/* read the file header */
			no_of_file = 0;			
			readfileheader(lzw_file,&output_file_names,&no_of_file);
			
			/* ADD CODES HERE */
			dictIndex = MAX_CHAR;
			memset(dictionary, EMPTY, sizeof(unsigned short) * (MAX_NODE) * (MAX_CHAR + 1));  
			decompress(lzw_file, output_file_names, no_of_file);
			
			fclose(lzw_file);		
			free(output_file_names);
		}else
			printusage = 1;
    }else
		printusage = 1;

	if (printusage)
		printf("Usage: %s -<c/d> <lzw filename> <list of files>\n",argv[0]);
 	
	return 0;
}

/*****************************************************************
 *
 * writefileheader() -  write the lzw file header to support multiple files
 *
 ****************************************************************/
void writefileheader(FILE *lzw_file,char** input_file_names,int no_of_files)
{
	int i;
	/* write the file header */
	for ( i = 0 ; i < no_of_files; i++) 
	{
		fprintf(lzw_file,"%s\n",input_file_names[i]);	
			
	}
	fputc('\n',lzw_file);

}

/*****************************************************************
 *
 * readfileheader() - read the fileheader from the lzw file
 *
 ****************************************************************/
void readfileheader(FILE *lzw_file,char** output_filenames,int * no_of_files)
{
	int noofchar;
	char c,lastc;

	noofchar = 0;
	lastc = 0;
	*no_of_files=0;
	/* find where is the end of double newline */
	while((c = fgetc(lzw_file)) != EOF)
	{
		noofchar++;
		if (c =='\n')
		{
			if (lastc == c )
				/* found double newline */
				break;
			(*no_of_files)++;
		}
		lastc = c;
	}

	if (c == EOF)
	{
		/* problem .... file may have corrupted*/
		*no_of_files = 0;
		return;
	
	}
	/* allocate memeory for the filenames */
	*output_filenames = (char *) malloc(sizeof(char)*noofchar);
	/* roll back to start */
	fseek(lzw_file,0,SEEK_SET);

	fread((*output_filenames),1,(size_t)noofchar,lzw_file);
	
	return;
}

/*****************************************************************
 *
 * read_code() - reads a specific-size code from the code file
 *
 ****************************************************************/
unsigned int read_code(FILE *input, unsigned int code_size)
{
    unsigned int return_value;
    static int input_bit_count = 0;
    static unsigned long input_bit_buffer = 0L;

    /* The code file is treated as an input bit-stream. Each     */
    /*   character read is stored in input_bit_buffer, which     */
    /*   is 32-bit wide.                                         */

    /* input_bit_count stores the no. of bits left in the buffer */

    while (input_bit_count <= 24) {
        input_bit_buffer |= (unsigned long) getc(input) << (24-input_bit_count);
        input_bit_count += 8;
    }
    
    return_value = input_bit_buffer >> (32 - code_size);
    input_bit_buffer <<= code_size;
    input_bit_count -= code_size;
    
    return(return_value);
}


/*****************************************************************
 *
 * write_code() - write a code (of specific length) to the file 
 *
 ****************************************************************/
void write_code(FILE *output, unsigned int code, unsigned int code_size)
{
    static int output_bit_count = 0;
    static unsigned long output_bit_buffer = 0L;

    /* Each output code is first stored in output_bit_buffer,    */
    /*   which is 32-bit wide. Content in output_bit_buffer is   */
    /*   written to the output file in bytes.                    */

    /* output_bit_count stores the no. of bits left              */    

    output_bit_buffer |= (unsigned long) code << (32-code_size-output_bit_count);
    output_bit_count += code_size;

    while (output_bit_count >= 8) {
        putc(output_bit_buffer >> 24, output);
        output_bit_buffer <<= 8;
        output_bit_count -= 8;
    }


    /* only < 8 bits left in the buffer                          */    

}

/*****************************************************************
 *
 * compress() - compress the source file and output the coded text
 *
 ****************************************************************/
void compress(FILE *lzw_file, char **input, int fileCount)
{

	/* ADD CODES HERE */
	FILE *fp;
	unsigned char getc;
	unsigned short Node;

	int i;
	for (i = 0; i < fileCount; i++) {

		fp = fopen(input[i], "rb");
		if(!fp){
			printf("ERROR: File:%s not exist!\n",input[i]);
			return;
		}

		Node = fread(&getc, 1, 1, fp);
		if (Node == 0) {
			write_code(lzw_file, MAX_NODE, CODE_SIZE);
			continue;
		}

		Node = getc;

		while (fread(&getc, 1, 1, fp) > 0) {
			if (dictionary[Node][getc] == EMPTY) {

				write_code(lzw_file, Node, CODE_SIZE);

				if (dictIndex == MAX_NODE){
					dictIndex = MAX_CHAR;
					memset(dictionary, EMPTY, sizeof(unsigned short) * (MAX_NODE) * (MAX_CHAR + 1));
				}
				else {
					dictionary[Node][getc] = dictIndex;
					dictIndex++;
				}
				Node = getc;

			} else {
				Node = dictionary[Node][getc];
			}
		}

		write_code(lzw_file, Node, CODE_SIZE);
		write_code(lzw_file, MAX_NODE, CODE_SIZE);

		fclose(fp);
	}

}


/*****************************************************************
 *
 * decompress() - decompress a compressed file to the orig. file
 *
 ****************************************************************/
void decompress(FILE *lzw_file, char *output, int fileCount) {
	char *filename;

	FILE *fp;
	unsigned short cW;
	unsigned short currentNode;
	unsigned char InChar;

	filename = strtok(output, "\n");
	while (filename != NULL) {

		fp = fopen(filename, "wb");

		cW = read_code(lzw_file, CODE_SIZE) & 0x0FFF;

		if (cW == MAX_NODE) {
			fclose(fp);
			filename = strtok(NULL, "\n");
			continue;
		}

		WriteDict(fp, cW);

		while (cW != MAX_NODE) {

			currentNode = cW;

			cW = read_code(lzw_file, CODE_SIZE) & 0x0FFF;
			if (cW == MAX_NODE) break;

			if (dictionary[cW][MAX_CHAR] != EMPTY || cW < MAX_CHAR) {

				InChar = WriteDict(fp, cW);

			} else {

				InChar = WriteDict(fp, currentNode);
				WriteDict(fp, InChar);
			}

			if (dictIndex == MAX_NODE) {
				dictIndex = MAX_CHAR;
				memset(dictionary, EMPTY, MAX_NODE * (MAX_CHAR + 1) * sizeof(unsigned short));
				continue;
			}

			AddNode(currentNode,InChar);
		}

		fclose(fp);
		filename = strtok(NULL, "\n");
	}
}

void AddNode(unsigned short currentNode,unsigned char InChar){
	dictionary[currentNode][InChar] = dictIndex;
	dictionary[dictIndex][MAX_CHAR] = currentNode;
	Map[dictIndex - MAX_CHAR] = InChar;
	dictIndex++;
}

unsigned char WriteDict(FILE *fp, unsigned short cW) {

	int strlen = 0;
	unsigned char InChar;

	if (cW < MAX_CHAR) {
		fwrite(&cW, 1, 1, fp);
		return cW;
	}

	while (cW >= MAX_CHAR) {
		StringBuf[strlen] = Map[cW - MAX_CHAR];
		cW = dictionary[cW][MAX_CHAR];
		strlen = strlen + 1;
	}

	InChar = cW;
	StringBuf[strlen] = InChar;

	while (strlen >= 0){
		fwrite(&(StringBuf[strlen]), 1, 1, fp);
		strlen = strlen - 1;
	}

	return InChar;
}
