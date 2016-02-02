#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hw1.h"

int main(int argc, char *argv[]) {
	if(argc == 1){
		printMenu();
	} else {
		FILE *fp;
		fp = fopen(argv[i], "r");
		if(fp == NULL) {
		for(i; i < argc; i++){
			printf("Argument %d: %s\n", i, argv[i]);
			if(argv[i][0] == '-'){
				switch(argv[i][1]){
					case 'h':
					case 'u':
						printMenu();
						return EXIT_SUCCESS;
					case 'i':
						if(argc - i > 1){
							if(argv[i+1][0] == '-' && argv[i+1][1] == 'u'){
								i++;
								uDispStat();
							}
						}
						dispStat(fp);
						break;
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
						return EXIT_FAILURE;
						break;
				}
			}
		}
	return EXIT_SUCCESS;
	}
}