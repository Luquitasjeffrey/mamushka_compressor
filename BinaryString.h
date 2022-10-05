#include "ProgramStructs.h"

#pragma once
uint32_t numberofzeros=0;
uint32_t numberofones=0;

void _Init(BinaryString *strptr){
	strptr->binarypos=0;
	uint32_t i=0;
	for(; i<0x4000000; i++){
		strptr->str[i]=0;
	}
}

uint8_t _2ex(uint8_t x){
	uint8_t i=0;
	uint8_t retrn=1;
	for(; i<x; i++){
		retrn*=2;
	}
	
	return retrn;
}

void AddBit(BinaryString *strptr, uint8_t bit){
	//Power=strptr->binarypos%8;
	strptr->str[strptr->binarypos/8]+=(bit*_2ex(strptr->binarypos%8));
	strptr->binarypos+=1;
	
	//this step is just for fun
	if(bit==1){
		numberofones+=1;
		return;
	}
	numberofzeros+=1;
}

void addnumber(BinaryString *str, uint64_t add, uint8_t binarylength){
	
	uint8_t bitarr[binarylength];
	int i=0;
	for(; i<binarylength; i++){
		bitarr[i]=add%2;
		add/=2;
	}
	i--;
	for(; i>=0; i--){
		AddBit(str, bitarr[i]);
	}
}

uint8_t GetBit(BinaryString *read_info){
	
	uint8_t chr=read_info->str[(read_info->binarypos)/8];
	uint8_t bit=read_info->binarypos%8;
	
	uint8_t i=0;
	for(; i<bit; i++){
		chr/=2;
	}
	read_info->binarypos+=1;
	return chr%2;
}

