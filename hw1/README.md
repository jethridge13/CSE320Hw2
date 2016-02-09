Joshua Ethridge
108639966

I put the functions that are called from hw1.c in hw1.h. I'm not entirely
sure if that is good convention, but it functions. 

If no file is provided, the program will take in input from the console. 
It will write to stderr and exit on failure if the input provided in the 
console does not also fit. Also, if no input is given to the console then
the program will exit on failure stating that no instructions were found. 
This I believe to be correct according to piazza and the grading criteria.

For the tool test case near the end of the assignment, the given example is: 
$ ./mstat -i < instructions.txt | sort -k2nr | grep 2
J-Type 28 5.8%
R-Type 121 25.1%
But when run with my program, returns:
./mstat -i < samples/mips4.txt | sort -k2nr | grep 2
R-Type 	121 	25.1%
J-Type 	28 	5.8%
While this does not match the example, I believe it to be correct based on my
understanding of the functions. That is, -k2nr should sort the strings based 
on the 2nd column and then reverse that order. My program correctly does this. 