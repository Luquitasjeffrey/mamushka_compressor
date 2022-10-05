#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define VOIDTXT 0xffff
#define TXTEND 0xff00
#define TXTPTR 0x7fff


#pragma once

static inline bool searchingbuffer_2(uint16_t(*textbuffer)[], uint32_t txtposition, uint8_t *wordsize, uint16_t *backjmp, uint32_t txtsize){
	
	
	
	uint32_t i=(txtposition>32768)? txtposition-32768 : 0;
	//printf("%u\n", i);
	
	inline uint8_t match(){
		
		int j=0;
		while(
			(*textbuffer)[j+txtposition]==(*textbuffer)[i+(j%(txtposition-i))] &&
			j<7
		){
			
			j++;
		}
		return j;
	}
	
	uint8_t wrdsize=5;
	uint32_t buffpos;
	
	for(; i<txtposition; i++){
		uint32_t nwrdsize=match();
		if(nwrdsize>wrdsize){
			buffpos=i;
			wrdsize=nwrdsize;
			
			if(wrdsize==7){
				//printf("<<%u>>\n", txtposition+wrdsize);
				break;
			}
			
		}
	}
	
	if(wrdsize>5 && (txtposition+wrdsize)<=txtsize){
		*wordsize=wrdsize;
		*backjmp=(txtposition-buffpos)-1;
		return 1;
	}
	
	return 0;
}



void LZ77_COMPRESSOR_2(uint16_t(*buffer)[], uint32_t tsize, uint32_t *finalsize, bool mode){
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
	//uint32_t nextref=0;
	uint32_t nextprintable=0;
	//main loop
	text[0]=(*buffer)[0];
	for(i=0; i<tsize; i++){
		uint16_t backptr;
		uint8_t  wrdsize;
		text[i+1-netrest]=(*buffer)[i+1];
		if(searchingbuffer_2(buffer, i+1, &wrdsize, &backptr, tsize)  && (*buffer)[i]!=TXTPTR){
			
			//nextref++;
			//printf("<ref %u, %u, %u>", i, wrdsize, backptr);
			cpresstxtsize-=wrdsize-5;
			//l_shift(&text, tsize, i+1-netrest, wrdsize-3);
			text[i+1-netrest]=TXTPTR;
			text[i+2-netrest]=TXTPTR;
			text[i+3-netrest]=TXTPTR;
			text[i+4-netrest]=backptr/128;
			text[i+5-netrest]=(2*(backptr%128)+(wrdsize-6));
			netrest+=wrdsize-5;
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
	//printf("\ntotal %u refs\n", nextref);
}


void LZ77_COMPRESS_2(uint16_t(*buffer)[], uint32_t tsize, uint32_t *finalsize){
	
	printf("\ncompressing...\n");
	
	
	LZ77_COMPRESSOR_2(buffer, tsize, finalsize, 1);
}


void LZ77EXTRACT_2(uint16_t (*output)[], uint16_t (*comp_data)[], uint32_t cdatasize, uint32_t *finalsize){
	uint32_t txtpos=0;
	uint32_t cdata=0;
	
	//uint32_t nextref=0;
	inline void extractword(uint16_t txtptr, uint8_t wrdsize){
		//r_shift(output, txtpos, wrdsize, txtpos+wrdsize+1);
		uint32_t copy=0;
		for(; copy<=wrdsize; copy++){
			(*output)[copy+txtpos]=(*output)[copy+txtpos-txtptr];
		}
		txtpos+=wrdsize;	
	}
	while(cdata<cdatasize){
		if((*comp_data)[cdata]==TXTPTR &&
		 (*comp_data)[cdata+1]==TXTPTR &&
		 (*comp_data)[cdata+2]==TXTPTR){
			//printf("ref");
			while((*comp_data)[cdata+3]==TXTPTR){
				(*output)[txtpos]=(*comp_data)[cdata];
				txtpos++;
				cdata++;
				//printf("ref(ref())");
			}
			
			/*printf("ref<%u, %u, %u>", cdata,
			(*comp_data)[cdata+4]%2+5, 
			(*comp_data)[cdata+3]*128+(*comp_data)[cdata+4]/2+1);
			*/
			//nextref++;
			extractword(
			(*comp_data)[cdata+3]*128+(*comp_data)[cdata+4]/2+1, 
			(*comp_data)[cdata+4]%2+5
			);
			cdata+=5;
		}
		else{
			(*output)[txtpos]=(*comp_data)[cdata];
			cdata++;
		}
		txtpos+=1;
	}
	*finalsize=txtpos;
	//printf("total %u refs\n", nextref);
}

