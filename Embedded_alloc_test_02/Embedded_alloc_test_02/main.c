/*
*	Embedded_alloc_test_02 - Test the functons allocating some variables
*
*	Created by Haroldo Luiz Moretti do Amaral, 19/09/2015
*	agaelema@globo.com
*	Released into the public domain
*/

#include <stdio.h>
#include <string.h>

#include "embedded_alloc.h"


#define		MEM_SIZE	255					// define the buffer size

#define		SMART_FREE							// control the test - uncomment just one
//#define		SIMPLE_FREE						// control the test - uncomment just one

/* array of chars (uint8_t) to handle the allocated variables */
unsigned char mem_buffer[MEM_SIZE];


void main(int argc, char **argv)
{
	/* test variables */
	char * test;
	long long * test2;
	char * test3;
	short * test4;
	short * test5;
	int * test6;

	/* variables to control the free process */
	unsigned char v1 = 0, v2 = 0, v3 = 0, v4 = 0, v5 = 0, v6 = 0;

	unsigned char *ptr_mem_buffer;
	ptr_mem_buffer = mem_buffer;

	/* initializes the buffer for dynamic allocation - sets the first meta_header */
	embedded_alloc_init(ptr_mem_buffer, MEM_SIZE);

	/* sets the buffer limit address */
	set_block_limit(ptr_mem_buffer, MEM_SIZE);


	/* Begins testing */

	// allocating 1 byte
	test = (char *)embedded_alloc(ptr_mem_buffer, 1);
	if (test != NULL)
	{
		*test = 0xAA;
		v1 = 1;
	}

	// allocating 8 bytes
	test2 = (long long *)embedded_alloc(ptr_mem_buffer, sizeof(long long));
	if (test2 != NULL)
	{
		*test2 = 0xBBBBBBBBBBBBBBBB;
		v2 = 1;
	}

	// allocating 100 bytes
	test3 = (int *)embedded_alloc(ptr_mem_buffer, 100);
	if (test3 != NULL)
	{
		/* a funcao memset trabalha com caracteres, truncando o int para char */
		memset(test3, 0xCCCCCCCC, 100);
		v3 = 1;
	}

	// try to free one variable
	if (v2)
	{
		embedded_free(test2);
	}

	// allocating 2 bytes
	test4 = (short *)embedded_alloc(ptr_mem_buffer, sizeof(short));
	if (test4 != NULL)
	{
		*test4 = 0xDDDD;
		v4 = 1;

	}

	// allocating 2 bytes
	test5 = (short *)embedded_alloc(ptr_mem_buffer, sizeof(short));
	if (test5 != NULL)
	{
		*test5 = 0xEEEE;
		v5 = 1;
	}

	// allocating 4 bytes
	test6 = (int *)embedded_alloc(ptr_mem_buffer, sizeof(int));
	if (test6 != NULL)
	{
		*test6 = 0xFFFFFFFF;
		v6 = 1;
	}


	// starts to completely release the variables - mode 1 - simple free

#ifdef SIMPLE_FREE

	if (v1)
	{
		embedded_free(test);
	}

	if (v3)
	{
		embedded_free(test3);
	}

	if (v4)
	{
		embedded_free(test4);
	}

	if (v5)
	{
		embedded_free(test5);
		embedded_free(test5);
	}

	if (v6)
	{
		embedded_free(test6);
	}

	if (v5)
	{
		embedded_free(test5);
	}

	if (v2)
	{
		embedded_free(test2);
	}

	enable_cleaning();

	if (v1)
	{
		embedded_free(test);
	}
#endif

	// starts to completely release the variables - mode 2 - smart free

#ifdef SMART_FREE

	if (v1)
	{
		embedded_smart_free(test);
	}

	if (v3)
	{
		embedded_smart_free(test3);
	}

	if (v4)
	{
		embedded_smart_free(test4);
	}

	if (v5)
	{
		embedded_smart_free(test5);
		embedded_smart_free(test5);
	}

	if (v6)
	{
		embedded_smart_free(test6);
	}

	if (v5)
	{
		embedded_smart_free(test5);
	}

	if (v2)
	{
		embedded_smart_free(test2);
	}

	enable_cleaning();

	if (v1)
	{
		embedded_smart_free(test);
	}
#endif

	getch();
	}