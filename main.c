#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "IOBinaryStream.h"
#include "bwt.h"
#include "mtf.h"
#include "lzw.h"
#include "rle.h"

typedef enum BML_ALGO BML_ALGO;
typedef enum BML_MODE BML_MODE;
enum BML_ALGO {BML_ALGO_BWT = 0, BML_ALGO_MTF, BML_ALGO_LZW, BML_ALGO_RLEv1, BML_ALGO_RLEv2, BML_ALGO_RLEv3};
enum BML_MODE {BML_MODE_UNKNOWN = 0, BML_MODE_COMPRESS, BML_MODE_DECOMPRESS};

const ull BML_ALGO_MAX_SIZE = 10;

const char* const 	BML_BWT_TMP = "bwt.tmp",
		  * const	BML_MTF_TMP = "mtf.tmp",
		  * const	BML_LZW_TMP = "lzw.tmp";

void bmlEncode(const char* inFilename, const char* outFilename, BML_ALGO* algoSequence, uint sequenceSize){
	char tempFilename[256];
	const char* curFilename = tempFilename;
	strcpy(tempFilename, inFilename);

	uint tempNum = 0;

	InBinaryStream* inFile;
	OutBinaryStream* outFile;

	for(uint i = 0; i < sequenceSize; i++){
		inFile = openInBinaryStream(curFilename); 

		sprintf(tempFilename, "%u.tmp", tempNum++);
		if(i == sequenceSize - 1) curFilename = outFilename;

		outFile = outFile = openOutBinaryStream(curFilename);

		switch(algoSequence[i]){
			case BML_ALGO_BWT:	bwtEncode(inFile, outFile);
							 	break;
			case BML_ALGO_MTF:	mtfEncode(inFile, outFile);
								break;
			case BML_ALGO_LZW:	lzwEncode(inFile, outFile);
								break;
			case BML_ALGO_RLEv1:rleV1Encode(inFile, outFile);
								break;
			case BML_ALGO_RLEv2:rleV2Encode(inFile, outFile);
								break;
			case BML_ALGO_RLEv3:rleV3Encode(inFile, outFile);
								break;
			default:
				printf("Error: Unknown algorithm in algoSequence!\n");
				exit(-14);
		}
		closeInBinaryStream(inFile);
		closeOutBinaryStream(outFile);
	}
}

void bmlDecode(const char* inFilename, const char* outFilename, BML_ALGO* algoSequence, uint sequenceSize){
	char tempFilename[256];
	const char* curFilename = tempFilename;
	strcpy(tempFilename, inFilename);

	uint tempNum = 0;

	InBinaryStream* inFile;
	OutBinaryStream* outFile;

	for(int i = (int)sequenceSize - 1; i >= 0; i--){
		//printf("TEST\n");
		inFile = openInBinaryStream(curFilename); 

		sprintf(tempFilename, "%u.tmp", tempNum++);
		if(i == 0) curFilename = outFilename;

		outFile = outFile = openOutBinaryStream(curFilename);

		switch(algoSequence[i]){
			case BML_ALGO_BWT:	bwtDecode(inFile, outFile);
							 	break;
			case BML_ALGO_MTF:	mtfDecode(inFile, outFile);
								break;
			case BML_ALGO_LZW:	lzwDecode(inFile, outFile);
								break;
			case BML_ALGO_RLEv1:rleV1Decode(inFile, outFile);
								break;
			case BML_ALGO_RLEv2:rleV2Decode(inFile, outFile);
								break;
			case BML_ALGO_RLEv3:rleV3Decode(inFile, outFile);
								break;
			default:
				printf("Error: Unknown algorithm in algoSequence!\n");
				exit(-14);
		}
		closeInBinaryStream(inFile);
		closeOutBinaryStream(outFile);
	}
}

BML_ALGO getCodeAbbreviation(const char* abbr){
	if(strcmp(abbr, "b") == 0){
		return BML_ALGO_BWT;
	}else if(strcmp(abbr, "m") == 0){
		return BML_ALGO_MTF;
	}else if(strcmp(abbr, "l") == 0){
		return BML_ALGO_LZW;
	}else if(strcmp(abbr, "r1") == 0){
		return BML_ALGO_RLEv1;
	}else if(strcmp(abbr, "r2") == 0){
		return BML_ALGO_RLEv2;
	}else if(strcmp(abbr, "r3") == 0){
		return BML_ALGO_RLEv3;
	}else{
		return -1;
	}
}

uint bmlMaskToArray(const char* algoMask, BML_ALGO* algoSeq){

	char* mask = (char*)malloc(sizeof(char) * (strlen(algoMask) + 1));
	strcpy(mask, algoMask);
	char* curPos = strtok(mask, ":");

	size_t curAlgoInd = 0;
	do{
		algoSeq[curAlgoInd] = getCodeAbbreviation(curPos);
		if(algoSeq[curAlgoInd] == -1){
			printf("Error: Unknown abbreviation '%s' in sequence\n", curPos);
			exit(-12);
		}
		curAlgoInd++;
		if(curAlgoInd >= BML_ALGO_MAX_SIZE){
			printf("Error: Too much algorithms in sequence\n", curPos);
			exit(-13);
		}
		curPos = strtok(NULL, ":");
	}while(curPos != NULL);

	free(mask);
	return curAlgoInd;
}

int main(int argc, char* argv[]){

	if(argc <= 1){
		fprintf(stderr,"Run 'bml --help' for more information.",argv[0]);
		return 0;
	}

	if(argc == 2 && !strcmp(argv[1], "--help")){
		printf(	"Arguments should look like this:\n"
				"bml [-c|-d] -i <filename> -o <filename> [-s <sequence mask>]\n"
				"\n"
				"Where:\n"
				"-c			= Compress file.\n"
				"-d			= Decompress file.\n"
				"\n"
				"-i <filename>		= Input file for compress/decompress.\n"
				"-o <filename>		= Output file for compress/decompress.\n"
				"-s <sequence mask>	= Set the sequence of encoding algorithms\n"
				"			  Where <sequence mask> is a sequence of abbreviated algorithm names separated by ':'.\n"
				"			  Abbreviated algorithm names:\n"
				"			  b  - Burrows-Wheeler transform (BWT)\n"
				"			  m  - Move-to-front transform (MTF)\n"
				"			  l  - Lempel-Ziv-Welch algorithm (LZW)\n"
				"			  r1 - Run-length encoding version 1 (RLEv1)\n"
				"			  r2 - Run-length encoding version 2 (RLEv2)\n"
				"			  r3 - Run-length encoding version 3 (RLEv3)\n\n"
				"			  Example: bml -c -i file.in -o file.out -s b:r1:m:l\n"
				"			  The example compresses the file 'file.in' by the chain\n"
				"			  of algorithms BWT -> RLEv1 -> MTF -> LZW and places the\n"
				"			  result in the file 'file.out'\n\n"
				"			  Note: the default chain is BWT -> MTF -> LZW");
		return 0;
	}

	BML_MODE mode = BML_MODE_UNKNOWN;
	uint inFilenameInd = 0;
	uint outFilenameInd = 0;
	uint maskInd = 0;

	for(int i = 1; i < argc; i++){
		if(strcmp(argv[i], "-i") == 0){
			if(inFilenameInd == 0){
				inFilenameInd = ++i;
				if(inFilenameInd >= argc){
					printf("Error: No filename was specified for the '-i' argument\n");
					return -8;
				}
			}else{
				printf("Error: Multiple uses of the '-i' argument\n");
				return -1;
			}
		}else if(strcmp(argv[i], "-o") == 0){
			if(outFilenameInd == 0){
				outFilenameInd = ++i;
				if(outFilenameInd >= argc){
					printf("Error: No filename was specified for the '-o' argument\n");
					return -9;
				}
			}else{
				printf("Error: Multiple uses of the '-o' argument\n");
				return -2;
			}
		}else if(strcmp(argv[i], "-c") == 0){
			if(mode == BML_MODE_UNKNOWN){
				mode = BML_MODE_COMPRESS;
			}else{
				printf("Error: It is possible to use only one option out of two '-c' or '-d'\n");
				return -3;
			}
		}else if(strcmp(argv[i], "-d") == 0){
			if(mode == BML_MODE_UNKNOWN){
				mode = BML_MODE_DECOMPRESS;
			}else{
				printf("Error: It is possible to use only one option out of two '-c' or '-d'\n");
				return -4;
			}
		}else if(strcmp(argv[i], "-s") == 0){
			if(maskInd == 0){
				maskInd = ++i;
				if(maskInd >= argc){
					printf("Error: No mask was specified for the '-s' argument\n");
					return -9;
				}
			}else{
				printf("Error: Multiple uses of the '-s' argument\n");
				return -2;
			}
		}else{
			printf("Error: Unknown argument '%s'\n", argv[i]);
			return -10;
		}
	}

	if(mode == BML_MODE_UNKNOWN){
		printf("Error: Must be one of the '-c' or '-d' options\n");
		return -4;
	}else if(inFilenameInd == 0){
		printf("Error: Input filename not specified.\n");
		return -5;
	}else if(outFilenameInd == 0){
		printf("Error: Output filename not specified.\n");
		return -6;
	}


	uint algoSize = 3;
	BML_ALGO algoSeq[BML_ALGO_MAX_SIZE];
	algoSeq[0] = BML_ALGO_BWT;
	algoSeq[1] = BML_ALGO_MTF;
	algoSeq[2] = BML_ALGO_LZW;


	if(maskInd != 0){
		algoSize = bmlMaskToArray(argv[maskInd], algoSeq);
	}

	
	if(mode == BML_MODE_COMPRESS){
		bmlEncode(argv[inFilenameInd], argv[outFilenameInd], algoSeq, algoSize);
	}else if(mode == BML_MODE_DECOMPRESS){
		bmlDecode(argv[inFilenameInd], argv[outFilenameInd], algoSeq, algoSize);
	}else{
		printf("Error: Unknown error.\n");
		return -7;
	}
	


	return 0;
}
