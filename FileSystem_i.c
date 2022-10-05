//tengo que abrir directorios y poner en la secuencia de caracteres los nombres de los archivos

/*
en la version 0.1 no se agruparan los archivos por extension
sin embargo luego se agruparan archivos por extension para lograr
un mayor ratio de compresion y mayor velocidad a la hora de abrir
un archivo comprimido.
*/
#include <dirent.h>
#include <stdlib.h>
#include "FileWriter.c"

//al principio de cada file/dir van 2 uchars que me dicen la longitud del titulo
//al final ira un uchar >128 si es un directorio y 0 si es un archivo.
//si es uchar>128, se especificara la cantidad de files/subdirs que hay en el dir
//al final de cada file ira un numero que sera un pointer a la posicion incial en la cadena de caracteres

struct _dir{
	uint16_t code[0x1000000];
	uint32_t strpos;
};

void copydir(struct _dir *dir, unsigned char *dirname){
	uint16_t len=strlen(dirname);
	int i=0;
	dir->code[dir->strpos]=len/256; 
	dir->code[dir->strpos+1]=len%256;
	dir->strpos+=2;
	for(; i<len; i++){
		dir->code[dir->strpos]=dirname[i];
		dir->strpos++;
	}
}

void listdir(const char* dirname, struct _dir *dirsection, int count){
	
	DIR *dir;
	struct dirent *entry;
	dir=opendir(dirname);
	
	//base case: (file)
	if(dir==NULL){
		dirsection->code[dirsection->strpos]=0;
		dirsection->strpos+=8;
		//hay que especificar que es un archivo y NO un directorio
		//en los 7 restantes iría almacenado el puntero en el archivo jcc
		return;
	}
	
	//recursive case: (subdir)
	
	//si paso este if, significa que es un directorio.
	//debo especifiar que se trata de un directorio
	const uint32_t STRPOS=dirsection->strpos;
	
	//ya que aca almacenare que se trata de un directorio y la cantidad de subdirs
	dirsection->strpos+=2;
	
	int n=0;
	
	
	while((entry=readdir(dir))!=NULL){
		
		//solo para debug
		
		/*
		int j=0;
		for(; j<count; j++){
			printf("	");
		}
		printf("%s\n", entry->d_name);
		*/
		
		if(n>1){
			//aca almaceno el nombre del archivo/directorio
			copydir(dirsection, entry->d_name);
			
			char newname[8192];
			memset(newname, 0, 8192);
			strcpy(newname, dirname);
			strcat(newname, "/");
			strcat(newname, entry->d_name);
			//aca vuelvo a repetir lo mismo en cada archivo/subdirectorio hasta que no haya mas subdirectorios
			//la funcion se detendra cuando no haya mas dirs
			listdir(newname, dirsection, count+1);
		}
		n++;
	}
	n-=2;
	//printf("\n");
	
	//aca pongo cuantos archivos/subdirs hay siendo que 
	dirsection->code[STRPOS]=128+n/128;
	dirsection->code[STRPOS+1]=n%256;
	closedir(dir);	
}

void DirList(struct _dir *dir, const char *dirname, unsigned char* name, bool mode){
	
	if(mode){
	
	memset(dir->code, 0, 0x100000);
	dir->strpos=0;
	copydir(dir, name);
	//strcat(dir->code, &addlen);
	//strcat(dir->code, dirname);
	listdir(dirname, dir, 0);
	}
	//printf("\n\n");
	
	
	bool printname(uint32_t since, uint32_t until, uint32_t *dirlen, int spacecount){
		
		int j=0;
		for(; j<spacecount; j++){
			printf("	");
		}
		
		for(; since<until; since++){
			printf("%c", dir->code[since]);
		}
		printf("\n");
		//printf("\n%u", since);
		
		//since++;
		dir->strpos=since;
		if(dir->code[since]>127){
			*dirlen=(dir->code[since]-128)*128 + dir->code[since+1];
			return 1;
		}
		return 0;
	}
	
	uint32_t important_data=dir->strpos;
	dir->strpos=0;
	
	uint32_t nfiles=0;
	void printall(uint32_t dirlen, int spacecount){
		//printf("%u\n\n", namelen);
		uint32_t j=0;
		for(; j<dirlen; j++){
			uint32_t namelen=dir->code[dir->strpos]*256+dir->code[dir->strpos+1];
			dir->strpos+=2;
			uint32_t newlen;
			if(printname(dir->strpos, dir->strpos+namelen, &newlen, spacecount)){
				dir->strpos+=2;
				//printf("<DIR(%u)>\n", dirlen);
				printall(newlen, spacecount+1);
				
			}
			else{
				//printf("(%u/%u)", j, dirlen);
				nfiles++;
				dir->strpos+=8;
			}
		}
	}
	
	//printf("\n\n");
	printall(1, 0);
	printf("size of dir is: %u\n\n", important_data);
	dir->strpos=important_data;
	
}

struct filesearch{
	uint32_t txtpos;
	uint32_t filenumber;
};

void searchfile(struct filesearch *srch, struct _dir *dir){
	uint32_t important_data=0;
	dir->strpos=0;
	uint32_t noffiles=0;
	
	bool _isdir(uint32_t until, uint32_t *dirlen){
		
		dir->strpos=until;
		if(dir->code[until]>127){
			*dirlen=(dir->code[until]-128)*128 + dir->code[until+1];
			return 1;
		}
		return 0;
	}
	
	uint32_t retrn;
	void search(uint32_t dirlen){
		//printf("%u\n\n", namelen);
		//printf("inf loop %u\n", noffiles);
		uint32_t j=0;
		for(; j<dirlen; j++){
			uint32_t namelen=dir->code[dir->strpos]*256+dir->code[dir->strpos+1];
			dir->strpos+=2;
			uint32_t newlen;
			//printf("(%u)", dirlen);
			if(_isdir(dir->strpos+namelen, &newlen)){
				dir->strpos+=2;
				//printf("<DIR(%u)>\n", dirlen);
				//printf("\nnew len is:(%u, %u)\n", newlen, namelen);
				search(newlen);
				
			}
			else{
				
				if(noffiles==srch->filenumber){
					important_data=dir->strpos; //termino recorriendo todo el archivo de dirs porque bueno me dio paja corregirlo
					//return;
				}
				noffiles++;
				//printf("\n%u", noffiles);
				dir->strpos+=8;
			}
		}
	}
	search(1);
	//printf("%u\n", noffiles);
	
	//printf("%u %u", noffiles, srch->filenumber);
	srch->filenumber++;
	srch->txtpos=important_data;
}

void StoreAll(unsigned char* dirname, uint16_t (*txt)[], uint32_t txtsize, uint32_t *fsize, unsigned char* DirName){
	//printf("inf loop\n");
	
	memset(txt, 0, txtsize);
	
	
	struct _dir dirtxt;
	DirList(&dirtxt, dirname, DirName, 1);
	
	uint32_t dirtxtsize=dirtxt.strpos;
	struct filesearch fcount;
	fcount.filenumber=0;
	
	uint32_t txtposition=dirtxt.strpos;
	void _StoreAll(const char* dirname){
		DIR *dir;
		struct dirent *entry;
		dir=opendir(dirname);
		
		//base case: (file)
		if(dir==NULL){
			//aca significa que llegue a un archivo, por lo que puedo copiar todo en el archivo
			searchfile(&fcount, &dirtxt);
			
			uint64_t postxt=txtposition;
			int j=7;
			
			for(; j>0; j--){
				dirtxt.code[fcount.txtpos+j]=postxt%256;
				postxt/=256;
			}
			printf("<%u> ", fcount.txtpos);
			dirtxt.strpos+=8;
			uint32_t size0=txtposition;
			copyfile(dirname, txt, txtposition, 0x1000000, &txtposition);
			printf("%s (%u)\n", dirname, txtposition);
			return;
		}
		//recursive case: (sub directories)
		int n=0;
		while((entry=readdir(dir))!=NULL){
	
			if(n>1){
				//aca almaceno el nombre del archivo/directorio
				char newname[8192];
				memset(newname, 0, 8192);
				strcpy(newname, dirname);
				strcat(newname, "/");
				strcat(newname, entry->d_name);
				//aca vuelvo a repetir lo mismo en cada archivo/subdirectorio hasta que no haya mas subdirectorios
				//la funcion se detendra cuando no haya mas dirs
				_StoreAll(newname);
			}
			n++;
		}
		
	}
	
	
	//DirList(&dirtxt, ".", ".", 0);
	_StoreAll(dirname);
	
	//aja, aca encontre el error
	//DirList(&dirtxt, ".", ".", 0);
	
	uint32_t cpy=0;
	for(; cpy<dirtxtsize; cpy++){
		(*txt)[cpy]=dirtxt.code[cpy];
	}
	
	printf("\nnet size in bytes: %u\n", txtposition);
	*fsize=txtposition;
}



