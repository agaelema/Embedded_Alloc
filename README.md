# Embedded_Alloc
Embedded and simple version of the "malloc/free" functions.

This repository is in the initial state and the functions will be updated as possible.
This code started to be developed in a course exercise and it was modified considering embedded systems and their limitations.


A defined buffer is used to allocate the variables (buffer limit of 65535 bytes). Each variable has a header (meta_header) with the size and state of use (used/not used).

Allocated variable:
*	----------------------------------------------------------------------
*	| Variable size | Allocated (Yes/No) | Variable n-size |
*	|-----2 Bytes----|----------1 Byte--------|------n*Bytes-----|
*	----------------------------------------------------------------------

When the buffer is initiated the first meta_head is initialized serving as base for the upcoming allocation. It's possible to work with more than one buffer.

The function "*embedded_alloc( )" search for a empty block and than alloc. Return NULL if there is no free block.

The function "embedded_free( )" just change the header to unused.

The functio "embedded_smart_free( )" verify if the nest block is free and concatenate both.


Last updates:
- Initial fork
- Initial example tested in the VS2015

To Do:
- create more examples
- Test the code in microcontrollers
- more...

Haroldo L. M. Amaral
