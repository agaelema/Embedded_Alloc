/*
*	embedded_alloc.h - version: 1.0
*
*	Created by Haroldo Luiz Moretti do Amaral, 18/09/2015
*	agaelema@globo.com
*	Released into the public domain
*/

#ifndef EMBEDDED_ALLOC_H_   /* Include guard */
#define EMBEDDED_ALLOC_H_

#include <stdint.h>

/*
*	Initializes the buffer used to handle the variables
*	Starts the first meta_header
*/
void embedded_alloc_init(void *buff, uint16_t buffer_size);

/*
*	Set the size of the current buffer (static variable)
*	Allows to use multiple buffers
*/
void set_block_size(uint16_t size);

/*
*	Set the upper limit (address) of the current buffer
*	Used with multiple buffers
*/
void set_block_limit(void *var, uint16_t size);

/*
*	allocate the variable in the buffer (if there is a minimum of bytes needed)
*/
void *embedded_alloc(void *buff, uint16_t num);

/*
*	frees an allocated variable
*/
void embedded_free(void *var);

/*
*	frees the memory block optimizing the free space
*	checks whether the next block is free and joining its size
*	In the end you can clear the data - saves 0 - enable_cleaning()
*/
void embedded_smart_free(void *var);

/*
*	Enable the cleaning in the smart_free
*/
void enable_cleaning();

/*
*	Disable the cleaning in the smart_free
*/
void disable_cleaning();


#endif // EMBEDDED_ALLOC_H_
