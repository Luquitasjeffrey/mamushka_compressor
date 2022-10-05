#include "LZ77.c"
#include "LZ77_1.c"
#include "LZ77_2.c"
#include "LZ77_3.c"

void mamushka_compress(uint16_t(*cstring)[], uint32_t size, uint32_t *fsize, int oflags){
	/*
	oflags:
	-1: stands for very fast compression and access (ratio=~=80%)
	1: standard compression (compression ratio =~=50%). not very fast but acceptable. similar to zip files.
	2: a little more compression than 1 at the expense of less velocity than 1
	3: a little more compression than 2 at the expense of less velocity than 2
	0: standss for use de minimal memory possible. slow execution, highest compression ratio.
	*/
	if(oflags==-1){
		return; 
	}
	int i=0;
	LZ77_COMPRESS(cstring, size, fsize);
	printf("\nsize after phase 1: %u\n", *fsize);
	i++;
	if(oflags==i){
		return;
	}
	LZ77_COMPRESS_1(cstring, *fsize, fsize);
	printf("\nsize after phase 2: %u\n", *fsize);
	i++;
	if(oflags==i){
		return;
	}
	LZ77_COMPRESS_2(cstring, *fsize, fsize);
	printf("\nsize after phase 3: %u\n", *fsize);
	i++;
	if(oflags==i){
		return;
	}
	LZ77_COMPRESS_3(cstring, *fsize, fsize);
	printf("\nsize after phase 4: %u\n", *fsize);
	i++;
	if(oflags==i){
		return;
	}
}

void mamushka_dec(uint8_t(*output)[], uint16_t(*comp_data)[], uint32_t cdatasize, uint32_t *fsize){
	
	uint16_t aux[0x1000000];
	
	LZ77EXTRACT_3(&aux, comp_data, cdatasize, fsize);
	LZ77EXTRACT_2(comp_data, &aux, *fsize, fsize);
	LZ77EXTRACT_1(&aux, comp_data, *fsize, fsize);
	LZ77EXTRACT(output, &aux, *fsize, fsize);
}
