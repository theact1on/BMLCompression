#ifndef LZW_H
#define LZW_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include "IOBinaryStream.h"
#include "trie.h"

typedef struct DictionaryElement DictionaryElement;

extern const ull 	LZW_INIT_CODESIZE,
					LZW_MAX_CODESIZE,
					LZW_INIT_BUFFSIZE,
					LZW_FIRST_CODE;

struct DictionaryElement{
	ull prefix;
	byte lastByte;
};

TrieNode* lzwInitTrie();
DictionaryElement* lzwInitDictionary();
void lzwEncode(InBinaryStream* inStream, OutBinaryStream* outStream);
byte lzwDictionaryDecode(OutBinaryStream* outStream, DictionaryElement* dict, ull code);
void lzwDecode(InBinaryStream* inStream, OutBinaryStream* outStream);

#endif