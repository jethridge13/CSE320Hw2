#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	printf("%d\n", argc);
	int i = 0;
	for(i; i < argc; i++){
		printf("%d\n", *argv[i]);
	}
	printf("Usage:		./mstat [OPTION]\n");
	printf("		./mstat -h		Displays this help menu.\n");
	printf("		./mstat -i [-u]	Displays statistics about instruction types.\n");
	printf("		./mstat -r [-u]	Displays information about the registers.\n");
	printf("		./mstat -o [-u]	Displays number and percentage of opcodes used.\n");
	printf("\n");
	printf("Optional flags:\n");
	printf("-u 			Displays human readable headers for the different outputs.\n");
	return EXIT_SUCCESS;
}