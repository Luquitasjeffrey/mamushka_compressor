#include "Dictionary.c"
#include "ProgramStructs.h"

//aca creo que hay un error algoritmico que voy a corregir

typedef struct store_char_temp{
	uint16_t      character[2];
	uint16_t	  originalposition; //por un pequeño problema que pasa mas adelante. luego lo cambiare para que ande normalmente
	unsigned int  occurence;
}store_char_temp;

typedef struct store_temporally{
	store_char_temp store[4096];
	uint16_t size;
}store_temporally;

store_temporally initialize_temporal_charlist(uint16_t (*txt)[], uint32_t txtsize){
	store_temporally retrn;
	uint32_t netdel=0;
	uint32_t netsum=0;
	uint32_t netrest=0;
	
	uint32_t i=0;
	for(; i<4096; i++){
		//printf("hey\n");
		retrn.store[i].character[0]=0;
		retrn.store[i].character[1]=0;
		retrn.store[i].character[2]=0;
		retrn.store[i].occurence=0;
	}
	
	
	void deletechar(uint16_t del){
		uint32_t i=0;
		for(; i<txtsize; i++){
			
			if((*txt)[i]==del){
				(*txt)[i]=NULLCHAR;
				netdel+=1;
			}
		}
	}
	
	void deletestr(uint16_t del[2]){
		uint32_t i=0;
		for(; i<txtsize-1; i++){
			if((*txt)[i]==del[0] && (*txt)[i+1]==del[1]){
				netdel+=2;
				(*txt)[i]=NULLCHAR;
				(*txt)[i+1]=NULLCHAR;
				
			}
		}
		
	}
	
	void StoreInReturn(uint16_t chars[2], uint32_t frecuency){
		if(chars[0]!=NULLCHAR){
			deletestr(chars);
		}
		else{
			deletechar(chars[1]);
		}
		
		uint32_t test=0;
		uint32_t count=0;
		
		retrn.store[retrn.size].character[0]=chars[0];
		retrn.store[retrn.size].character[1]=chars[1];
		retrn.store[retrn.size].occurence=frecuency;
		retrn.size+=1;
	}
	
	uint32_t radsize=txtsize>66050? 66050 : txtsize; 
	RAD store[txtsize];
	CHARS datarr;
	retrn.size=0;
	while(/*netsum*/netdel<txtsize){
		//_ReturnBest() chars. un solo caracter 
		//high_score() CHARS. 2 caracteres
		
		//printf("\n%u %u ", retrn.size, netsum);
		
		DATA bchars=high_score(&store, radsize, txt, netrest);
		
		//printf("%x %x", bchars.chars[0], bchars.chars[1]);
		
		/*if(netdel!=netsum){
			printf(" error here(%u %u)!", netsum, netdel);
		}*/
		
		chars bchar=_ReturnBest(&datarr, txt, txtsize, netrest);		
		/*
			*1
		*/
		
		//if b no contiene a
		if(bchars.chars[0]!=bchar.character && bchars.chars[1]!=bchar.character){
			uint16_t store[2];
			store[0]=NULLCHAR;
			store[1]=bchar.character;
			StoreInReturn(store, bchar.appereance);
			netsum+=bchar.appereance; //importante  actualizar el valor de esta variable
			
			uint32_t test1=0;
			uint32_t nptr=0;
			for(; test1<txtsize; test1++){
				if((*txt)[test1]==TXTPTR){
					nptr+=1;
				}
			}
			
			
		//	printf("a");
		//	printf(" (%x) (%u, %u) %f", bchar.character, bchar.appereance, nptr,bchar.alxs);
		}
		//if b contiene a
		else{
			double length_a=bchar.alxs*bchar.appereance;
			//tengo que encontrar c;
			uint16_t found=(bchars.chars[0]==bchar.character)? bchars.chars[1] : bchars.chars[0];
			found=(found==TXTPTR)? TXTPTRVALUE : found;
			chars char_c=datarr.CHARS[found];
			double length_c= log2((txtsize-netrest)/char_c.alxs)*(char_c.alxs+1)+12;
			const double LengthIfNotB=length_a+length_c;
			
			length_a=bchar.appereance-bchars.appereances;
			length_c=char_c.appereance-bchars.appereances;
			if(length_a>0 && length_c>0){
				length_a=16+log2(length_a/(txtsize-netrest))*(length_a+1);
				length_c=16+log2(length_c/(txtsize-netrest))*(length_c+1);
				const double LengthIfYesB=length_a+length_c+bchars.score*bchars.appereances;
				
				if(LengthIfYesB<LengthIfNotB){
					StoreInReturn(bchars.chars, bchars.appereances);
					netsum+=bchars.appereances*2;
					netrest+=bchars.appereances;
					//printf(" %u (%x, %x) b", bchars.appereances*2, bchars.chars[0], bchars.chars[1]);
				}
				else{
					uint16_t store[2];
					store[0]=NULLCHAR;
					store[1]=bchar.character;
					StoreInReturn(store, bchar.appereance);
					//printf("c");
					netsum+=bchar.appereance;
				}
			}
			//if uno de los dos se hace 0. es obvio que va a convenir
			else{
				StoreInReturn(bchars.chars, bchars.appereances);
				netsum+=bchars.appereances*2;
				//printf("d");
				netrest+=bchars.appereances;
			}
		}
		
	}//main loop
	
	//debo ordenar el retorno de menor a mayor segun la frecuencia con la que aparecio
	
	bool not_sorted(){
		uint16_t check=0;
		for(; check<retrn.size-1; check++){
			if(retrn.store[check].occurence>retrn.store[check+1].occurence){
				return 1;
			}
		}
		return 0;
	}
	

	uint32_t k=0;
	for(; k<retrn.size; k++){
		retrn.store[k].originalposition=k;
	}
	
	while(not_sorted()){
		uint32_t check=0;
		for(; check<retrn.size-1; check++){
			if(retrn.store[check].occurence>retrn.store[check+1].occurence){
				store_char_temp carry=retrn.store[check+1];
				retrn.store[check+1]=retrn.store[check];
				retrn.store[check]=carry;
			}
		}
	}
	
	
	
	//printf("\n%u %u\n", netsum, netdel);
	return retrn;
}

void initialize_charlist(store_temporally initialized, CHARLIST *cptr){
	(*cptr).size=initialized.size;
	
	uint16_t i=0;
	for(; i<4096; i++){
		(*cptr).chars[i].addr=-1;
	}
	
	i=0;
	for(; i<initialized.size; i++){
		(*cptr).chars[i].character[0]=initialized.store[i].character[0];
		(*cptr).chars[i].character[1]=initialized.store[i].character[1];
		(*cptr).chars[i].occurence=initialized.store[i].occurence;
		cptr->chars[i].originalpos=initialized.store[i].originalposition;
		int j=0;
		for(; j<32; j++){
			cptr->chars[i].huffmancode[j].h_int=-1;
		}
		
	}
	
	
}

/*
int main(){
	//uint8_t floyd[]="hello hello hello, is there anybody in there";
	
	printf("%u\n", sizeof(CHARLIST));
	unsigned char floyd[65536];
	
	FILE* fpointer=fopen("LZ77.c", "r");
	uint32_t sze=0;
	while(!feof(fpointer)){
		unsigned char read[200];
		memset(read, 0, 200);
		fgets(read, 200, fpointer);
		int j=0;
		while(read[j]!=0){
			floyd[sze]=read[j];
			sze++;
			j++;
		}
	}
	
	uint16_t pink[sze];
	uint32_t i=0;
	for(; i<sze; i++){
		pink[i]=floyd[i];
	}
	uint32_t csize;
	LZ77_COMPRESS(&pink, sze, &csize);
	sze=csize;
	
	RAD hashmap[sze];
	CHARLIST HuffmannTree;
	initialize_charlist(initialize_temporal_charlist(&pink, sze), &HuffmannTree);
	DATA winner=high_score(&hashmap, sze, &pink, 0);
	printf("\n%x %x %u(%f bps)\nsize=%u", winner.chars[0], winner.chars[1], winner.appereances, winner.score, sze);
	
	CHARS datarr;
	chars Winner=_ReturnBest(&datarr, &pink, sze, 0);
	printf("\n%u %x (%f bps)", Winner.appereance, Winner.character, Winner.alxs);
	
	printf("\n");
	system("pause");
	return 0;
}*/
		/*
		
		*1
		
		aca viene el momento importante del codigo
			voy a decidir según cual ocupa menos espacio cual de las dos opciones voy a guardar.
			asímismo voy a tener en cuenta la acción que tomaría poner la secuencia en el diccionario.
			la acción sera expresada matemáticamente de la siguiente forma:
			
			sea 'a' la opción de elegir un solo caracter y 'b' la opcion de elegír 2
			if(alxs(b)<alxs(a)){
				if(b no contiene a){
					STORE(a);
				}
				else{
					sea 'c' igual al otro simbolo que contiene la secuencia 'b'
					sea 'N' igual a la sumatoria de todos los bits almacenados sin almacenar b
					sea '|b'= sabiendo que se almaceno b
					
					N1=alxs(a)*n(a)+alxs(c)*n(c)
					N2=alxs(a|b)*(n(a)-n(b))+alxs(b)*n(b)+alxs(c|b)*(n(c)-n(b))
					
					if(N2>N1){
						STORE(b);
					}
					else{
						STORE(a);
					}
				}
			}
			else{
				el mismo procedimiento que antes calculando las variantes
				//STORE (a);
				//STORE (b);
			}
			
		*/
