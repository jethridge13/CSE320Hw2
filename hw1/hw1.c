#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hw1.h"

int main(int argc, char *argv[]) {
	if(argc == 1){
		printMenu();
	} else {
		FILE *fp;
		int i = 1;
		for(i; i < argc; i++){
			printf("Argument %d: %s\n", i, argv[i]);
			if(argv[i][0] == '-'){
				switch(argv[i][1]){
					case 'h':
						printMenu();
						break;
					case 'i':
						dispStat();
						break;
					case 'r':
						dispInfo();
						break;
					case 'o':
						dispNum();
						break;
					case 'u':
						//TODO
						break;
				}
			// If argument does not start with -, assume is filename
			} else {
				fp = fopen(argv[i], "r");
				if(fp == NULL) {
					printf("File not found\n");
				}
				char test[100];
				while (fscanf(fp, "%s", test) != EOF)
					printf("String: %s\n", test);
			}	
		}
	}
	return EXIT_SUCCESS;
}