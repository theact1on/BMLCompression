#ifndef TRIE_H
#define TRIE_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

typedef unsigned char byte;
typedef unsigned int uint;
typedef unsigned long long ull;

typedef struct TrieNode TrieNode;

extern const ull 	TRIE_EMPTY,
					TRIE_NOT_FOUND,
					TRIE_ALPHABET_SIZE;

struct TrieNode{
	TrieNode** childs;
	ull code;
};

TrieNode* trieInitNode(ull code);
void trieFreeNode(TrieNode* node);
void trieInsertData(TrieNode* trieNode, byte* data, size_t dataSize, ull code);
ull trieFindData(TrieNode* trieNode, byte* data, size_t dataSize);

#endif