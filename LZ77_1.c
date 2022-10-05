#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define VOIDTXT 0xffff
#define TXTEND 0xff00
#define TXTPTR 0x7fff


#pragma once

static inline bool searchingbuffer_1(uint16_t(*textbuffer)[], uint32_t txtposition, uint8_t *wordsize, uint16_t *backjmp, uint32_t txtsize){
	
	
	
	uint32_t i=(txtposition>16384)? txtposition-16384 : 0;
	//printf("%u\n", i);
	
	inline uint8_t match(){
		
		int j=0;
		while(
			(*textbuffer)[j+txtposition]==(*textbuffer)[i+(j%(txtposition-i))] &&
			j<8
		){
			
			j++;
		}
		return j;
	}
	
	uint8_t wrdsize=4;
	uint32_t buffpos;
	
	for(; i<txtposition; i++){
		uint32_t nwrdsize=match();
		if(nwrdsize>wrdsize){
			buffpos=i;
			wrdsize=nwrdsize;
			
			if(wrdsize==8){
				//printf("<<%u>>\n", txtposition+wrdsize);
				break;
			}
			
		}
	}
	
	if(wrdsize>4 && (txtposition+wrdsize)<=txtsize){
		*wordsize=wrdsize;
		*backjmp=(txtposition-buffpos)-1;
		return 1;
	}
	
	return 0;
}



void LZ77_COMPRESSOR_1(uint16_t(*buffer)[], uint32_t tsize, uint32_t *finalsize, bool mode){
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
		if(searchingbuffer_1(buffer, i+1, &wrdsize, &backptr, tsize) && (*buffer)[i]!=TXTPTR){
			
			//printf("<ref %u, %u, %u>", i, wrdsize, backptr);
			cpresstxtsize-=wrdsize-4;
			//l_shift(&text, tsize, i+1-netrest, wrdsize-3);
			text[i+1-netrest]=TXTPTR;
			text[i+2-netrest]=TXTPTR;
			text[i+3-netrest]=backptr/64;
			text[i+4-netrest]=(4*(backptr%64)+(wrdsize-5));
			netrest+=wrdsize-4;
			uint32_t *up=&i;
			*up+=wrdsize-1;
			
			
			if(i>nextprintable){
			nextprintable+=1000;
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


void LZ77_COMPRESS_1(uint16_t(*buffer)[], uint32_t tsize, uint32_t *finalsize){
	
	printf("\ncompressing...\n");
	
	
	LZ77_COMPRESSOR_1(buffer, tsize, finalsize, 1);
}

uint32_t getarrsize_1(uint16_t (*buffer)[], uint32_t*cdata){
	uint32_t i=0;
	uint32_t size=0;
	uint32_t entries=0;
	while((*buffer)[i]!=VOIDTXT){
		if((*buffer)[i]==TXTPTR){
			size+=(*buffer)[i+2]%16+4;
			i+=3;
			entries++;
		}
		else{
		size++;
		i++;
		}
	}
	*cdata=i;
	return size;
}

void LZ77EXTRACT_1(uint16_t (*output)[], uint16_t (*comp_data)[], uint32_t cdatasize, uint32_t *finalsize){
	uint32_t txtpos=0;
	uint32_t cdata=0;
	
	void extractword(uint16_t txtptr, uint8_t wrdsize){
		//r_shift(output, txtpos, wrdsize, txtpos+wrdsize+1);
		uint32_t copy=0;
		for(; copy<=wrdsize; copy++){
			(*output)[copy+txtpos]=(*output)[copy+txtpos-txtptr];
		}
		txtpos+=wrdsize;	
	}
	
	while(cdata<cdatasize){
		if((*comp_data)[cdata]==TXTPTR && (*comp_data)[cdata+1]==TXTPTR){
			//printf("ref");
			if((*comp_data)[cdata+2]==TXTPTR){
				(*output)[txtpos]=(*comp_data)[cdata];
				txtpos++;
				cdata++;
			}
			extractword(
			(*comp_data)[cdata+2]*64+(*comp_data)[cdata+3]/4+1, 
			(*comp_data)[cdata+3]%4+4
			);
			cdata+=4;
		}
		else{
			(*output)[txtpos]=(*comp_data)[cdata];
			cdata++;
		}
		txtpos+=1;
	}
	*finalsize=txtpos;
}

