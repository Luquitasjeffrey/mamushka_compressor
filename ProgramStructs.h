/*
this file exists because i´m using nested functions in some C files, and
i want to include some header files just to make my code run a little bit
more faster.
*/
#include <stdint.h>
#pragma once

typedef struct HCODE{
	short int h_int:2;
}HCODE;

typedef struct CHAR{
	uint16_t      character[2];
	unsigned int  occurence;
	uint16_t 	  originalpos;
	short int     addr;
	HCODE		  huffmancode[32];
}CHAR;

typedef struct CHARLIST{
	CHAR chars[4096];
	short int size;
}CHARLIST;

typedef struct BinaryString{
	uint32_t bsize;
	uint32_t binarypos;
	uint8_t  str[0x4000000];
}BinaryString;
