// A01151866
// 13256
// This source file defines the functions which will be used in the main source file and which were declared in the header file.
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "PressureInfo.h"


/*	Recieves a pointer to a string containing the input file name, and returns the total number
	of lines present in the text file.	*/
unsigned GetFileLineCount(const char *pFilename){

	// checks if the file is not NULL and string length is greater than 0
	if ( !(pFilename) || !(strlen(pFilename) > 0) ){
		return 0;
	}

	FILE *inputFile = fopen(pFilename, "r");	// opens file for READING
	if (inputFile == NULL){
		return 0;
	}

	int totalLines = 0;
	while (!feof(inputFile)){
		// when the file reaches a new line, increment totalLines
		if ( fgetc(inputFile) == '\n' ){
			totalLines++;
		}
	}

	fclose(inputFile);		// always close file when done
	return totalLines;		// return the total number of lines present in the text file
}


/*	Receives a pointer to a string containing the input file name and the number of lines contained in the file,
	and returns ointer to a dynamically-allocated array of PressureInfo structures.	*/
PressureInfo *LoadSensorDataFromFile(const char *pFilename, const unsigned infoCount){
	
	//	checks if pointer to file is not NULL, length of string is nonzero, and infoCount is >1
	if ((pFilename == NULL) || (strlen(pFilename) == 0) || !(infoCount > 1)){
		return NULL;
	}

	// open file for reading, verify the open succeeded
	FILE* inputFile = fopen(pFilename, "r");
	if (inputFile == NULL){
		return NULL;
	}

	/*	allocate a block of memory large enough for the number of PressureInfo structures specified by infoCount
		then verify malloc succeeded	*/
	PressureInfo* pressureInput = NULL;
	pressureInput = malloc(sizeof(PressureInfo)*infoCount);
	if (pressureInput == NULL){
		return NULL;
	}

	unsigned int i = 0;				// used for iteration
	int scanResult = 0;				// used to verify sscanf succeeded
	for (i = 0;i<infoCount; i++){
		scanResult = fscanf(inputFile, "%lf", &pressureInput[i].sensorPressure);
		if (scanResult != 1){
			free(pressureInput);
			fclose(inputFile);
			return NULL;
		}
		// initializing the reset of the structure:
		pressureInput[i].filteredPressure = 0.0;
		pressureInput[i].forecast = NULL;
		if ((pressureInput[i].sensorPressure >= LOWEST_PRESSURE_RECORD) && (pressureInput[i].sensorPressure <= HIGHEST_PRESSURE_RECORD)){
			pressureInput[i].isValid = true;
		}
		else{
			pressureInput[i].isValid = false;
		}
	}

	fclose(inputFile);		// always close file when finished
	return pressureInput;	// return pointer to the allocated array to the caller
}


/*	Receives a pointer to an array of PressureInfo structures, the number of elements in the array, 
	and the beta value that will be plugged into the lowpass filter formula. This function performs
	its calculations and stores the result into the filteredPressure member of each element in the array */
void ApplyLowpassFilter(PressureInfo *pressureInfo, const unsigned infoCount, const double beta){

	unsigned i = 0;					// used for iteration through the array
	bool isFirstValid = true;		// flag used to identify the first valid array of a sequence

	/*	FOR loop to iterate through the array of PressureInfo structures	*/
	for (i = 0; i < infoCount; i++){
		if ((pressureInfo[i].isValid) && (isFirstValid)){
			
			/* first low pass filter formula:	y_0 = x_0	*/
			pressureInfo[i].filteredPressure = pressureInfo[i].sensorPressure;
			isFirstValid = false;	// found the first valid in sequence, set flag to false
		}
		else if (pressureInfo[i].isValid){

			/* second low pass filter formula:	y_i = Beta * x_i + (1-Beta)y_i-1 , where 0 < Beta < 1	*/
			pressureInfo[i].filteredPressure = (beta * pressureInfo[i].sensorPressure) + (double)(1.00 - beta) * pressureInfo[i-1].filteredPressure;
		}
		else{
			isFirstValid = true;	// reset flag to 'true' 
		}
	}
}


/*	Receives a pointer to an array of PressureInfo structures, and the number of elements in the array.
	The function performs its analysis of each VALID filteredPressure member and stores its weather
	prediction into the forcast member*/	
void AnalyzeFilteredData(PressureInfo *pressureInfo, const unsigned infoCount){
	unsigned i = 0;					// used for iteration
	bool isFirstValid = true;	// flag used to identify the first valid array of a sequence
	
	/*	FOR loop to iterate through array of PressureInfo structures	*/
	for (i = 0; i < infoCount; i++){
		if ((pressureInfo[i].isValid)&&(isFirstValid)){
			pressureInfo[i].forecast = "--- Restart ---";
			isFirstValid = false;	// found the first valid array in sequence, set the flag to false
		}
		else if ((pressureInfo[i].isValid) && (pressureInfo[i].filteredPressure > UPPER_PRESSURE_THRESHOLD) && (pressureInfo[i].filteredPressure >= pressureInfo[i - 1].filteredPressure)){
			pressureInfo[i].forecast = "Fair";
		}
		else if ((pressureInfo[i].isValid) && (pressureInfo[i].filteredPressure > UPPER_PRESSURE_THRESHOLD) && (pressureInfo[i].filteredPressure < pressureInfo[i - 1].filteredPressure)){
			pressureInfo[i].forecast = "Cloudy";
		}
		else if ((pressureInfo[i].isValid) && (pressureInfo[i].filteredPressure < LOWER_PRESSURE_THRESHOLD) && (pressureInfo[i].filteredPressure >= pressureInfo[i - 1].filteredPressure)){
			pressureInfo[i].forecast = "Clearing";
		}
		else if ((pressureInfo[i].isValid) && (pressureInfo[i].filteredPressure < LOWER_PRESSURE_THRESHOLD) && (pressureInfo[i].filteredPressure < pressureInfo[i - 1].filteredPressure)){
			pressureInfo[i].forecast = "Preciiptation or Storm";
		}
		else if ((pressureInfo[i].isValid) && (LOWER_PRESSURE_THRESHOLD < pressureInfo[i].filteredPressure < UPPER_PRESSURE_THRESHOLD) && (pressureInfo[i].filteredPressure >= pressureInfo[i - 1].filteredPressure)){
			pressureInfo[i].forecast = "No Change";
		}
		else if ((pressureInfo[i].isValid) && (LOWER_PRESSURE_THRESHOLD < pressureInfo[i].filteredPressure < UPPER_PRESSURE_THRESHOLD) && (pressureInfo[i].filteredPressure < pressureInfo[i - 1].filteredPressure)){
			pressureInfo[i].forecast = "Precipitation Likely";
		}
		else {
			// else statement for pressureInfo is not valid
			isFirstValid = true;	// reset the flag to deteremine the first valid array in sequence.
		}
	}
}


/*	Receives a pointer to an array of PressureInfo structures, the number of elements in the array, and 
	a pointer to a string containing the output file name. This function returs a bool that is true on success,
	false on failure	*/	
bool SavePressureReportToFile(const PressureInfo *pressureInfo, const unsigned infoCount, const char *pFilename){
	/*	Check conditions of the function perameters	*/
	if ((!pressureInfo) || (!pFilename) || (strlen(pFilename) == 0) || (infoCount <= 1)){
		return false;
	}

	/*	Open file for writing, verify fopen succeeded	*/
	FILE* outputFile = fopen(pFilename, "w");
	if (!outputFile){
		return false;
	}

	/*	print the column heading to the output file	*/
	fprintf(outputFile, "Record    Sensor Pressure     Filtered Pressure     Forecast\n");

	/*	For loop that runs through all elements, check if element is valid.
		If valid, print appropriate data into the output file	*/
	unsigned i = 0;
	for (i = 0; i < infoCount; i++){
		if (pressureInfo[i].isValid){
			fprintf(outputFile, "%6d %14.3lfinHG %17.3lfinHg     %-s\n", i, pressureInfo[i].sensorPressure, pressureInfo[i].filteredPressure, pressureInfo[i].forecast);
		}
	}

	fclose(outputFile);
	outputFile = NULL;
	return true;
}