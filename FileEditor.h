//editor de archivos.h
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "ProgramStructs.h"
#include "BinaryString.h"

#define NEWTXTPTR 0b111111111
#define C256      0b111111110   

#define NULLCHAR 0xffff


void PrintHuffmanCode(HCODE huffmancode[32]){
	int j=0;
	while(huffmancode[j].h_int!=-1){
		j++;
	}
	j--;
	
	for(; j>=0; j--){
		printf("%u", huffmancode[j].h_int);
	}
}

uint64_t MakeIntFromHcode(HCODE huffmancode[32], uint32_t *finallength, uint32_t *start){
	uint32_t i=0;
	uint64_t retrn=0;
	uint64_t base=1;
	while(huffmancode[i].h_int!=-1){
		if(huffmancode[i].h_int==1){
			retrn+=base;
		}
		
		base*=2;
		i++;
	}
	*finallength=i;
	base=1;
	i=0;
	while(base<=retrn){
		base*=2;
		i++;
	}
	*start=(*finallength)-i;
	return retrn;
}

void PrintCharlist(CHARLIST *list){
	uint16_t i=0;
	
	for(; i<list->size; i++){
		uint32_t ptr;
		uint32_t start;
		if(list->chars[i].character[0]==NULLCHAR){
			printf("\n(%x)=", list->chars[i].character[1]);
			PrintHuffmanCode(list->chars[i].huffmancode);
			printf("	{%llu, ", MakeIntFromHcode(list->chars[i].huffmancode, &ptr, &start));
			printf("%u, %u}", ptr, start);
		}
		else{
			printf("\n(%x, %x)=", list->chars[i].character[0], list->chars[i].character[1]);
			PrintHuffmanCode(list->chars[i].huffmancode);
			printf("	{%llu, ", MakeIntFromHcode(list->chars[i].huffmancode, &ptr, &start));
			printf("%u, %u}", ptr, start);
		}
		
	}
	
}

uint16_t _find(CHARLIST *list, uint16_t find[2], uint8_t *plusequal){
	
	uint32_t i=0;
	for(; i<list->size; i++){
		if(find[0]==list->chars[i].character[0] &&
		find[1]==list->chars[i].character[1]){
			*plusequal=2;
			return i;
		}
	}
	
	for(i=0; i<list->size; i++){
		if(find[0]==list->chars[i].character[1]){
			*plusequal=1;
			return i;
		}
	}
	printf("not found");
}

void PrintFileInConsole(CHARLIST *list, uint16_t (*txt)[], uint32_t txtsize){
	
	uint32_t i=0;
	
	uint8_t plusequal=0;
	uint16_t find[2];
	uint16_t character;
	for(; i<txtsize-1; i+=plusequal){
		find[0]=(*txt)[i];
		find[1]=(*txt)[i+1];
		
		character=_find(list, find, &plusequal);
		//printf("(%u)", character);
		PrintHuffmanCode(list->chars[character].huffmancode);
	}
	
	if(i<txtsize){
		find[0]=(*txt)[txtsize-1];
		find[1]=0xffff;
		
		character=_find(list, find, &plusequal);
		PrintHuffmanCode(list->chars[character].huffmancode);
	}
	
}

uint8_t HLen(HCODE huffmancode[32]){
	uint8_t r=0;
	while(huffmancode[r].h_int!=-1){
		r++;
	}
	return r-1;
}

uint8_t SetStart(uint8_t n){
	uint8_t i=0;
	uint8_t base=1;
	while(base<=n){
		base*=2;
		i++;
	}
	return 5-i;
}

void CompileDictionary(CHARLIST *list, BinaryString *jcout){
	
	//detras de cada delcaracion hay 6 bits que dicen: 1er bit: 0=contiene un solo caracter. 1=contiene 2 caracteres. 
	//los 5 restantes indican la longitud del codificado huffmann.
	//la longitud real es 1+ la longitud declarada. esto es asi ya que me permite almacenar hasta 32 cuando si no podria almacenar hsta 31
	
	uint32_t i=0;
	uint64_t hnumber;
	uint32_t start;
	uint32_t netlen;
	
	//lo primero que se vera sera la longitud del diccionario.
	//por una cuestion meramente arbitraria voy a usar 12 bits.
	//al igual que antes le resto 1 para poder almacenar un valor mas, ya que ninguno tendra longitud cero.
	
	//tendria que poder luego interpretarse de forma totalmente desmambigua (en teoría).
	//printf("xd");
	addnumber(jcout, list->size-1, 12);
	
	//main loop
	for(; i<list->size; i++){
		hnumber=MakeIntFromHcode(list->chars[i].huffmancode, &netlen, &start);
		
		if(list->chars[i].character[0]==NULLCHAR){
			AddBit(jcout, 0);
			
			addnumber(jcout, netlen-1, 5);
			if(list->chars[i].character[1]==TXTPTR){
				addnumber(jcout, NEWTXTPTR, 9);
			}
			else if(list->chars[i].character[1]==255){
				addnumber(jcout, C256, 9);
			}
			else{
				addnumber(jcout, list->chars[i].character[1], 8);
			}
			addnumber(jcout, hnumber, netlen);
		}
		else{
			AddBit(jcout, 1);
			
			addnumber(jcout, netlen-1, 5);
			uint8_t j=0;
			for(; j<=1; j++){
				if(list->chars[i].character[j]==TXTPTR){
					addnumber(jcout, NEWTXTPTR, 9);
				}
				else if(list->chars[i].character[j]==255){
					addnumber(jcout, C256, 9);
				}
				else{
					addnumber(jcout, list->chars[i].character[j], 8);
				}
			}
			addnumber(jcout, hnumber, netlen);
		}
		
	}//main for loop
	
}



void StoreCompFile(const char* filename, uint16_t (*txt)[], CHARLIST *list, uint32_t txtsize){
		
	BinaryString jcout;
	_Init(&jcout);
	//al inicio añadire un numero que indicara la longitud total del archivo en bits.
	//el mismo tendra un peso de 32 bits(sería suficiente para almacenar un archivo de 512 mb, mas que sobra para nuestros propositos
	CompileDictionary(list, &jcout);
	printf("size of dict=%u\n", jcout.binarypos);
	uint32_t i=0;
	uint16_t find[2];
	uint8_t plusequal;
	while(i<txtsize-1){
		find[0]=(*txt)[i];
		find[1]=(*txt)[i+1];
		uint16_t huff=_find(list, find, &plusequal);
		i+=plusequal;
		
		uint32_t nlen;
		uint32_t start;
		uint64_t prt=MakeIntFromHcode(list->chars[huff].huffmancode, &nlen, &start);
		addnumber(&jcout, prt, nlen);
	}
	if(i<txtsize){
		find[0]=NULLCHAR;
		find[1]=(*txt)[i];
		uint16_t huff=_find(list, find, &plusequal);
		
		uint32_t nlen;
		uint32_t start;
		uint64_t prt=MakeIntFromHcode(list->chars[huff].huffmancode, &nlen, &start);
		addnumber(&jcout, prt, nlen);
	}
	i=jcout.binarypos; //main storage in ram loop
	jcout.bsize=i;
	//es necesario poner el tamaño del archivo en BITS, no bytes.
	uint32_t binarysize=i;
	//storage in HD
	uint8_t filesize[4];
	uint32_t j=0;
	for(; j<4; j++){
		filesize[3-j]=jcout.binarypos%256;
		jcout.binarypos/=256;
	}
	i=(i%8==0)? i/8 : 1+i/8;
	
	FILE *fpointer=fopen(filename, "wb");
	for(j=0; j<4; j++){
		
		//printf("(%u)\n", filesize[j]);
		putc(filesize[j], fpointer);
	}
	for(j=0; j<i; j++){
		//printf("<%u>%u, ", j,jcout.str[j]);
		int pok=putc(jcout.str[j], fpointer);
		
		/*if(pok==-1){
			printf("error in putc");
		}
		*/
	}
	
	printf("\n\nNet size in bits is: %u\nnet zeros=%u\nnet ones=%u", binarysize, numberofzeros, numberofones);
	/*	
		formato:
		-comp_size en BITS
		-diccionario huffman
		-comp txt con LZ77 y luego con huffman
	*/	
	
}



