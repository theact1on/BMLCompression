#ifndef BWT_H
#define BWT_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "IOBinaryStream.h"

extern const ull 	BWT_BLOCK_SIZE,
					BWT_INIT_POS_SIZE;

typedef struct bwtPair bwtPair;
struct bwtPair{
	byte symbol;
	size_t position;
};

int bwtPaircmp(const void *p1, const void *p2);
uint bwtLess(size_t a, size_t b, byte* mesBuffer, size_t mSize);
uint bwtLessEq(size_t a, size_t b, byte* mesBuffer, size_t mSize);
void bwtSwap(size_t* a, size_t* b, size_t** firstPtr);
void bwtSort(size_t L, size_t R, size_t* indBuffer, byte* mesBuffer, size_t mSize, size_t** firstPtr);
void bwtEncode(InBinaryStream* inStream, OutBinaryStream* outStream);
void bwtDecode(InBinaryStream* inStream, OutBinaryStream* outStream);

#endif