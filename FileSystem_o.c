#include <sys/stat.h>
#include <sys/types.h>
#include "FileSystem_i.c"
#include "FileReader.c"
#include "mamushka.c"
//#include "LZ77.c"

void GetName(struct filesearch *srch, struct _dir *dir, char (*rvalue)[], const char* route, bool mode){
	uint32_t important_data=0;
	dir->strpos=0;

	char filename[4096];
	memset(filename, 0, 4096);
	strcat(filename, route);

	uint32_t filenum=0;

	bool _isdir(uint32_t since, uint32_t until, uint32_t *dirlen){

		char add[1024];
		memset(add, 0, 1024);
		int i=0;
		for(; since<until; since++){
			add[i]=dir->code[since];
			i++;
		}

		strcat(filename, "/");
		strcat(filename, add);



		filenum++;
		dir->strpos=until;


		if(dir->code[until]>127){

			if(mode){
				mkdir(filename);
				//make directory
			}

			*dirlen=(dir->code[until]-128)*128 + dir->code[until+1];
			filenum--;
			return 1;
		}

		else if(filenum==(srch->filenumber+1) && rvalue!=NULL){
			i=0;
			srch->txtpos=dir->strpos;
			uint32_t len=strlen(filename);
			for(; i<len; i++){
				(*rvalue)[i]=filename[i];
			}
		}

		/*

		*/
		return 0;
	}

	void dlastdir(){

		int i=0;
		int slashpos=0;
		while(filename[i]!=0){
			if(filename[i]==47){
				slashpos=i;
			}
			i++;
		}
		for(; slashpos<i; slashpos++){
			filename[slashpos]=0;
		}
	}

	void search(uint32_t dirlen){
		//printf("%u\n\n", namelen);
		//printf("inf loop %u\n", noffiles);
		uint32_t j=0;
		for(; j<dirlen; j++){
			uint32_t namelen=dir->code[dir->strpos]*256+dir->code[dir->strpos+1];
			dir->strpos+=2;
			uint32_t newlen;
			//printf("(%u)", dirlen);
			if(_isdir(dir->strpos, dir->strpos+namelen, &newlen)){
				dir->strpos+=2;
				//printf("<DIR(%u)>\n", dirlen);
				//printf("\nnew len is:(%u, %u)\n", newlen, namelen);
				search(newlen);

			}
			else{


				if((filenum-1)==srch->filenumber){
					important_data=dir->strpos; //termino recorriendo todo el archivo de dirs porque bueno me dio paja corregirlo
					//printf("returning: %s\n", filename);
					//return;
				}


				//printf("\n%u", noffiles);
				dir->strpos+=8;
			}

			dlastdir();
		}
	}
	search(1);
	//printf("%u\n", noffiles);
	//printf("%u %u", noffiles, srch->filenumber);

	srch->filenumber++;
	srch->txtpos=important_data;

}


void CopyInDirectories(uint8_t (*dectxt)[], uint64_t txtsize, const char *dest_route){


	struct _dir dir;

	uint32_t i=0;
	for(; i<25000; i++){
		dir.code[i]=(*dectxt)[i];
	}

	char destination[4096];
	memset(destination, 0, 4096);

	struct filesearch srch;
	srch.filenumber=0;
	GetName(&srch, &dir, &destination, dest_route, 1);


	void extractfile(uint64_t since, uint64_t to){
		printf("%s (%u)\n", destination, to-since);
		FILE *file;
		file=fopen(destination, "wb");
		for(; since<to; since++){
			putc((*dectxt)[since], file);
		}
		fclose(file);
	}

	srch.filenumber=0;
	do{
		memset(destination, 0, 4096);
		GetName(&srch, &dir, &destination, dest_route, 0);
		uint64_t since=0;
		uint64_t add=1;
		int j=7;
		for(; j>0; j--){
			since+=add*(*dectxt)[srch.txtpos+j];
			add*=256;
		}
		//srch.filenumber+=1;

		GetName(&srch, &dir, NULL, dest_route, 0);
		uint64_t to=0;
		add=1;
		for(j=7; j>0; j--){
			to+=add*(*dectxt)[srch.txtpos+j];
			add*=256;
		}

		srch.filenumber-=1;
		if(srch.txtpos==0){
			to=txtsize;
		}
		//printf("<%u>\n", to);
		extractfile(since, to);

	}while(srch.txtpos!=0);

}


/*
int main(){


	uint16_t txt[0x1000000];
	uint32_t size;
	makeall(&txt, 0x1000000, "Compression algorithms C.jcc", &size);

	printf("\nhuffman complete\n");

	uint8_t extract[0x1000000];
	memset(extract, 0, 0x1000000);


	//LZ77EXTRACT(&extract, &txt, size, &size);
    mamushka_dec(&extract, &txt, size, &size);

	printf("\nafter LZ77 size is: %u\n", size);

	CopyInDirectories(&extract, size, "program output");


	return 0;
}
*/
