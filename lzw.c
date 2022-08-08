#include "lzw.h"

const ull 	LZW_INIT_CODESIZE 	= 9,
			LZW_MAX_CODESIZE	= 16,
			LZW_INIT_BUFFSIZE 	= 1024,
			LZW_FIRST_CODE 		= 256;


TrieNode* lzwInitTrie(){
	TrieNode* newTrie = trieInitNode(1);
	byte k = 0;
	
	for(ull i = 0; i < TRIE_ALPHABET_SIZE; i++){
		trieInsertData(newTrie,&k,1,k);
		k++;
	}
	
	return newTrie;
}

DictionaryElement* lzwInitDictionary(){
	DictionaryElement* dict = (DictionaryElement*)malloc(sizeof(DictionaryElement)*(1<<17));
	
	for(size_t i = 0; i < 256; i++){
		dict[i].prefix = ULLONG_MAX;
		dict[i].lastByte = (byte)i; 
	}
	
	return dict;
}

void lzwEncode(InBinaryStream* inStream, OutBinaryStream* outStream){
	TrieNode* root = lzwInitTrie();
	
	uint codeSize = LZW_INIT_CODESIZE;
		 
	ull bitBuffer, 
		previousCodeWord, 
		codeWord = LZW_FIRST_CODE;
	
	size_t 	bitsReaded,
			chainLength = 0, 
			bufferSize = LZW_INIT_BUFFSIZE;
			
	byte* buffer = (byte*)malloc(sizeof(byte)*bufferSize);
	do{
		bitsReaded = readBits(inStream, 8, &bitBuffer);
		if(bitsReaded == 8){
			buffer[chainLength++] = bitBuffer;
			if(trieFindData(root, buffer, chainLength) == TRIE_NOT_FOUND){
				previousCodeWord = trieFindData(root, buffer, chainLength-1);
				writeBits(outStream, previousCodeWord, codeSize);
				
				if(codeWord >= (1<<codeSize)) 
					codeSize++;
				

				trieInsertData(root, buffer, chainLength, codeWord++);
				buffer[0] = buffer[chainLength-1];
				chainLength = 1;
				
				if(codeSize > LZW_MAX_CODESIZE){
					codeWord = LZW_FIRST_CODE;
					codeSize = LZW_INIT_CODESIZE;
					trieFreeNode(root);
					root = lzwInitTrie();
				}
				
				
			}

			if(chainLength == bufferSize){
				bufferSize += LZW_INIT_BUFFSIZE;
				buffer = realloc(buffer, bufferSize);
			}
			
			
		}
	}while(!inStreamEnd(inStream));

	previousCodeWord = trieFindData(root, buffer, chainLength);
	writeBits(outStream, previousCodeWord, codeSize);
	
	free(buffer);
	trieFreeNode(root);
}

byte lzwDictionaryDecode(OutBinaryStream* outStream, DictionaryElement* dict, ull code){
	byte ret = dict[code].lastByte;
	if(dict[code].prefix != ULLONG_MAX){
		ret = lzwDictionaryDecode(outStream, dict, dict[code].prefix);
	}

	writeBits(outStream, (ull)dict[code].lastByte, 8);
	return ret;
}

void lzwDecode(InBinaryStream* inStream, OutBinaryStream* outStream){
	DictionaryElement* dict = lzwInitDictionary();
	
	size_t bitReaded;
	ull bitBuffer;
	
	ull codeWord = LZW_FIRST_CODE;
	uint codeSize = LZW_INIT_CODESIZE;
	
	byte bt;
	ull prevPrefix = ULLONG_MAX;
	
	do{
		bitReaded = readBits(inStream, codeSize, &bitBuffer);
		if(bitReaded == codeSize){
			
			if(bitBuffer < codeWord){
				bt = lzwDictionaryDecode(outStream, dict, bitBuffer);
			}else{
				bt = lzwDictionaryDecode(outStream, dict, prevPrefix);
				writeBits(outStream, bt, 8);
			}

			if(prevPrefix != ULLONG_MAX){
				dict[codeWord].prefix = prevPrefix;
				dict[codeWord].lastByte = bt;
				codeWord++;
			}
			
			prevPrefix = bitBuffer;
			if(codeWord >= (1<<codeSize)){
				codeSize++;
			}
			
			if(codeSize > LZW_MAX_CODESIZE){
				free(dict);
				dict = lzwInitDictionary();
				codeWord = LZW_FIRST_CODE;
				codeSize = LZW_INIT_CODESIZE;
				prevPrefix = ULLONG_MAX;
			}


		}
	}while(!inStreamEnd(inStream));
}