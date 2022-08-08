#ifndef MTF_H
#define MTF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "IOBinaryStream.h"
#include "treap.h"

extern const ull 	MTF_INIT_MIN_KEY,
					MTF_INIT_TREAP_SIZE;

TreapNode* mtfInitTreap(ull beginInd, ull endInd, ull biggestPriority);
ull* mtfInitKeys(size_t kSize);
void mtfEncode(InBinaryStream* inStream, OutBinaryStream* outStream);
void mtfDecode(InBinaryStream* inStream, OutBinaryStream* outStream);

#endif