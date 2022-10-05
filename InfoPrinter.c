#include <stdio.h>
#include <stdlib.h>
#include "InitTree.c"
#ifndef InfoPrinter_C_
#define InfoPrinter_C_

void Non_compressed_data(char input[65536], CHARLIST list){
	int k=0;
	int n=list.size-1;
	while(n>1){
		n/=2;
		k++;
	}
	int i=0;
	printf("\nthis is the non-compressed data:\ndictionary:");
	for(; i<list.size; i++){
		printf("\n%u %u=", list.chars[i].character[0], list.chars[i].character[1]);
		int j=0;
		int p=i;
		for(; j<=k; j++){
			printf("%i", p%2);
			p/=2;
		}
	}
	/*
	printf("\nplaintext:\n");
	double size=0;
	i=0;
	while(input[i]!=0){
		int j=0;
		while(input[i]!=list.chars[j].character){
			j++;
		}
		int p=0;
		for(; p<=k; p++){
			size+=0.125;
			printf("%i", j%2);
			j/=2;
		}
		i++;
	}
	printf("\nthe size of non compressed text is: %.2f bytes\n", size);
	*/
}

void Compressed_data(CHARLIST list, uint16_t text[65536]){
	
	int obtain_huffman_limit(unsigned int addr){
		int i=0;
		while(list.chars[addr].huffmancode[i].h_int!=-1){
			i++;
		}
		return i-1;
	}
	
	int obtain_char_addr(uint16_t character){
		int i=0;
		while(character!=list.chars[i].character){
			i++;
		}
		return i;
	}
	
	double PrintHuffman(int addr){
		double n=0;
		int i=obtain_huffman_limit(addr);
		for(; i>=0; i--){
			n+=0.125;
			printf("%i", list.chars[addr].huffmancode[i].h_int);
		}
		return n;
	}
	
	
	printf("\nthis is the compressed data:\ndictionary:\n");
	int j=0;
	for(; j<list.size; j++){
		printf("\n%u %u=", list.chars[j].character[0], list.chars[j].character[1]);
		PrintHuffman(j);
	}
	
	printf("\ncompressed text:\n");
	double size=0;
	j=0;
	while(text[j]!=0){
		size+=PrintHuffman(obtain_char_addr(text[j]));
		j++;
	}
	printf("\nthe size of compressed text is: %.2f bytes\n\n", size);
}


#endif
