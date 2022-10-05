#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "BinaryString.h"
#include "LZ77.c"

#define TXTPTR 0x7fff
#define NULLCHAR 0xffff

void ReadFile(const char* filename, BinaryString *copy){
	
	FILE *file=fopen(filename, "rb");
	uint32_t filesize=0;
	uint32_t base=0x1000000;
	
	uint32_t i=0;
	for(; i<4; i++){
		
		uint32_t add=getc(file);
		filesize+=base*add;
		base/=256;
	}
	copy->bsize=filesize;
	
	_Init(copy);
	filesize=(filesize%8==0)? filesize/8 : 1+filesize/8;
	
	for(i=0; i<filesize; i++){
		copy->str[i]=fgetc(file);
	}
}

typedef struct HuffmanChars{
	uint16_t chars[2];
}HuffmanChars;

typedef struct BinaryTree{
	
	bool exist0;
	struct BinaryTree *_0ramif;
	
	bool exist1;
	struct BinaryTree *_1ramif;
	
	bool END;
	HuffmanChars *character;
	
}BinaryTree;





void makeall(uint16_t (*txt)[], uint32_t txtsize, const char* filename, uint32_t *fsize){
	//printf("infinite loop...");	
	BinaryString copy;
	ReadFile(filename, &copy);
	
	
	
	uint32_t i=0;
	uint32_t add=2048;
	
	uint16_t dicclen=1;
	for(; i<12; i++){
		if(GetBit(&copy)){
			dicclen+=add;
		}
		add/=2;
	}
	
	//printf("%u\n", dicclen);
	uint8_t getlen(){
		uint8_t retrn=1;
		uint8_t sum=16;
		uint8_t j=0;
		for(; j<5; j++){
			if(GetBit(&copy)){
				retrn+=sum;
			}
			sum/=2;
		}
		return retrn;
	}
	
	uint16_t _getc(){
		uint8_t retrn=0;
		uint8_t sum=128;
		uint8_t j=0;
		for(; j<8; j++){
			if(GetBit(&copy)){
				retrn+=sum;
			}
			sum/=2;
		}
		if(retrn!=255){
		return retrn;
		}
		if(GetBit(&copy)){
			return TXTPTR;
		}
		return 255;
	}
	
	BinaryTree TREE[dicclen*2];
	HuffmanChars chars[dicclen];
	
	
	//now setup TREE (bools in zero)
	i=0;
	for(; i<dicclen*2; i++){
		TREE[i].END=0;
		TREE[i].exist0=0;
		TREE[i]._0ramif=0;
		TREE[i].exist1=0;
	}
	
	uint16_t chararrpos=0;
	uint16_t nodecount=1;
	
	
	
	void AddChars(uint16_t addchars[2], uint8_t codelen){
		
		BinaryTree *search=&TREE[0]; //the "root" tree node
		
		//printf("inf loop");
		//printf(" %u, %u, %u\n", nodecount, codelen, copy.binarypos);
		
		void _AddBit(){
			if(GetBit(&copy)){
				if(search->exist1){
					search=search->_1ramif;
				}
				else{	
					search->exist1=1;
					search->_1ramif=&TREE[nodecount];
					search=search->_1ramif;
					nodecount++;
				}
			}
			else{
				if(search->exist0){
					search=search->_0ramif;
				}
				else{
					search->exist0=1;
					search->_0ramif=&TREE[nodecount];
					search=search->_0ramif;
					nodecount++;
				}
			}
		}
		
		uint8_t j=0;
		for(; j<codelen; j++){
			_AddBit();
		}
		chars[chararrpos].chars[0]=addchars[0];
		chars[chararrpos].chars[1]=addchars[1];
		search->END=1; //importante marcar que aca termina
		search->character=&chars[chararrpos];
		chararrpos++;
		
	}
	
	//ahora a completar el arbol.
	
	for(; chararrpos<dicclen; ){ //";" ya que se van sumando mientras añado mas al arbol
		//getlen() me devuelve la longitud del codigo huffman
		//_getc() me devuelve el caracter. puede devolver tambein el TXTPTR	
		//printf("(%u)\n", copy.binarypos);
		uint16_t chrs[2];
		uint8_t codelen;
		
		if(GetBit(&copy)){ //si es asi va a contener 2 caracteres, de otra forma solo contiene uno
			codelen=getlen();
			chrs[0]=_getc();
			chrs[1]=_getc();
			AddChars(chrs, codelen);
		}
		else{
			codelen=getlen();
			chrs[0]=NULLCHAR;
			chrs[1]=_getc();
			AddChars(chrs, codelen);
		}
		
	}
	//esto seria el loop que leeria el diccionario
	
	//printf("\nsize of dict is:%u\n", copy.binarypos);
	uint32_t printev=0;
	
	void _getchars(uint16_t (*chrs)[2]){
		
		BinaryTree *climb=&TREE[0]; //beggining at the root
		
		/*
		if(printev>=7122118){
			printf("\nentring getc %u %u\n", copy.binarypos, climb);
			printf("%u\n", GetBit(&copy));
			copy.binarypos--;
			printf("problem found %u?\n", climb->END);
		}
		*/
		while(!(climb->END)){
			
			
			
			if(GetBit(&copy)){
				climb=climb->_1ramif;	
			}
			else{
				climb=climb->_0ramif;
			}
			
			/*
			if(printev>=7122118){
				printf("\n%u, %u, %u, (%u)", climb->exist0, climb->exist1, climb->END, copy.binarypos);
			}
			*/
			//los tipos de datos recursivos son hermosos <3
		}
		
		(*chrs)[0]=climb->character->chars[0];
		(*chrs)[1]=climb->character->chars[1];
		/*
		if(copy.binarypos>7122110){
			printf(" end of getchar %u\n", copy.binarypos);
		}
		*/
	}
	
	memset(txt, 0, txtsize);
	uint32_t txtpos=0;
	uint16_t readedchars[2];
	
	//printf("H dec\n");
	//printf("%u\n", sizeof(BinaryString));
	for(; copy.binarypos<copy.bsize; ){
		_getchars(&readedchars);
		
		if(readedchars[0]==NULLCHAR){
			(*txt)[txtpos]=readedchars[1];
			txtpos++;
		}
		else{
			//printf("<%u, %x, %x>", txtpos, readedchars[0], readedchars[1]);
			(*txt)[txtpos]=readedchars[0];
			(*txt)[txtpos+1]=readedchars[1];
			txtpos+=2;
		}
		
		/*
		if(copy.binarypos>printev){
			printf("\rHuffman Dec (%u/%u), %u", copy.binarypos, copy.bsize, txtpos);
			printev=copy.binarypos;
			printev+=100000;
		}
		*/
	}
	
	*fsize=txtpos;
	//printf("\n%u\n", txtpos);
	//printf("\n%u\n", nodecount);
}

/*
int main(){
	
	
	uint32_t size=0x1000000;
	uint16_t txt[size];
	makeall(&txt, size, "main.jcc", &size);
	
	
	
	/*
	uint32_t i=0;
	for(; i<size; i++){
		printf("(%u, i)", txt[i], i);
	}
	
	//debug(&txt);
	uint32_t arrsize=size;
	uint8_t inflate[50000];
	LZ77EXTRACT(&inflate, &txt, arrsize);
	
	printf("%s\n\n", inflate);
	
	system("pause");
	return 0;
}
*/
