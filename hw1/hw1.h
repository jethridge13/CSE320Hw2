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
	printf("Display Stats\n");
	char test[100];
	while (fscanf(fp, "%s", test) != EOF)
		printf("String: %s\n", test);
	*/

	int instrCount = 0;

	int iCount = 0;
	double iPerc = 100.0;

	int jCount = 0;
	double jPerc = 100.0;

	int rCount = 0;
	double rPerc = 100.0;

	char line[100];
	while (fscanf(fp, "%s", line) != EOF){
		instrCount++;
		//Mask the other chars out
		char value[3];
		value[0] = line[2];
		value[1] = line[3];
		value[2] = '\0';
		//printf("%s\n", value);
		int opcode = -1;
		if(value[0] != 'a'){
			switch(value[1]){
				case 'a':
				case 'b':
				case 'c':
				case 'd':
				case 'e':
				case 'f':
					break;
				default:
					opcode = atoi(value);
					break;
			}
		}
		if(opcode == 0){
			rCount++;
		} else if (opcode == 2 || opcode == 3){
			jCount++;
		} else {
			iCount++;
		}
	}

	iPerc = (double)iCount / instrCount;
	iPerc = iPerc * 100;

	jPerc = (double)jCount / instrCount;
	jPerc = jPerc * 100;

	rPerc = (double)rCount / instrCount;
	rPerc = rPerc * 100;

	printf("Total Instructions: %d\n", instrCount);

	printf("I-Type 	%d 	%f\n", iCount, iPerc);
	printf("J-Type 	%d 	%f\n", jCount, jPerc);
	printf("R-Type 	%d 	%f\n", rCount, rPerc);
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
	int instrCount = 0;

	char line[100];
	int codes[64];
	int i = 0;
	for(i; i < 64; i++){
		codes[i] = 0;
	}
	while (fscanf(fp, "%s", line) != EOF){
		instrCount++;

		//char opcode[5];
		//opcode[4] = '\0';

		//int j = 0;
		//for(j; j < 4; j++){
		//	opcode[j] = line[j];
		//}

		//printf("%s\n", opcode);

		char *ptr;
		unsigned long int value = strtoul(line, &ptr, 16);
		value = value >> 26;
		codes[value]++;
		//printf("%lu\n", value);
	}
	i = 0;
	for(i; i < 64; i++){
		double perc = 100.0;
		perc = (double)codes[i] / instrCount;
		perc = perc * 100;
		
		printf("0x%x 	%d 	%f\n", i, codes[i], perc);
	}
}