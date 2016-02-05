void printMenu(){
	fprintf(stdout, "Usage:		./mstat [OPTION]\n");
	fprintf(stdout, "		./mstat -h		Displays this help menu.\n");
	fprintf(stdout, "		./mstat -i [-u]	Displays statistics about instruction types.\n");
	fprintf(stdout, "		./mstat -r [-u]	Displays information about the registers.\n");
	fprintf(stdout, "		./mstat -o [-u]	Displays number and percentage of opcodes used.\n");
	fprintf(stdout, "\n");
	fprintf(stdout, "Optional flags:\n");
	fprintf(stdout, "-u 			Displays human readable headers for the different outputs.\n");
}

int dispStat(FILE *fp, int u) {

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

		char *chp;
		unsigned long int value = strtoul(line, &chp, 16);
		// If strtoul returns 0, either something bad happened or it's 0.
		if(*chp != '\0') {
			fprintf(stderr, "There was an error reading in from the file.\n");
			fprintf(stderr, "Line in question: %s\n", line);
			return EXIT_FAILURE;
		}
		int opcode = -1;
		opcode = value >> 26;
		if(opcode == 0){
			rCount++;
		} else if (opcode == 2 || opcode == 3){
			jCount++;
		} else {
			iCount++;
		}
	}

	if(!instrCount){
		fprintf(stderr, "No valid instructions were found.\n");
		printMenu();
		return EXIT_FAILURE;
	}

	iPerc = (double)iCount / instrCount;
	iPerc = iPerc * 100;

	jPerc = (double)jCount / instrCount;
	jPerc = jPerc * 100;

	rPerc = (double)rCount / instrCount;
	rPerc = rPerc * 100;

	if(u){
		fprintf(stdout, "TYPE	COUNT	PERCENT\n");
	}

	fprintf(stdout, "I-Type 	%d 	%.1f\n", iCount, iPerc);
	fprintf(stdout, "J-Type 	%d 	%.1f\n", jCount, jPerc);
	fprintf(stdout, "R-Type 	%d 	%.1f\n", rCount, rPerc);

	return EXIT_SUCCESS;
}

int dispInfo(FILE *fp, int u) {

	int instrCount = 0;

	char line[100];
	// [0] = R-Type
	// [1] = I-Type
	// [2] = Total use
	int reg[32][3];
	int i = 0;
	// Initialize the memory space of the array.
	for(; i < 32; i++){
		int j = 0;
		for(; j < 3; j++){
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
		
		char *chp;
		unsigned long int value = strtoul(line, &chp, 16);
		if(*chp != '\0') {
			fprintf(stderr, "There was an error reading in from the file.\n");
			fprintf(stderr, "Line in question: %s\n", line);
			return EXIT_FAILURE;
		}
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

	if(!instrCount){
		fprintf(stderr, "No valid instructions were found.\n");
		printMenu();
		return EXIT_FAILURE;
	}

	const char * regName[] = {"zero", "at",
		"v0", "v1", "a0", "a1", "a2", "a3", 
		"t0", "t1", "t2", "t3", "t4", "t5", 
		"t6", "t7", "s0", "s1", "s2", "s3",
		"s4", "s5", "s6", "s7", "t8", "t9",
		"k0", "k1", "gp", "sp", "fp", "ra",
		};  
	if(u){
		fprintf(stdout, "REG 	USE 	R-TYPE	I-TYPE	J-TYPE	PERCENT\n");
	}
	i = 0;
	for(; i < 32; i++){
		double perc = 100.0;
		perc = (double)reg[i][2] / instrCount;
		perc = perc * 100;

		if(u){
			fprintf(stdout, "$%s 	%d 	%d 	%d 	0 	%.1f\n", regName[i], reg[i][2], reg[i][0], reg[i][1], perc);
		} else {
			fprintf(stdout, "$%d 	%d 	%d 	%d 	0 	%.1f\n", i, reg[i][2], reg[i][0], reg[i][1], perc);
		}
	}
	return EXIT_SUCCESS;
}

int dispNum(FILE *fp, int u) {
	int instrCount = 0;
	int rCodeCount = 0;

	errno = 0;

	char line[100];
	int codes[64];
	int func[64];
	int i = 0;
	for(; i < 64; i++){
		codes[i] = 0;
		func[i] = 0;
	}
	//	Place holders for variables because I'm pretty sure 
	// declaring them inside the loop is bad? 
	int opcode = 0;
	char *chp;
	unsigned long int value = 0;
	int funcCode = 0;
	//	Lower 6 bits for function code for R types
	int mask = 0x3f;
	while (fscanf(fp, "%s", line) != EOF){
		instrCount++;

		value = strtoul(line, &chp, 16);
		if(*chp != '\0') {
			fprintf(stderr, "There was an error reading in from the file.\n");
			fprintf(stderr, "Line in question: %s\n", line);
			return EXIT_FAILURE;
		}
		opcode = value >> 26;
		codes[opcode]++;
		if(!opcode){
			rCodeCount++;
			funcCode = value & mask;
			func[funcCode]++;
		}
	}
	if(!instrCount){
		fprintf(stderr, "No valid instructions were found.\n");
		printMenu();
		return EXIT_FAILURE;
	}
	if(u){
		fprintf(stdout, "OPCODE 	COUNT 	PERCENTAGE\n");
	}
	i = 0;
	double perc = 100.0;
	for(; i < 64; i++){
		perc = 100.0;
		perc = (double)codes[i] / instrCount;
		perc = perc * 100;
		
		fprintf(stdout, "0x%x 	%d 	%.1f\n", i, codes[i], perc);
	}
	fprintf(stdout, "\n");
	if(u) {
		fprintf(stdout, "FUNC 	COUNT 	PERCENTAGE\n");
	}
	i = 0;
	for(; i < 64; i++){
		perc = 100.0;
		perc = (double)func[i] / rCodeCount;
		perc = perc * 100;

		fprintf(stdout, "0x%x 	%d 	%.1f\n", i, func[i], perc);
	}
	return EXIT_SUCCESS;
}