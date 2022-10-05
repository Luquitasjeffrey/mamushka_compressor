#include "FileSystem_o.c"
#include "MakeTree.c"
#include "FileWriter.c"
//#include "FileSystem_i.c"
//#include "mamushka.c"


void copytxt(uint16_t (*origin)[], uint16_t (*destination)[], uint32_t size){
	uint32_t i=0;
	for(; i<size; i++){
		(*destination)[i]=(*origin)[i];
	}
}

void decompress_directory(const char *inputname, const char* outputfilename){
	uint16_t txt[0x4000000];
	uint32_t size;
	makeall(&txt, 0x4000000, inputname, &size);

	printf("\nhuffman complete\n");

	uint8_t extract[0x4000000];
	memset(extract, 0, 0x4000000);

    mamushka_dec(&extract, &txt, size, &size);

	printf("\nafter mamushka size is: %u\n", size);

	CopyInDirectories(&extract, size, outputfilename);
}

void compress(char * ifilename, char *ofilename, int oflags){
	uint16_t input[0x4000000];
	memset(input,0,0x4000000);
		
	uint32_t i=0;
	
	//printf("size of input dir=%u\n", sizeof(input));
	StoreAll(ifilename, &input, 0x4000000, &i, ifilename);
	
	//copyfile("jcc.exe", &input, i, 0x1000000, &i);
	printf("\nNon compressed size: %u bytes", i);
	
	
	uint32_t fsize;
	
	mamushka_compress(&input, i, &fsize, oflags);
	
	printf("\nAfter LZ77 size=%u bytes\n", fsize);
	
	CHARLIST HuffmannTree;
	
	
	uint16_t floyd[fsize];
	copytxt(&input, &floyd, fsize+1);
	initialize_charlist(initialize_temporal_charlist(&input, fsize), &HuffmannTree);
	
	HuffmannTree=maketree(HuffmannTree);	
	//printf("\n%u", HuffmannTree.size);
	printf("\n\n");
	StoreCompFile(ofilename, &floyd, &HuffmannTree, fsize);
	
	
	printf("\n\n");
}
