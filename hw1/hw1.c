#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hw1.h"

int main(int argc, char *argv[]) {
	// If more than 
	if(argc > 3){
		printMenu();
		return EXIT_FAILURE;
	}
	if(argc == 1){
		printMenu();
	} else {
		FILE *fp;
		fp = stdin;
		//fp = fopen(argv[i], "r");
		if(fp == NULL) {
			printf("NULL File\n");
		} else {
			// Print statement used for debugging
			//printf("FILE\n");
		}
		int i = 0;
		for(i; i < argc; i++){
			// Print statement used for debugging
			//printf("Argument %d: %s\n", i, argv[i]);
			if(argv[i][0] == '-'){
				switch(argv[i][1]){
					case 'h':
						printMenu();
						return EXIT_SUCCESS;
					case 'u':
						printMenu();
						return EXIT_FAILURE;
					case 'i':
						if(argc - i > 1){
							if(argv[i+1][0] == '-' && argv[i+1][1] == 'u'){
								i++;
								uDispStat();
							}
						}
						dispStat(fp);
						return EXIT_SUCCESS;
					case 'r':
						if(argc - i > 1){
							if(argv[i+1][0] == '-' && argv[i+1][1] == 'u'){
								i++;
								uDispInfo();
							}
						}
						dispInfo(fp);
						break;
					case 'o':
						if(argc - i > 1){
							if(argv[i+1][0] == '-' && argv[i+1][1] == 'u'){
								i++;
								uDispNum();
							}
						}
						dispNum(fp);
						break;
					default:
						printMenu();
						return EXIT_FAILURE;
						break;
				}
			}
		}
	return EXIT_SUCCESS;
	}
}