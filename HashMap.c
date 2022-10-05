#include <stdint.h>
#include <stdbool.h>


typedef struct DATA{
	uint16_t chars[2];
	uint32_t appereances;
	double score;
}DATA;

typedef struct random_access_data{
	bool exist;
	DATA data;
	uint32_t id;
}RAD;


void setup(RAD (*datapointer)[], uint32_t size){
	uint32_t setzero=0;
	for(; setzero<=size; setzero++){
		(*datapointer)[setzero].data.appereances=0;
		(*datapointer)[setzero].exist=0;	
	}
}

static inline bool store_data(DATA data, uint32_t size, RAD(*datapointer)[]){
	uint32_t ID=data.chars[1]*0x10000+data.chars[0];
	uint32_t select_node=ID%size;
	uint32_t pcount=0;
	
	while((*datapointer)[select_node].exist==1){
		
		if(
		(*datapointer)[select_node].exist==0 ||
		(*datapointer)[select_node].id==ID
		){
			(*datapointer)[select_node].id=ID;
			(*datapointer)[select_node].data=data;
			return 1;
		}
		
		if(pcount>size){
			return 0;
		}
		
		pcount++;	
		select_node++;
		select_node%=size;
	}	
	
	(*datapointer)[select_node].id=ID;
	(*datapointer)[select_node].data=data;
	(*datapointer)[select_node].exist=1;
	return 1;
}

static inline bool load_data(uint32_t ID, RAD(*datapointer)[], DATA *loaded, uint32_t size, uint32_t* arrpos){
	uint32_t select_node=ID%size;
	uint32_t pcount=0;
	while((*datapointer)[select_node].exist==1){
		
		if(
		(*datapointer)[select_node].id==ID
		){
			*arrpos=select_node;	
			*loaded=(*datapointer)[select_node].data;
			return 1;
		}
		
		if(pcount>size){
			return 0;
		}
		pcount++;
		select_node++;
		select_node%=size;
	}
	
	return 0;
}

/*
int main(){
	uint16_t size=10;
	RAD info[size];
	setup(&info, size);

	DATA data;
	
	for(;;){
	scanf("%s", &data.chars);
	scanf("%llu", &data.score);
	store_data(data, size, &info);
	uint16_t memoryaddr;
	scanf("%s", &data.chars);
	if(load_data(data.chars[0]+data.chars[1]*0xffff, &info, &data, size, &memoryaddr)){
	
		printf("%s %u %llu\n", data.chars, memoryaddr, data.score);
	}
	else{
		printf("data not localized\n");
	}
	}
	
	return 0;
}
*/
