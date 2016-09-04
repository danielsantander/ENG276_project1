#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "PressureInfo.h"

unsigned GetFileLineCount(const char *pFilename){
	// checks if the file is not NULL and string length is greater than 0
	if ( !(pFilename) || !(strlen(pFilename) > 0) ){
		return 0;
	}

	FILE* inputFile = fopen(pFilename, "r");	// opens file for READING
	if (!inputFile){
		return 0;
	}

	int totalLines = 0;
	while (!feof(inputFile)){
		// when the file reaches a new line, increment totalLines
		if ( fgetc(inputFile) == '\n' ){
			totalLines++;
		}
	}

	fclose(inputFile);
	return totalLines;
}

PressureInfo *LoadSensorDataFromFile(const char *pFilename, const unsigned infoCount){
	if (!(pFilename) || (strlen(pFilename) == 0) || !(infoCount > 1)){
		return NULL;
	}

	// open file for reading, verify the open succeeded
	FILE* inputFile = fopen(pFilename, "r");
	if (!inputFile){
		return NULL;
	}

	// allocate a block of memory large enough for the number of PressureInfo structures specified by infoCount
	// verify malloc succeeded
	PressureInfo* pressureInput = NULL;
	pressureInput = (PressureInfo *) malloc(sizeof(PressureInfo)*infoCount);
	if (!pressureInput){
		return NULL;
	}

	unsigned int i = 0;				// used for iteration
	int scanResult = 0;				// used to verify sscanf succeeded
	for (i = 0;i<=infoCount; i++){
		scanResult = fscanf(inputFile, "%lf", &pressureInput[i].sensorPressure);
		if (scanResult != 1){
			free(pressureInput);
			fclose(inputFile);
			return NULL;
		}
		pressureInput[i].filteredPressure = 0.0;
		pressureInput[i].forecast = NULL;
		if ((pressureInput[i].sensorPressure >= LOWEST_PRESSURE_RECORD) && (pressureInput[i].sensorPressure <= HIGHEST_PRESSURE_RECORD)){
			pressureInput[i].isValid = true;
		}
		else{
			pressureInput[i].isValid = false;
		}
	}

	fclose(inputFile);
	return pressureInput;
}

void ApplyLowpassFilter(PressureInfo *pressureInfo, const unsigned infoCount, const double beta){

	// CONTINUE HERE!
	int i = 0;					// used for iteration through the array
	bool isFirstValid = true;	// flag used to identify the first valid array of a sequence

	for (i = 0; i < infoCount; i++){
		if ((pressureInfo[i].isValid) && (isFirstValid)){
			// first low pass filter formula:
			pressureInfo[i].filteredPressure = pressureInfo[i].sensorPressure;
			isFirstValid = false;	// found the first valid in sequence, set flag to false
		}
		else if ((pressureInfo[i].isValid) && (!isFirstValid)){
			// second low pass filter formula:
			pressureInfo[i].filteredPressure = (beta * pressureInfo[i].sensorPressure) + (double)(1.00 - beta) * pressureInfo[i].filteredPressure;
		}
		else{
			isFirstValid = true;	// reset flag to 'true' 
		}
	}
}