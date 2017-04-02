/*

	CSCI 3280, Introduction to Multimedia Systems
	Spring 2015
	
	Assignment 01 Skeleton

	ascii.cpp
	
*/

#include "stdio.h"
#include "malloc.h"
#include "memory.h"
#include "math.h"

#define MAX_SHADES 8

char shades[MAX_SHADES] = {'~', '=', '+', 'A', 'X', '%', '#', '@'};

#define SAFE_FREE(p)  { if(p){ free(p);  (p)=NULL;} }

struct BYTE3
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

BYTE3*	image_bm	= NULL;
BYTE3**	image_pm	= NULL;

int image_width		= 0;		//	use it for obtaining image_width after PPM loading
int image_height	= 0;		//	use it for obtaining image_height after PPM loading

//
//	PPM decoder
//
//	............ DO NOT CHANGE ...............
//
void SwapByte( void *src, int n )
{
	int i,j,n2;
	unsigned char a;
	unsigned char *s0;
	s0 = (unsigned char*)src;

	n2 = n/2;
	for( i=0, j=n-1; i<n2; i++, j-- )
	{
		a = s0[i];
		s0[i] = s0[j];
		s0[j] = a;
	}
}

BYTE3 * LoadPPMImage(const char *spath)
{
	char str[256], info[2];
	int width=0, height=0;
	short mode;
	short mode_swap;


	FILE *f0 = fopen( spath, "rb" );
	if( f0==NULL )
	{
		printf("unable to find ppm file!\n");
		return NULL;
	}

	fread( &mode, sizeof(short), 1, f0 ); 
	mode_swap=mode;
	SwapByte( &mode_swap, sizeof(mode) );
	fread( info, sizeof(char), 2, f0 );

	fclose(f0);

	if( mode=='P5' || mode=='P6' || mode_swap=='P5'|| mode_swap=='P6')
	{
		FILE *f0 = fopen( spath, "rb" );
		if(f0==NULL)
		{
			printf("unable to find ppm file!\n");
			return NULL;
		}

		if( info[0]=='\n' || info[1]=='\n' )
		{
			fgets( str, 256, f0 );
			while( fgets( str, 256, f0 ) && str[0] == '#' );
			sscanf(str, "%i %i", &width, &height);
			fgets( str, 256, f0 );
		}
		else if( info[0]==' ' )
		{
			fgets( str, 256, f0 );
			char tmp[16];
			sscanf( str, "%s %i %i", tmp, &width, &height );
		}
		else if( info[0]=='\r' && info[1]!='\n' )
		{
			fscanf( f0, "%[^\r]", str ); fgetc(f0);
			do{ fscanf( f0, "%[^\r]", str ); fgetc(f0); }while( str[0] == '#' );
			sscanf(str, "%i %i", &width, &height);
			fscanf( f0, "%[^\r]", str ); fgetc(f0);
		}
		else
		{
			printf("format error!\n");
			return NULL;
		}

		SAFE_FREE( image_bm );
		SAFE_FREE( image_pm );

		image_bm = (BYTE3*) malloc( width * height * sizeof(BYTE3) );
		if(image_bm==NULL)
		{
			printf("unable to alloc memory!\n");
			return NULL;
		}

		memset( image_bm, 0, width*height*sizeof(BYTE3) );

		image_pm = (BYTE3**) malloc( height * sizeof(BYTE3*) );
		if(image_pm==NULL)
		{
			printf("unable to alloc memory!\n");
			return false;
		}

		for(int j=0; j<height; j++ )
			image_pm[j] = &image_bm[j*width];

		image_width = width;
		image_height = height;

		fread( image_bm, sizeof(BYTE3), width*height, f0 );

		fclose(f0);
	}

	return image_bm;
}

//
//	************ END OF 'DO NOT CHANGE' *************
//

char ChangeAscii(int input){
	if(input < 32){
		return shades[0];
	} else if (input < 64) {
		return shades[1];
	} else if (input < 96) {
		return shades[2];
	} else if (input < 128) {
		return shades[3];
	} else if (input < 160) {
		return shades[4];
	} else if (input < 192) {
		return shades[5];
	} else if (input < 224) {
		return shades[6];
	} else if (input < 256) {
		return shades[7];
	}
}

//
//	***** PUT ALL YOUR CODE INSIDE main() *****
//
int main( int argc, char** argv)
{

	//	Read in image data
	//
	BYTE3 *image_data = LoadPPMImage( argv[2] );

	if ( !image_data ) {
		printf("unable to load source.ppm!\n");
		return -1;
	}
	
	bool	onPaper;
	if ( argv[1][0] == 'p' || argv[1][0] == 's'){
	} else {
	return 0;
	}

	//
	//	Your code goes here ....
	//

	int i,j;
	int temp;
	int *greyimage;
	char buffer[100];
	int n;
	FILE* fPtr;
	fPtr = fopen("output.html", "wb+");
    if (!fPtr) {
        printf("error.\n");
        return 0;
    }

	greyimage = (int *) malloc(sizeof(char)*image_width*image_height);

	for(i=0;i<image_width;i++){
		for(j=0;j<image_height;j++){
		greyimage[j+i*image_width] = image_bm[j+i*image_width].r*0.299+image_bm[j+i*image_width].g*0.587+image_bm[j+i*image_width].b*0.114;
		if(argv[1][0] == 's'){
		n = sprintf(buffer,"<font color=\"#%02x%02x%02x\">%c</font>",image_bm[j+i*image_width].r,image_bm[j+i*image_width].g,image_bm[j+i*image_width].b,ChangeAscii(greyimage[j+i*image_width]));
		fwrite(&buffer, n, 1, fPtr);
	//	printf("<font color=\"#%02x%02x%02x\">%c</font>",image_bm[j+i*image_width].r,image_bm[j+i*image_width].g,image_bm[j+i*image_width].b,ChangeAscii(greyimage[j+i*image_width]));
	//	printf("%c",ChangeAscii(greyimage[j+i*image_width])); // 255- = 's'
		} else {
		n = sprintf(buffer,"<font color=\"#%02x%02x%02x\">%c</font>",image_bm[j+i*image_width].r,image_bm[j+i*image_width].g,image_bm[j+i*image_width].b,ChangeAscii(255-greyimage[j+i*image_width]));
		fwrite(&buffer, n, 1, fPtr);
		//	printf("%c",ChangeAscii(255-greyimage[j+i*image_width]));
		}
		}
		n = sprintf(buffer,"<br />\n");
	//	printf("<br />\n");
		fwrite(&buffer, n, 1, fPtr);
	}
	return 0;

} 
