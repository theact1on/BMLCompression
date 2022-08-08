#include "trie.h"

const ull 	TRIE_EMPTY 			= ULLONG_MAX,
			TRIE_NOT_FOUND 		= ULLONG_MAX-1,
			TRIE_ALPHABET_SIZE 	= 256;

TrieNode* trieInitNode(ull code){
	TrieNode* p = (TrieNode*)malloc(sizeof(TrieNode));
	
	p->code = code;
	p->childs = (TrieNode**)malloc(sizeof(TrieNode*) * TRIE_ALPHABET_SIZE);
	memset(p->childs, 0, TRIE_ALPHABET_SIZE*sizeof(TrieNode*));
	
	return p;	
}

void trieFreeNode(TrieNode* node){
	for(size_t i = 0; i < TRIE_ALPHABET_SIZE; i++){
		if(node->childs[i] != NULL){
			trieFreeNode(node->childs[i]);
		}
	}
	free(node);
	return;
}

void trieInsertData(TrieNode* trieNode, byte* data, size_t dataSize, ull code){
	TrieNode* init = trieNode;
	for(size_t i = 0; i < dataSize-1; i++){
		if(init->childs[data[i]] == NULL){
			init->childs[data[i]] = trieInitNode(TRIE_EMPTY);
		}
		init = init->childs[data[i]];
	}
	if(dataSize != 0){
		init->childs[data[dataSize-1]] = trieInitNode(code);
	}
	
}

ull trieFindData(TrieNode* trieNode, byte* data, size_t dataSize){
	TrieNode* init = trieNode;

	for(size_t i = 0; i < dataSize; i++){
		if(init->childs[data[i]] == NULL)
			return TRIE_NOT_FOUND;
			
		init = init->childs[data[i]];
	}

	return init->code;
}
