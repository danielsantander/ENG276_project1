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
