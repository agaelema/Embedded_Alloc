/*
*	embedded_alloc.c - version: 1.0
*
*	Functions used to perform the dynamic allocation of variables
*	It uses a defined buffer of "n" bytes (uint8_t) to handle the variables
*	Each new allocated variable has a identifier (meta_header)
*	Buffer with limit of 65535 bytes
*	
*	--------------------------------------------------------
*	| Variable size | Allocated (Yes/No) | Variable n-size |
*	|     2 Bytes   |       1 Byte       |      n*Bytes    |
*	--------------------------------------------------------
*
*	Created by Haroldo Luiz Moretti do Amaral, 18/09/2015
*	agaelema@globo.com
*	Released into the public domain
*/

#include	<stdint.h>
#include	<string.h>
#include	"embedded_alloc.h"

#define		USED		1					// used variable flag - "1"
#define		UN_USED		0					// unused variable flag - "0"
#define		META_HEADER	3					// size of the meta_header - "3 bytes"
#define		META_BYTE	0
#define		META_USED	1

volatile static uint8_t otimiza_free = 0;			// flag to enable optimization
volatile static uint8_t clean_block = 0;			// flag to enable the block cleanning
volatile static uint32_t upper_block_limit = 0;		// variable to save the upper addr limit
volatile static uint16_t save_block_size = 0;		// variable to save the block released and compare
volatile static uint16_t mem_block_size = 0;		// variable to save the size of memory block


/* Functions */

/*
*	Initializes the buffer used to handle the variables
*	Starts the first meta_header
*/
void embedded_alloc_init(void *buff, uint16_t buffer_size)
{
	/* pointer to save the address passed by the argument */
	uint16_t *buffer_addr;
	buffer_addr = (uint16_t *)buff;

	/* Set the variable (static) with the buffer size - number of bytes */
	mem_block_size = buffer_size;

	/* If the size is less than meta_header, force 0 bytes free */
	if (mem_block_size <= META_HEADER)
	{
		*(buffer_addr + META_BYTE) = 0;
	}
	else
	{
		/* Set the entire buffer with zero - clean the memory */
		memset(buffer_addr, 0, mem_block_size);

		/* Set the number of free bytes: total size - meta_header */
		*(buffer_addr + META_BYTE) = buffer_size - META_HEADER;
		/* Force as non used */
		*(buffer_addr + META_USED) = 0;
	}
}


/*
*	Set the size of the current buffer (static variable)
*	Allows to use multiple buffers
*/
void set_block_size(uint16_t size)
{
	mem_block_size = size;
}


/*
*	Set the upper limit (address) of the current buffer
*	Used with multiple buffers
*/
void set_block_limit(void *var, uint16_t size)
{
	uint8_t *var_addr;
	var_addr = var;

	upper_block_limit = var_addr + size;
}


/*
*	allocate the variable in the buffer (if there is a minimum of bytes needed)
*/
void *embedded_alloc(void *buff, uint16_t num)
{
	uint16_t *current_addr_size;
	uint8_t *current_block_used;

	current_addr_size = (uint8_t *)buff + META_BYTE;
	current_block_used = (uint16_t *)current_addr_size + META_USED;

	/* If the current size is zero - return null */
	if (*current_addr_size == 0)
	{
		return NULL;
	}

	uint8_t verify_used;
	uint16_t verify_size;

	verify_used = *current_block_used;
	verify_size = *current_addr_size;

	/* search for the first empty block with the needed size */
	while ((verify_used != 0) || ((verify_size + META_HEADER) < num))
	{
		/* temporary variable - because the difference between uint8 and uint16 */
		uint8_t *current_temp;
		current_temp = current_addr_size;

		/* update the next address - the address points to the first byte after the meta_header */
		current_temp += (*current_addr_size + META_HEADER);
		current_addr_size = current_temp;

		/* verifies if the new block is marked as used */
		current_block_used = (uint16_t *)current_addr_size + META_USED;

		verify_used = *current_block_used;
		verify_size = *current_addr_size;

		/* prevents new meta_header to be saved out of the buffer */
		if (((uint8_t *)current_addr_size - buff) >= ((uint8_t *)mem_block_size - META_HEADER))
		{
			return NULL;
		}
	}

	/* 
	*	If the size of the current block is bigger than meta_header
	*	- create the meta_header of the next block
	*	if not, does not create the next meta_header
	*/
	if ((*current_addr_size - num) > 3)
	{
		uint16_t *new_addr;				// address with size of block
		uint8_t *new_block_used;		// flag indicanting the use

		/* next address of size indicator = current addrs + 3 bytes of meta_header + size of allocated variable*/
		new_addr = (uint8_t *)current_addr_size + META_HEADER + num;
		new_block_used = new_addr + META_USED;

		/* Update the next size with the remaining bytes */
		*new_addr = *current_addr_size - META_HEADER - num;
		/* force the next block as unused - avoid possible garbage */
		*new_block_used = UN_USED;

		/* Save the current size and mark as used */
		*current_addr_size = num;
		*current_block_used = USED;
	}
	else
	{
		*current_block_used = USED;
	}

	/* return the address of the first byte after the meta_header */
	return ((uint8_t *)current_addr_size + META_HEADER);
}

/* 
*	frees an allocated variable
*/
void embedded_free(void *var)
{
	uint16_t *mem_block;
	mem_block = (uint16_t *)var;

	/* update the block as unused */
	*((uint8_t *)mem_block - 1) = UN_USED;
	/* save the number of released bytes in a static variable - used in smart_free() */
	save_block_size = *((uint16_t *)((uint8_t *)mem_block - 3));

	/*
	*	Verify if the next block was released, case yes concatenate with the current
	*	activated with the function smart_free()
	*/
	if (otimiza_free == 1)
	{
		uint16_t *next_block;
		uint8_t *next_block_temp;

		next_block_temp = (uint8_t *)mem_block + *(uint16_t *)((uint8_t *)mem_block - 3) + META_HEADER;
		next_block = next_block_temp;

		if (next_block < upper_block_limit)
		{
			if (*((uint8_t *)next_block - 1) == 0)
			{
				*((uint16_t *)((uint8_t *)mem_block - 3)) += (*((uint16_t *)((uint8_t *)next_block - 3)) + 3);
				*((uint16_t *)((uint8_t *)next_block - 3)) = 0;
			}
		}
	}
}

/*
*	frees the memory block optimizing the free space
*	checks whether the next block is free and joining its size
*	In the end you can clear the data - saves 0 - enable_cleaning()
*/
void embedded_smart_free(void *var)
{
	volatile uint16_t compara_block_size;
	uint16_t *var_addr;
	var_addr = var;

	otimiza_free = 1;
	do
	{
		embedded_free(var);
		compara_block_size = *((uint16_t *)((uint8_t *)var_addr - 3));
	} while (compara_block_size != save_block_size);
	otimiza_free = 0;

	/* fill the bytes with zeros */
	if (clean_block == 1)
	{
		memset(var_addr, 0, *((uint16_t *)((uint8_t *)var_addr - 3)));
	}
}

/*
*	Enable the cleaning in the smart_free
*/
void enable_cleaning()
{
	clean_block = 1;
}

/*
*	Disable the cleaning in the smart_free
*/
void disable_cleaning()
{
	clean_block = 0;
}
