#include "mtf.h"

const ull 	MTF_INIT_MIN_KEY = 256,
			MTF_INIT_TREAP_SIZE = 256;

TreapNode* mtfInitTreap(ull beginInd, ull endInd, ull biggestPriority) {
	TreapNode* root = treapInitNode((beginInd + endInd) / 2,
									biggestPriority, 
									(beginInd + endInd) / 2);
	TreapNode* temp;
	if ((beginInd + endInd) / 2 != beginInd) {
		temp = mtfInitTreap(beginInd,
							(beginInd + endInd) / 2 - 1,
							biggestPriority - (endInd - (beginInd + endInd) / 2 + 1));
		root = treapMerge(temp, root);
	}
	if ((beginInd + endInd) / 2 != endInd) {
		temp = mtfInitTreap((beginInd + endInd) / 2 + 1,
							endInd,
							biggestPriority - 1);
		root = treapMerge(root, temp);

	}
	return root;
}

ull* mtfInitKeys(size_t kSize) {
	ull* keys = (ull*)malloc(sizeof(ull)*kSize);
	for (size_t i = 0; i < kSize; i++) {
		keys[i] = i;
	}
	return keys;
}

void mtfEncode(InBinaryStream* inStream, OutBinaryStream* outStream) {
	TreapNode* 	treap = mtfInitTreap(0, MTF_INIT_TREAP_SIZE-1, MTF_INIT_TREAP_SIZE-1),
				*tmpNode;
				
	ull* keys = mtfInitKeys(MTF_INIT_TREAP_SIZE);
	
	size_t bitsReaded;
	
	ull minKey = MTF_INIT_MIN_KEY, 
		bitBuffer;
			
	byte code;
		 
	do {
		bitsReaded = readBits(inStream, 8, &bitBuffer);
		if(bitsReaded == 8){
			code = treapGetCodeword(treap, keys[bitBuffer]);
			writeBits(outStream, code, 8);
			tmpNode = treapFind(treap, keys[bitBuffer]);
			treap = treapRemove(treap, keys[bitBuffer]);
			keys[bitBuffer] = minKey;
			treap = treapInsert(treap, minKey++, tmpNode->key.priority, tmpNode->data);
		}
	} while (!inStreamEnd(inStream));
	
	free(keys);
	treapFreeNode(treap);
}

void mtfDecode(InBinaryStream* inStream, OutBinaryStream* outStream) {
	TreapNode* 	treap = mtfInitTreap(0, MTF_INIT_TREAP_SIZE-1, MTF_INIT_TREAP_SIZE-1),
				*tmpNode;
	
	size_t bitsReaded;
				
	ull minKey = MTF_INIT_MIN_KEY,
		bitBuffer;
		
	byte code;
	
	do {
		bitsReaded = readBits(inStream, 8, &bitBuffer);
		if(bitsReaded == 8){
			tmpNode = treapFindByCodeword(treap, bitBuffer);
			writeBits(outStream, tmpNode->data, 8);
			treap = treapRemove(treap, tmpNode->key.key);
			treap = treapInsert(treap, minKey++, tmpNode->key.priority, tmpNode->data);
		}
	} while (!inStreamEnd(inStream));
	
	treapFreeNode(treap);
}


/*
void mtfEncode(InBinaryStream* inStream, OutBinaryStream* outStream){
	byte* list = mtfInitList(256);

	uint bitReaded;
	ull code;
	ull mtfCode;
	do{
		bitReaded = readBits(inStream, 8, &code);
		if(bitReaded == 8){
			mtfCode = mtfFindSymbol(code, list, 256);
			mtfSymbol(mtfCode, list, 256);
			writeBits(outStream, mtfCode, 8);
		}
	}while(!inStreamEnd(inStream));
}
*/