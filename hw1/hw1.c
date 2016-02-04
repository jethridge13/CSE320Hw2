#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hw1.h"

//TODO Consider changing how the file is read to fit specifications
//TODO Provide error messages for each EXIT_FAILURE
//TODO Makefile
int main(int argc, char *argv[]) {
	// If more than 
	if(argc > 3){
		printMenu();
		return EXIT_FAILURE;
	}
	if(argc == 1){
		printMenu();
		return EXIT_FAILURE;
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
						return dispStat(fp);
					case 'r':
						if(argc - i > 1){
							if(argv[i+1][0] == '-' && argv[i+1][1] == 'u'){
								i++;
								return uDispInfo(fp);
							}
						}
						return dispInfo(fp);
					//TODO Function field arguments
					case 'o':
						if(argc - i > 1){
							if(argv[i+1][0] == '-' && argv[i+1][1] == 'u'){
								i++;
								uDispNum();
							}
						}
						return dispNum(fp);
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