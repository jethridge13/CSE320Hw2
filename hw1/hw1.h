void printMenu(){
	printf("Usage:		./mstat [OPTION]\n");
	printf("		./mstat -h		Displays this help menu.\n");
	printf("		./mstat -i [-u]	Displays statistics about instruction types.\n");
	printf("		./mstat -r [-u]	Displays information about the registers.\n");
	printf("		./mstat -o [-u]	Displays number and percentage of opcodes used.\n");
	printf("\n");
	printf("Optional flags:\n");
	printf("-u 			Displays human readable headers for the different outputs.\n");
}

void uDispStat() {
	printf("TYPE	COUNT	PERCENT\n");
}

void dispStat(FILE *fp) {
	/*
	char test[100];
	while (fscanf(fp, "%s", test) != EOF)
					printf("String: %s\n", test);
					*/
}

void uDispInfo(){
	printf("REG 	USE 	R-TYPE	I-TYPE	J-TYPE	PERCENT\n");
}

void dispInfo(FILE *fp) {

}

void uDispNum(){
	printf("OPCODE 	COUNT	PERCENTAGE\n");
}

void dispNum(FILE *fp) {

}