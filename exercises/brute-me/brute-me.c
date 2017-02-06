#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char* loadFile(){
	FILE *fp;
	char *buffer;
	int n;

	fp = fopen("flag.txt", "r");	

	
	if( fp == NULL )  {
		perror ("Error opening file");
		return NULL;
	}

	fseek(fp, 0, SEEK_END);
	n = ftell(fp);

	rewind(fp);
	buffer = (char*)calloc(n, sizeof(char));

	fread(buffer, sizeof(char), n, fp);
	
	fclose(fp);

	return buffer;
}

int main(int argc, char **argv){
	char *flag;
	int ret;
	
	puts("~Brute Me~");
	puts("Hit me with args and see if I can take the hits! :D");
	printf("\n\nResult: ");

	if ( argc != 2) {
		puts("You never hit me properly!");
	}
	else {    
        flag = loadFile();
        if ( flag == NULL ) {
        	return -1;
        }

		ret = strncmp(flag, argv[1], 28);

		if(ret < 0) {
			puts("Too high!");
		}
		else if(ret > 0) {
			puts("Too low!");
		}		
		else {
			puts("YOU GOT IT! Or did you? ;)");
		}	
        
    }
    return 0;
}
