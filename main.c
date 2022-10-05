//main.c
/*compile with the following command

gcc.exe -Wl,--stack,536870912 -o mamushka.exe main.c

*/
#include "io.c"


int main(int argc, char *(argv[])){
	int flags;
	
	if(*(argv[1])=='c'){
		switch(*(argv[4])){
			case '4':
				flags=-1;
				break;
			
			case '0':
				flags=0;
				break;
			
			case '1':
				flags=1;
				break;
				
			case '2':
				flags=2;
				break;
				
			case '3':
				flags=3;
				break;
				
			default:
				printf("error in oflags\n", stderr);
				return 1;
		}
		compress(argv[2], argv[3], flags);
		return 0;
	}
	else if(*(argv[1])=='d'){
		decompress_directory(argv[2], argv[3]);
		return 0;
	}
	
	return 1;
}
