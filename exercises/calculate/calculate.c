#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 10
#define NUM_QUESTIONS 1000

char* getFlag(){
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

int is_digit(char c) {
    return '0' <= c && c <= '9';
}

int is_number(const char *num) {	
	// negative numbers not considered
	int i = 0;
	while ( i < 10 ) {
		if ( ! is_digit( num[i] )) {
			if ( num[i] == '\0' || num[i] == '\n' ) {
				return atoi(num);
			}
			else {
				break;
			}
		}
		i++;
	}
	return 0;
}

void seedrand() {
    FILE *f = fopen("/dev/urandom", "r");
    unsigned seed;
    fread(&seed, sizeof(seed), 1, f);
    srand(seed);
    fclose(f);
}

int main() {	
	char *flag = getFlag();
	
	puts("Welcome to the mathematic competition for primary school students.");
	puts("Solve all the problems to get the flag.");
	
	seedrand();

	int x, y, res;
	int i;

	char buffer[BUFFER_SIZE];

	for ( i=0; i<NUM_QUESTIONS; i++) {
		x = rand() % 40 + 1;
		y = rand() % 40 + 1;
		printf("%d + %d = ? \n", x, y);
		
		memset(buffer, '\0', BUFFER_SIZE);
		if ( fgets(buffer, BUFFER_SIZE , stdin) != NULL ) {						
			res = is_number(buffer);
			if ( res != x + y ) {				
				puts("Oops! Wrong answer! Good luck next time ;) ");
				return 0;
			}
			
		}

	}

	printf("Well done! Here is the flag: %s\n", flag);
	return 0;
}
