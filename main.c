// A01151866
// 13256
// This main source file performs a low pass filter algorithm given an input of pressure vaules (of type double)
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "PressureInfo.h"

/*	This function displays the usage instructions and exits function with a -1 value	*/
void ShowUsageAndExit(){
	printf("pressureFilter <inputfile> <betavalue> <outputfile>\n");
	printf("where:\n");
	printf("<inputfile> is the pathname of the file containing pressure sensor data.\n");
	printf("<betavalue> is a real number between 0 and 1 used in the filter.\n");
	printf("<outputfile> is the pathname of the filtered data file to be written.\n");
	exit(-1);
}


int main(int argc, char *argv[]){

	/* If incorrect num of arguments, show usage statement. */
	if (argc != 4){
		printf("Incorrect number of comand-line arguments.\n");
		ShowUsageAndExit();
	}

	/* Uses sscanf to convert argv[2] into  a double. If beta is NOT between
		0 and 1, display an error message and call ShowusageAndExit function*/
	double betaValue = 0.0;
	if ( (sscanf(argv[2],"%lf", &betaValue) != 1) || (betaValue < 0) || (betaValue > 1)){
		printf("Error: beta value must be greater than 0 and less than 1.\n");
		ShowUsageAndExit();
	}

	/*	Call GetFileLineCount to fine the number of lines in the input file.	*/
	unsigned numEntries = GetFileLineCount(argv[1]);
	
	/*	Calling LoadSensorDataFromFile and stores the pointer returned.	*/
	PressureInfo *pressureData = NULL;	// this will 'point' to the pointer returned by the function
	pressureData = LoadSensorDataFromFile(argv[1], numEntries);

	/*	Returns -1 if pointer returned by LoadSensorFromDataFile is NULL, or
		the number of lines is not greater than zero	*/
 	if ((pressureData == NULL) || (numEntries < 1)){
		printf("Error: pressureData is NULL and/or number of entries is not greater than 0.\n");
		return -1;
	}

	/*	Calling ApplyLowpassFilter to perform filtering operation and calling AnalyzeFilterData
		to perform the weather prediction analysis	*/
	ApplyLowpassFilter(pressureData, numEntries, betaValue);
	AnalyzeFilteredData(pressureData, numEntries);

	/*	Calling SavePressureReportToFile to write results to output file. If this call fails,
		an error display message will appear then return -1 from main. Regardless, free the array back
		to the heap and return 0 from main */
	if ( !(SavePressureReportToFile(pressureData, numEntries, argv[3])) ){
		printf("Error: writing results to output file %s \n", argv[3]);
		free(pressureData);
		return -1;
	}
	free(pressureData);
	return 0;
}