#include "bwt.h"

const ull 	BWT_BLOCK_SIZE = (1<<16),
			BWT_INIT_POS_SIZE = 8;

int bwtPaircmp(const void *p1, const void *p2){
	
	const bwtPair* pair1 = (const bwtPair*)p1;
	const bwtPair* pair2 = (const bwtPair*)p2;
	
	if(pair1->symbol < pair2->symbol){
		return -1;
	}else if(pair1->symbol > pair2->symbol){
		return 1;
	}else if(pair1->position < pair2->position){
		return -1;
	}else{
		return 1;
	}
}


uint bwtLess(size_t a, size_t b, byte* mesBuffer, size_t mSize){
	size_t i = a, j = b;
    size_t k = mSize - 2;
    
    i = i + k - (i + k >= mSize ? mSize : 0);
    j = j + k - (j + k >= mSize ? mSize : 0);

    while((mesBuffer[i] == mesBuffer[j]) && (k > 0)){
    	i = (i == 0 ? mSize - 1 : i - 1);
   		j = (j == 0 ? mSize - 1 : j - 1);
    	k--;
    }
    if (mesBuffer[i] < mesBuffer[j])
        return 1;
    else
        return 0;
}

uint bwtLessEq(size_t a, size_t b, byte* mesBuffer, size_t mSize){
	size_t i = a, j = b;
    size_t k = mSize - 2;
    
    i = i + k - (i + k >= mSize ? mSize : 0);
    j = j + k - (j + k >= mSize ? mSize : 0);

    while((mesBuffer[i] == mesBuffer[j]) && (k > 0)){
    	i = (i == 0 ? mSize - 1 : i - 1);
   		j = (j == 0 ? mSize - 1 : j - 1);
    	k--;
    }
    if (mesBuffer[i] <= mesBuffer[j])
        return 1;
    else
        return 0;
}

void bwtSwap(size_t* a, size_t* b, size_t** firstPtr){
	if(a == *firstPtr){
		*firstPtr = b;
	}
	else if(b == *firstPtr){
		*firstPtr = a;
	}
	
	size_t t = *a;
	*a = *b;
	*b = t;
}


void bwtSort(size_t L, size_t R, size_t* indBuffer, byte* mesBuffer, size_t mSize, size_t** firstPtr){
	size_t i, j, x, m;
	
	if ((R - L) <= 56){
	    for (i = L + 1; i <= R; i++) {
	        x = indBuffer[i], j = i - 1;
	        while ((j >= 0) && (bwtLess(x, indBuffer[j],mesBuffer,mSize))) {
	            bwtSwap(&indBuffer[j + 1],&indBuffer[j], firstPtr);
	            if(j == 0)
	            	break;
	            j--;
	        }
	    }
	    return;
    }
    
	 while (L < R) {
        m = (L + R) / 2;
        if (bwtLess(indBuffer[m], indBuffer[L], mesBuffer, mSize))
            bwtSwap(&indBuffer[L], &indBuffer[m], firstPtr);
        if (bwtLess(indBuffer[R], indBuffer[L], mesBuffer, mSize))
            bwtSwap(&indBuffer[L], &indBuffer[R], firstPtr);
        if (bwtLess(indBuffer[m], indBuffer[R], mesBuffer, mSize))
            bwtSwap(&indBuffer[R], &indBuffer[m], firstPtr);
        x = indBuffer[R];
        
        
        i = L;
        
        for (j = L; j < R; j++)
            if (bwtLessEq(indBuffer[j], x, mesBuffer, mSize))
            	bwtSwap(&indBuffer[i++], &indBuffer[j], firstPtr);
            	
        bwtSwap(&indBuffer[i], &indBuffer[R], firstPtr);
        if (2 * i + 1 > R + L) {
            if ((i + 1) < R)
                bwtSort(i, R, indBuffer, mesBuffer, mSize, firstPtr);
            R = i;
        }
        else {
            if (L < i)
                bwtSort(L, i, indBuffer, mesBuffer, mSize, firstPtr);
            L = i + 1;
        }
    }
}

void bwtEncode(InBinaryStream* inStream, OutBinaryStream* outStream){
	byte mesBuffer[BWT_BLOCK_SIZE];
	size_t indBuffer[BWT_BLOCK_SIZE];
	size_t initPos;
	
	size_t* firstPtr;
	size_t mSize;
	do {
		mSize = readBytes(inStream, mesBuffer, BWT_BLOCK_SIZE);
		if(mSize != 0){
			for(size_t i = 0; i < mSize; i++){
				indBuffer[i] = i;
			}
			firstPtr = &indBuffer[1];
			bwtSort(0, mSize-1, indBuffer, mesBuffer, mSize, &firstPtr);
			initPos = firstPtr - indBuffer;
			for(size_t i = 0; i < mSize; i++){
				writeBytes(outStream, &mesBuffer[(indBuffer[i] + mSize - 1) % mSize], 1);
			}
			writeBytes(outStream, (byte*)&initPos, BWT_INIT_POS_SIZE);
		}
	} while (!inStreamEnd(inStream));
}

void bwtDecode(InBinaryStream* inStream, OutBinaryStream* outStream){
	byte mesBuffer[BWT_BLOCK_SIZE + BWT_INIT_POS_SIZE];
	
	bwtPair* decodeTable = (bwtPair*)malloc(sizeof(bwtPair) * BWT_BLOCK_SIZE);
	
	size_t indBuffer[BWT_BLOCK_SIZE];
	size_t initPos;
	
	size_t* firstPtr;
	size_t mSize;
	do {
		mSize = readBytes(inStream, mesBuffer, BWT_BLOCK_SIZE + BWT_INIT_POS_SIZE);
			if(mSize != 0){
			initPos = *((size_t*)&mesBuffer[mSize-BWT_INIT_POS_SIZE]);
			
			size_t mLen = mSize - BWT_INIT_POS_SIZE;
			
			for(size_t i = 0; i < mLen; i++){
				decodeTable[i].symbol = mesBuffer[i];
				decodeTable[i].position = i;
			}
		
			qsort(decodeTable, mLen, sizeof(bwtPair), bwtPaircmp);
			
			bwtPair tempPair = decodeTable[0];
			bwtPair curPair = decodeTable[0];
			size_t curInd = 0;
			
		
			for(size_t i = 0; i < mLen; i++){
				tempPair = decodeTable[curPair.position];
				decodeTable[curPair.position] = curPair;
				decodeTable[curPair.position].position = curInd;
				curInd = curPair.position;
				curPair = tempPair;
			}
		
			for(size_t i = 0; i < mLen; i++){
				writeBytes(outStream, (byte*)&decodeTable[initPos].symbol, 1);
				initPos = decodeTable[initPos].position;
			}
		}
	} while (!inStreamEnd(inStream));
	free(decodeTable);
}