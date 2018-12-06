/* Implementation of the log functions */
/* for comments on the functions see the associated .h-file */

/* ---------------------------------------------------------------- */
/* Include required external definitions */
#include <math.h>
#include "bs_types.h"
#include "globals.h"
#include "log.h"


/* ---------------------------------------------------------------- */
/*                Declarations of local helper functions            */

/* ---------------------------------------------------------------- */
/* Declarations of global variables visible only in this file 		*/
// array with strings associated to scheduling events for log outputs
char eventString[3][12] = {"completed", "io", "quantumOver"};

/* ---------------------------------------------------------------- */
/*                Externally available functions					*/
/* ---------------------------------------------------------------- */

void logGeneric(char* message)
{
	printf("%6u : %s\n", systemTime, message); 
}
	
void logPid(unsigned pid, char * message)
{
	printf("%6u : PID %3u : %s\n", systemTime, pid, message); 
}
		
void logPidCompleteness(unsigned pid, unsigned done, unsigned length, 
						char * message)
{
	printf("%6u : PID %3u : completeness: %u/%u | %s\n", systemTime, 
			pid, done, length, message); 
}
		
void logPidMem(unsigned pid, char * message)
{
	printf("%6u : PID %3u : Used memory: %6u | %s\n", systemTime, 
			pid, usedMemory, message); 
}
		

/* ----------------------------------------------------------------- */
/*                       Local helper functions                      */
/* ----------------------------------------------------------------- */



