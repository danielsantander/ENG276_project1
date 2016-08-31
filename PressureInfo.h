#ifndef PRESSUREINFO_H
#define	PRESSUREINFO_H

// preprocessors used throughout program:
#define HIGHEST_PRESSURE_RECORD	32.01
#define LOWEST_PRESSURE_RECORD 25.69
#define UPPER_PRESSURE_THRESHOLD 30.20
#define LOWER_PRESSURE_THRESHOLD 29.80

#include <stdbool.h>

typedef struct PressureInfo
{
	double sensorPressure;		// unfiltered input data
	double filteredPressure;	// filtered output data
	char   *forecast;			// weather forcast (null-terminated string)
	bool   isValid;				// false if data should be ignored
} PressureInfo;

unsigned GetFileLineCount(const char *pFilename);
PressureInfo *LoadSensorDataFromFile(const char *pFilename, const unsigned infoCount);
void ApplyLowpassFilter(PressureInfo *pressureInfo, const unsigned infoCount, const double beta);
void AnalyzeFilteredData(PressureInfo *pressureInfo, const unsigned infoCount);
bool SavePressureReportToFile(const PressureInfo *pressureInfo, const unsigned infoCount,
	const char *pFilename);

#endif	/* PRESSUREINFO_H */