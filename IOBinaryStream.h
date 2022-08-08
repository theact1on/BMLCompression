#ifndef IO_BINARY_STREAM_H
#define IO_BINARY_STREAM_H

#include <stdio.h>
#include <stdlib.h>

typedef unsigned char byte;
typedef unsigned int uint;
typedef unsigned long long ull;

typedef struct IBinaryStream InBinaryStream;
struct IBinaryStream{
	FILE* file;
	byte mask;
	byte curByte;
};

typedef struct OBinaryStream OutBinaryStream;
struct OBinaryStream{
	FILE* file;
	byte mask;
	byte curByte;
};


InBinaryStream* openInBinaryStream(const char* inFilename);
OutBinaryStream* openOutBinaryStream(const char* outFilename);
void writeBits(OutBinaryStream* outStream, ull code, uint bitCount);
size_t readBits(InBinaryStream* inStream, uint bitCount, ull* retCode);
void writeBytes(OutBinaryStream* outStream, byte* buffer, size_t bSize);
size_t readBytes(InBinaryStream* inStream, byte* buffer, size_t bSize);
void closeOutBinaryStream(OutBinaryStream* outStream);
void closeInBinaryStream(InBinaryStream* inStream);
int inStreamEnd(InBinaryStream* inStream);

#endif