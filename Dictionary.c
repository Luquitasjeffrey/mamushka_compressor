//Dictionary creation
#include <math.h>
#include "LZ77.c"
#include "HashMap.c"
#define TXTPTRVALUE 0x0100
#define NULLCHAR 0xffff



DATA high_score(RAD(*arr)[], uint32_t strsize, uint16_t(*txt)[], uint32_t netrest){
	DATA output;
	output.appereances=0;
	output.score=0;
	
	double _map(DATA data){
		uint32_t arrpos;
		if(load_data(data.chars[0]+data.chars[1]*0x10000, arr, &data, strsize, &arrpos)){
			data.score++;
			(*arr)[arrpos].data.score=data.score;
		}
		else{
			data.score=1;
			store_data(data, strsize, arr);
		}
		return data.score;
	}
	
	void sethighscore(uint16_t candidate[2]){
		if(candidate[0]==NULLCHAR || candidate[1]==NULLCHAR){
			return;
		}
		DATA data;
		data.chars[0]=candidate[0];
		data.chars[1]=candidate[1];
		data.score=_map(data);
		if(data.score>output.score){
			output=data;
		}	
	}
	
	//main loop
	setup(arr, strsize);
	
	//primera pasada para evitar un if(>0) checkifnotequal();
	uint16_t candidate[2];
	candidate[0]=(*txt)[0];
	candidate[1]=(*txt)[1];
	sethighscore(candidate);
	
	uint32_t i=0;
	uint16_t CHECK[2];
	
	bool _notequal(uint16_t str1[2], uint16_t str2[2]){
		return !(str1[0]==str2[0] && str1[1]==str2[1]);
	}
	
	uint16_t check2[]={0x7777, 0x7777}; //no existen esos numeros en el string por lo que el primer notequal me va a dar true
	
	for(; i<strsize-1; i++){
		CHECK[0]=candidate[0];
		CHECK[1]=candidate[1];
		candidate[0]=(*txt)[i];
		candidate[1]=(*txt)[i+1];
		
		if(_notequal(CHECK, candidate)){
			sethighscore(candidate);
		}
		else if(!_notequal(check2, CHECK)){
			sethighscore(candidate);
		}
		check2[0]=CHECK[0];
		check2[1]=CHECK[1];
	}
	
	/*
	score=average length per symbol(alxs)
	K=symbol declaration constant
	h=huffman encoding length
	len=sequence length
	size=messagesize
	n=appereances in the message
	
	h=log2(((size-n*(len-1))/n)
	alxs=(K+h+len)/n + h/len
	*/
	
	//output.score=n --> output.score=alxs
	//k=2
	//len=2
	if(output.score==0){
		output.appereances=0xffff;
		output.score=0xffff;
	}
	strsize-=netrest;
	output.appereances=output.score;
	//printf("%u", output.appereances);
	output.score=(24+(log2((strsize-output.score)/output.score)))/output.score + log2((strsize-output.score)/output.score)/2;
	
	return output;	
}

typedef struct chars{
	uint16_t character;
	uint32_t appereance;
	double alxs;
}chars;

typedef struct CHARS{
	chars CHARS[260];
}CHARS;

chars _ReturnBest(CHARS (*imp_data), uint16_t(*txt)[], uint32_t txtsize, uint32_t netrest){
	
	chars rvalue={0,0,0};
	
	//setting to zero
	uint16_t i=0;
	for(; i<260; i++){
		(*imp_data).CHARS[i].character=i;
		(*imp_data).CHARS[i].appereance=0;
	}
	(*imp_data).CHARS[256].character=TXTPTR;
	
	
	uint32_t tst=0;
	
	uint32_t txtpos=0;
	rvalue.appereance=0;
	for(; txtpos<txtsize; txtpos++){
		
		if((*txt)[txtpos]!=NULLCHAR){
			uint32_t arrpos=((*txt)[txtpos]==TXTPTR)? TXTPTRVALUE : (*txt)[txtpos];
			if(arrpos>256){
				printf(" error {%u, %u}", arrpos, txtpos);
				continue;
			}
			(*imp_data).CHARS[arrpos].appereance++;
			if((*imp_data).CHARS[arrpos].appereance>rvalue.appereance){
				rvalue=(*imp_data).CHARS[arrpos];
				tst=txtpos;
			}
		}
	}
	
	
	//printf("<%x, %u, %x, %u>", rvalue.character, tst, (*txt)[tst],rvalue.appereance /*(*imp_data).CHARS[0].appereance*/);
	txtsize-=netrest;
	rvalue.alxs=rvalue.appereance;
	rvalue.alxs=((16+log2((txtsize/rvalue.alxs)))/rvalue.alxs)+log2((txtsize/rvalue.alxs));
	return rvalue;
}


