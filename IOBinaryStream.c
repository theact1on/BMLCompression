#include "IOBinaryStream.h"

InBinaryStream* openInBinaryStream(const char* inFilename){
	InBinaryStream* inStream = (InBinaryStream*)malloc(sizeof(InBinaryStream));
	inStream->file = fopen(inFilename, "rb");
	inStream->mask = 0x80;
	inStream->curByte = 0;
	return inStream;
}

OutBinaryStream* openOutBinaryStream(const char* outFilename){
	OutBinaryStream* outStream = (OutBinaryStream*)malloc(sizeof(OutBinaryStream));
	outStream->file = fopen(outFilename, "wb");
	outStream->mask = 0x80;
	outStream->curByte = 0;
	return outStream;
}

void writeBits(OutBinaryStream* outStream, ull code, uint bitCount){
	ull mask = 1ULL<<(bitCount-1);
	while(mask != 0){
		if(mask & code){
			outStream->curByte |= outStream->mask;
		}
		outStream->mask >>= 1;
		if(outStream->mask == 0){
			fwrite(&(outStream->curByte), sizeof(byte), 1, outStream->file);
			outStream->mask = 0x80;
			outStream->curByte = 0;
		}
		mask >>= 1;
	}
}

size_t readBits(InBinaryStream* inStream, uint bitCount, ull* retVal){
	ull mask = 1ULL<<(bitCount-1);
	ull retValue = 0;
	uint bitReaded = 0;
	while(mask != 0){
		if(inStream->mask == 0x80){
			fread(&(inStream->curByte), sizeof(byte), 1 ,inStream->file);
			if(feof(inStream->file)) 
				break;
		}
		if(inStream->curByte & inStream->mask){
			retValue |= mask;
		}
		inStream->mask >>= 1;
		mask >>= 1;
		if(inStream->mask == 0){
			inStream->mask = 0x80;
		}
		bitReaded++;
	}
	*retVal = retValue;
	
	return bitReaded;
}

void writeBytes(OutBinaryStream* outStream, byte* buffer, size_t bSize){
	if(outStream->mask != 0x80){
		fwrite(&(outStream->curByte), sizeof(byte), 1, outStream->file);
		outStream->mask = 0x80;
		outStream->curByte = 0;
	}
	fwrite(buffer, sizeof(byte), bSize, outStream->file);
}


size_t readBytes(InBinaryStream* inStream, byte* buffer, size_t bSize){
	inStream->mask = 0x80;
	return fread(buffer, sizeof(byte), bSize ,inStream->file);
}

void closeOutBinaryStream(OutBinaryStream* outStream){
	if(outStream->mask != 0x80){
		fwrite(&(outStream->curByte), sizeof(byte), 1, outStream->file);
	}
	fclose(outStream->file);
	free(outStream);
}

void closeInBinaryStream(InBinaryStream* inStream){
	fclose(inStream->file);
	free(inStream);
}

int inStreamEnd(InBinaryStream* inStream){
	int feofRet = feof(inStream->file);
	if(inStream->mask == 0x80 && feofRet != 0){
		return feofRet;
	}else{
		return 0;
	}
}