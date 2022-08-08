#ifndef RLE_H
#define RLE_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "IOBinaryStream.h"

void rleV1Encode(InBinaryStream* inStream, OutBinaryStream* outStream);
void rleV1Decode(InBinaryStream* inStream, OutBinaryStream* outStream);

void rleV2Encode(InBinaryStream* inStream, OutBinaryStream* outStream);
void rleV2Decode(InBinaryStream* inStream, OutBinaryStream* outStream);

void rleV3Encode(InBinaryStream* inStream, OutBinaryStream* outStream);
void rleV3Decode(InBinaryStream* inStream, OutBinaryStream* outStream);

#endif