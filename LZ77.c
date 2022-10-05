#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define VOIDTXT 0xffff
#define TXTEND 0xff00
#define TXTPTR 0x7fff


#pragma once

static inline bool searchingbuffer(uint16_t(*textbuffer)[], uint32_t txtposition, uint8_t *wordsize, uint16_t *backjmp, uint32_t txtsize){
		
	uint32_t i=(txtposition>4096)? txtposition-4096 : 0;
	//printf("%u\n", i);
	
	inline uint8_t match(){
		
		int j=0;
		while((*textbuffer)[j+txtposition]==(*textbuffer)[i+(j%(txtposition-i))] && //aca mi codigo va a pasar la mayor parte del tiempo jaja
		j<19){
			j++;
		}
		return j;
	}
	
	uint8_t wrdsize=3;
	uint32_t buffpos;
	
	for(; i<txtposition; i++){
		uint32_t nwrdsize=match();
		if(nwrdsize>wrdsize){
			buffpos=i;
			wrdsize=nwrdsize;
			
			if(wrdsize==19){
				//printf("<<%u>>\n", txtposition+wrdsize);
				break;
			}
			
		}
	}
	
	if(wrdsize>3 && (txtposition+wrdsize)<=txtsize){
		*wordsize=wrdsize;
		*backjmp=(txtposition-buffpos)-1;
		return 1;
	}
	
	return 0;
}

/*
//aca esta el problema, esta funcion me hace que mi codigo tenga una complejidad de n^2
void l_shift(uint16_t(*buffer)[], uint32_t buffersize, uint32_t since, uint32_t quantity){
	for(; since<=(buffersize-quantity); since++){
		(*buffer)[since]=(*buffer)[since+quantity];	
	}
	(*buffer)[buffersize-quantity]=TXTEND;
}
*/

void LZ77_COMPRESSOR(uint16_t(*buffer)[], uint32_t tsize, uint32_t *finalsize, bool mode){
	uint16_t text[tsize+1];
	uint32_t cpresstxtsize=tsize;
	uint32_t netrest=0;
	uint32_t i=0;
	
	/*
	for(; i<tsize; i++){
		text[i]=(*buffer)[i];
	}
	*/
	text[tsize]=VOIDTXT;
	
	uint32_t nextprintable=0;
	//main loop
	text[0]=(*buffer)[0];
	for(i=0; i<tsize; i++){
		uint16_t backptr;
		uint8_t  wrdsize;
		text[i+1-netrest]=(*buffer)[i+1];
		if(searchingbuffer(buffer, i+1, &wrdsize, &backptr, tsize)){
			if(backptr<4096){
			//printf("<ref>");
			cpresstxtsize-=wrdsize-3;
			//l_shift(&text, tsize, i+1-netrest, wrdsize-3);
			text[i+1-netrest]=TXTPTR;
			text[i+2-netrest]=backptr/16;
			text[i+3-netrest]=(16*(backptr%16)+(wrdsize-4));
			netrest+=wrdsize-3;
			uint32_t *up=&i;
			*up+=wrdsize-1;
			}
			
			if(i>nextprintable){
			nextprintable+=50000;
			float percentage=i;
			percentage/=tsize;
			printf("\r%f%% completed", percentage*100);
			}	
		}
		
	}
	text[i-netrest]=VOIDTXT;
	*finalsize=cpresstxtsize;
	
	if(mode==1){
		uint32_t copyarr=0;
		for(; copyarr<cpresstxtsize; copyarr++){
			(*buffer)[copyarr]=text[copyarr];
		}
		(*buffer)[cpresstxtsize]=VOIDTXT;
	}
}

/*
void r_shift(uint8_t (*buffer)[], uint32_t since, uint32_t quantity, uint32_t bufsize){
	bufsize+=1;
	for(; bufsize>since; bufsize--){
		(*buffer)[bufsize+quantity-1]=(*buffer)[bufsize-1];
	}	
}

void debug(uint16_t(*buffer)[]){
	uint32_t debug=0;
	printf("\n\n");
	while((*buffer)[debug]!=VOIDTXT){
		if((*buffer)[debug]==TXTPTR){
			printf("<%u,%u>", (*buffer)[debug+1]*16+(*buffer)[debug+2]/16, (*buffer)[debug+2]%16+4);
			debug+=3;
		}
		else{
			printf("%c", (*buffer)[debug]);
			debug++;
		}
	}
}
*/

void LZ77_COMPRESS(uint16_t(*buffer)[], uint32_t tsize, uint32_t *finalsize){
	
	printf("\ncompressing...\n");
	
	
	LZ77_COMPRESSOR(buffer, tsize, finalsize, 1);
}


void LZ77EXTRACT(uint8_t (*output)[], uint16_t (*comp_data)[], uint32_t cdatasize, uint32_t *finalsize){
	uint32_t txtpos=0;
	uint32_t cdata=0;
	
	inline void extractword(uint16_t txtptr, uint8_t wrdsize){
		//r_shift(output, txtpos, wrdsize, txtpos+wrdsize+1);
		uint32_t copy=0;
		for(; copy<=wrdsize; copy++){
			(*output)[copy+txtpos]=(*output)[copy+txtpos-txtptr];
		}
		txtpos+=wrdsize;	
	}
	
	while(cdata<cdatasize){
		if((*comp_data)[cdata]==TXTPTR){
			//printf("ref");
			
			extractword(
			(*comp_data)[cdata+1]*16+(*comp_data)[cdata+2]/16+1, 
			(*comp_data)[cdata+2]%16+3
			);
			cdata+=3;
		}
		else{
			(*output)[txtpos]=(*comp_data)[cdata];
			cdata++;
		}
		txtpos+=1;
	}
	*finalsize=txtpos;
}
