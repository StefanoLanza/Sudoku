#pragma once

#include <wchar.h>
#include <include/config.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct Rules;

typedef struct Config {
	int gridSize;
	int numbers[maxGridCells];
	int numberCount;
	struct Rules* rules;
	int defaultAlphabet;
	int alphabetCount;
	wchar_t alphabet[maxGridSize];
} Config;

int readConfigFromFile(Config* config, const char* fileName);
int writeConfigToFile(const Config* config, const char* fileName);

#ifdef __cplusplus
}
#endif
