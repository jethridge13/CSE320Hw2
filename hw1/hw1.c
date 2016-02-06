#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "hw1.h"

int main(int argc, char *argv[]) {
	// If more than 3 arguments, doesn't work. 
	if(argc > 3){
		fprintf(stderr, "Too many arguments given.\n");
		printMenu();
		return EXIT_FAILURE;
	}
	if(argc == 1){
		fprintf(stderr, "No arguments found.\n");
		printMenu();
		return EXIT_FAILURE;
	} else {
		FILE *fp;
		fp = stdin;
		int i = 0;
		for(; i < argc; i++){
			if(argv[i][0] == '-'){
				switch(argv[i][1]){
					case 'h':
						printMenu();
						return EXIT_SUCCESS;
					case 'u':
						fprintf(stderr, "The -u argument should only be used with another command.\n");
						printMenu();
						return EXIT_FAILURE;
					case 'i':
						if(argc - i > 1){
							if(argv[i+1][0] == '-' && argv[i+1][1] == 'u'){
								i++;
								return dispStat(fp, 1);
							} else {
								return dispStat(fp, 0);
							}
						} else {
							return dispStat(fp, 0);
						}
					case 'r':
						if(argc - i > 1){
							if(argv[i+1][0] == '-' && argv[i+1][1] == 'u'){
								i++;
								return dispInfo(fp, 1);
							} else { 
								return dispInfo(fp, 0);
							}
						} else {
							return dispInfo(fp, 0);
						}
					case 'o':
						if(argc - i > 1){
							if(argv[i+1][0] == '-' && argv[i+1][1] == 'u'){
								i++;
								return dispNum(fp, 1);
							} else { 
								return dispNum(fp, 0);
							}
						} else {
							return dispNum(fp, 0);
						}
						break;
					default:
						fprintf(stderr, "No valid arguments found\n");
						printMenu();
						return EXIT_FAILURE;
						break;
				}
			} 
		}
	}
}