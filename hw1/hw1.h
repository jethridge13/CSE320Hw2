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

int dispStat(FILE *fp) {
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

	return EXIT_SUCCESS;
}

int uDispInfo(FILE *fp){
	printf("REG 	USE 	R-TYPE	I-TYPE	J-TYPE	PERCENT\n");

	int instrCount = 0;

	char line[100];
	// [0] = R-Type
	// [1] = I-Type
	// [2] = Total use
	int reg[32][3];
	int i = 0;
	// Initialize the memory space of the array.
	for(i; i < 32; i++){
		int j = 0;
		for(j; j < 3; j++){
			reg[i][j] = 0;
		}
	}
	int rs = 0;
	int rt = 0;
	int rd = 0;
	// Registers are always 5 bits.
	// This mask will get them if they are shifted into the lower 5 bits.
	int mask = 0x1F;
	while (fscanf(fp, "%s", line) != EOF){
		
		char *ptr;
		unsigned long int value = strtoul(line, &ptr, 16);
		int opcode = -1;
		opcode = value >> 26;

		// J-TYPE
		if (opcode == 2 || opcode == 3){
			instrCount++;
		} // R-TYPE
		else if (opcode == 0) {
			instrCount++;

			value = value >> 11;
			rd = value & mask;
			value = value >> 5;
			rt = value & mask;
			value = value >> 5;
			rs = value & mask;

			reg[rs][0]++;
			reg[rs][2]++;

			reg[rt][0]++;
			reg[rt][2]++;

			reg[rd][0]++;
			reg[rd][2]++;

			// Put the values back to 0.
			rs = 0;
			rt = 0;
			rd = 0;

		} // I-TYPE
		else { 
			instrCount++;

			value = value >> 16;
			rt = value & mask;
			value = value >> 5;
			rs = value & mask;

			reg[rs][2]++;
			reg[rs][1]++;

			reg[rt][2]++;
			reg[rt][1]++;

			rs = 0;
			rt = 0;
		}
	}

	const char * regName[] = {"zero", "at",
		"v0", "v1", "a0", "a1", "a2", "a3", 
		"t0", "t1", "t2", "t3", "t4", "t5", 
		"t6", "t7", "s0", "s1", "s2", "s3",
		"s4", "s5", "s6", "s7", "t8", "t9",
		"k0", "k1", "gp", "sp", "fp", "ra",
		};  

	i = 0;
	for(i; i < 32; i++){
		double perc = 100.0;
		perc = (double)reg[i][2] / instrCount;
		perc = perc * 100;

		printf("$%s 	%d 	%d 	%d 	0 	%f\n", regName[i], reg[i][2], reg[i][0], reg[i][1], perc);
	}
	return EXIT_SUCCESS;
}

int dispInfo(FILE *fp) {
	int instrCount = 0;

	char line[100];
	// [0] = R-Type
	// [1] = I-Type
	// [2] = Total use
	int reg[32][3];
	int i = 0;
	// Initialize the memory space of the array.
	for(i; i < 32; i++){
		int j = 0;
		for(j; j < 3; j++){
			reg[i][j] = 0;
		}
	}
	int rs = 0;
	int rt = 0;
	int rd = 0;
	// Registers are always 5 bits.
	// This mask will get them if they are shifted into the lower 5 bits.
	int mask = 0x1F;
	while (fscanf(fp, "%s", line) != EOF){
		
		char *ptr;
		unsigned long int value = strtoul(line, &ptr, 16);
		int opcode = -1;
		opcode = value >> 26;

		// J-TYPE
		if (opcode == 2 || opcode == 3){
			instrCount++;
		} // R-TYPE
		else if (opcode == 0) {
			instrCount++;

			value = value >> 11;
			rd = value & mask;
			value = value >> 5;
			rt = value & mask;
			value = value >> 5;
			rs = value & mask;

			reg[rs][0]++;
			reg[rs][2]++;

			reg[rt][0]++;
			reg[rt][2]++;

			reg[rd][0]++;
			reg[rd][2]++;

			// Put the values back to 0.
			rs = 0;
			rt = 0;
			rd = 0;

		} // I-TYPE
		else { 
			instrCount++;

			value = value >> 16;
			rt = value & mask;
			value = value >> 5;
			rs = value & mask;

			reg[rs][2]++;
			reg[rs][1]++;

			reg[rt][2]++;
			reg[rt][1]++;

			rs = 0;
			rt = 0;
		}
	}
	i = 0;
	for(i; i < 32; i++){
		double perc = 100.0;
		perc = (double)reg[i][2] / instrCount;
		perc = perc * 100;

		printf("$%d 	%d 	%d 	%d 	0 	%f\n", i, reg[i][2], reg[i][0], reg[i][1], perc);
	}
	return EXIT_SUCCESS;
}

void uDispNum(){
	printf("OPCODE 	COUNT	PERCENTAGE\n");
}

int dispNum(FILE *fp) {
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
	return EXIT_SUCCESS;
}