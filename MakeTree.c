//generate tree using sum of 2 smallest in the group (.c)
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "InitTree.c"

typedef struct intarray{
	unsigned int addr[4096];
}intarray;

typedef struct ImportantData{
	unsigned int size;	
	unsigned int RelativePosition;
}ImportantData;

typedef struct MAKETREE{
unsigned int NetSize;
ImportantData datarray[4096];
}MAKETREE;

CHARLIST maketree(CHARLIST list){
	MAKETREE tree;
	intarray listaddr;
	unsigned int newaddr;
	void __SetUp(){
		int i=0;
		tree.NetSize=list.size;
		for(; i<=4095; i++){
			tree.datarray[i].size=list.chars[i].occurence;
			tree.datarray[i].RelativePosition=i;
			list.chars[i].addr=i;
		}
	}
	//endof __setup()
	void __SetIntarray(){
		int i=0;
		for(; i<4096; i++){
			listaddr.addr[i]=list.chars[i].addr;
		}
	}
	//endof __SetIntarray
	bool __sorted(){
		int i=0;
		for(; i<tree.NetSize-1; i++){
			if(tree.datarray[i].size>tree.datarray[i+1].size){
				return 0;
			}
		}
		return 1;
	}
	//endof __sorted()
	void __sort(){
		while(!__sorted()){	
			int i=0;
			for(; i<tree.NetSize-1; i++){
				if(tree.datarray[i].size>tree.datarray[i+1].size){
					ImportantData carry=tree.datarray[i];
					tree.datarray[i]=tree.datarray[i+1];
					tree.datarray[i+1]=carry;
				}
			}
		}
	}
	//endof __sort()
	void __shift_array(){
		int i=0;
		for(; i<tree.NetSize; i++){
			tree.datarray[i]=tree.datarray[i+1];
		}
		tree.NetSize--;
	}
	//endof __shift_array()
	int __found(unsigned int addr, int since){
		while(since<list.size){
			if(list.chars[since].addr==addr){
				newaddr=since;
				//printf("\n%i=%i", addr, newaddr);
				return 1;
			}
			since++;
		}
		return 0;
	}
	//endof __found
	void __actualize(){
		newaddr=0;
		while(__found(0, newaddr)){
			int j=0;
			while(list.chars[newaddr].huffmancode[j].h_int!=-1 && j<32){
				j++;
			}
			list.chars[newaddr].huffmancode[j].h_int=0;
			newaddr++;
		}
		newaddr=0;
		while(__found(1, newaddr)){
			int j=0;
			tree.datarray[1].RelativePosition=0;
			list.chars[newaddr].addr=0;
			while(list.chars[newaddr].huffmancode[j].h_int!=-1 && j<32){
				j++;
			}
			list.chars[newaddr].huffmancode[j].h_int=1;
			newaddr++;		
		}
		__shift_array();
		__sort();
		__SetIntarray();
		int i=0;
		for(; i<tree.NetSize; i++){
			newaddr=0;
			while(newaddr<list.size){
			while(!__found(tree.datarray[i].RelativePosition, newaddr)&& newaddr<list.size){
				newaddr++;
			}
			listaddr.addr[newaddr]=i;
			newaddr++;
			}
			tree.datarray[i].RelativePosition=i;
		}
		i=0;
		for(; i<4096; i++){
			list.chars[i].addr=listaddr.addr[i];
		}
	}
	
	//endof __actualize_list
	void __MakeTree(){
		while(tree.NetSize>1){	
			tree.datarray[1].size=tree.datarray[0].size+tree.datarray[1].size;
			__actualize();
		}
	}
	//endof __MakeTree()
	__SetUp();
	__MakeTree();
	return list;
}

