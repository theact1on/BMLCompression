#include "rle.h"

void rleV1Encode(InBinaryStream* inStream, OutBinaryStream* outStream){
	ull bitBuffer;
	ull previousByte;
	ull byteCounter = 1;
	
	size_t 	bitsReaded;
	
	bitsReaded = readBits(inStream, 8, &previousByte);
	if(bitsReaded == 8){
		while(!inStreamEnd(inStream)){
			bitsReaded = readBits(inStream, 8, &bitBuffer);
			if(bitsReaded == 8){
				if((previousByte != bitBuffer || byteCounter == 255)){
						
					writeBits(outStream, previousByte, 8);
					writeBits(outStream, byteCounter, 8);
					byteCounter = 1;
					
				}else{
					byteCounter++;
				}
				
				
				previousByte = bitBuffer;
			}
		}
		writeBits(outStream, previousByte, 8);
		writeBits(outStream, byteCounter, 8);
	}
}

void rleV1Decode(InBinaryStream* inStream, OutBinaryStream* outStream){
	ull bitBuffer;
	ull byteCounter;
	
	size_t 	bitsReaded;
	
	do{
		bitsReaded = readBits(inStream, 8, &bitBuffer);
		bitsReaded = readBits(inStream, 8, &byteCounter);
		for(ull i = 0; i < byteCounter; i++){
			writeBits(outStream, bitBuffer, 8);
		}
	}while(!inStreamEnd(inStream));
}

void rleV2Encode(InBinaryStream* inStream, OutBinaryStream* outStream){
	ull bitBuffer;
	ull previousByte;
	ull byteCounter = 1;
	
	size_t 	bitsReaded;
	
	bitsReaded = readBits(inStream, 8, &previousByte);
	if(bitsReaded == 8){
		while(!inStreamEnd(inStream)){
			bitsReaded = readBits(inStream, 8, &bitBuffer);
			if(bitsReaded == 8){
				if((previousByte != bitBuffer || byteCounter == 256)){
					writeBits(outStream, previousByte, 8);
					if(byteCounter > 1){
						writeBits(outStream, previousByte, 8);
						writeBits(outStream, byteCounter-1, 8);	
					}
					byteCounter = 1;
					
				}else{
					byteCounter++;
				}
				
				
				previousByte = bitBuffer;
			}
		}
		writeBits(outStream, previousByte, 8);
		if(byteCounter > 1){
			writeBits(outStream, previousByte, 8);
			writeBits(outStream, byteCounter-1, 8);	
		}
	}
}

void rleV2Decode(InBinaryStream* inStream, OutBinaryStream* outStream){
	ull firstByte;
	ull secondByte;
	ull byteCounter = 0;
	
	size_t 	bitsReaded;
	
	bitsReaded = readBits(inStream, 8, &firstByte);
	if(bitsReaded == 8){
		while(!inStreamEnd(inStream)){
			bitsReaded = readBits(inStream, 8, &secondByte);
			if(bitsReaded == 8){
				if(firstByte == secondByte){
					bitsReaded = readBits(inStream, 8, &byteCounter);
					for(ull i = 0; i < byteCounter+1; i++){
						writeBits(outStream, secondByte, 8);
					}
					bitsReaded = readBits(inStream, 8, &firstByte);
					if(bitsReaded != 0){
						byteCounter = 0;
					}
				}else{
					writeBits(outStream, firstByte, 8);
					firstByte = secondByte;
					byteCounter = 0;
				}
			}
			
		}
		if(byteCounter == 0){
			writeBits(outStream, firstByte, 8);
		}
	}
}

void rleV3Encode(InBinaryStream* inStream, OutBinaryStream* outStream){
	ull bitBuffer;
	ull previousByte;
	ull byteCounter = 0;
	
	size_t 	bitsReaded;
	do{
		bitsReaded = readBits(inStream, 8, &bitBuffer);
		if(bitsReaded == 8){
			if(bitBuffer == 0x00){
				byteCounter++;
				if(byteCounter == 255){
					writeBits(outStream, 0xFF, 8);
					writeBits(outStream, byteCounter, 8);
					byteCounter = 0;
				}
			}else{
				if(byteCounter == 1){
					writeBits(outStream, 0x00, 8);
				}else if(byteCounter > 1){
					writeBits(outStream, 0xFF, 8);
					writeBits(outStream, byteCounter, 8);
				}
				
				if(bitBuffer == 0xFF){
					writeBits(outStream, 0xFF, 8);
					writeBits(outStream, 0x00, 8);
				}else{
					writeBits(outStream, bitBuffer, 8);	
				}
				byteCounter = 0;
			}
		}
	}while(!inStreamEnd(inStream));
	
	if(byteCounter == 1){
		writeBits(outStream, 0x00, 8);
	}else if(byteCounter > 1){
		writeBits(outStream, 0xFF, 8);
		writeBits(outStream, byteCounter, 8);
	}
}

void rleV3Decode(InBinaryStream* inStream, OutBinaryStream* outStream){
	ull bitBuffer;
	ull previousByte;
	ull byteCounter = 0;
	
	size_t 	bitsReaded;
	do{
		bitsReaded = readBits(inStream, 8, &bitBuffer);
		if(bitsReaded == 8){
			if(bitBuffer == 0xFF){
				bitsReaded = readBits(inStream, 8, &byteCounter);
				if(byteCounter == 0){
					writeBits(outStream, 0xFF, 8);
				}else{
					for(ull i = 0; i < byteCounter; i++){
						writeBits(outStream, 0x00, 8);
					}
				}
			}else{
				writeBits(outStream, bitBuffer, 8);
			}
		}
	}while(!inStreamEnd(inStream));
}