Joshua Ethridge
108639966

I put the functions that are called from hw1.c in hw1.h. I'm not entirely
sure if that is good convention, but it functions. 

If no file is provided, the program will take in input from the console. 
It will write to stderr and exit on failure if the input provided in the 
console does not also fit. Also, if no input is given to the console then
the program will exit on failure stating that no instructions were found. 
This I believe to be correct according to piazza and the grading criteria.