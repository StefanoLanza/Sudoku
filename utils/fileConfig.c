#include "fileConfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <include/sudoku.h>
#include <inih/ini.h>

#define PARSE_INT(varName, var)     \
	if (! strcmp(name, #varName)) { \
		var = atoi(value);     \
	}

#define PARSE_BOOL(varName, var)                    \
	if (! strcmp(name, #varName)) {                 \
		var = strcmp(value, "true") ? false : true; \
	}

static bool isSeparator(char c) {
	return c == ' ' || c == ',';
}

static int parseNumbers(int* numbers, int oldCount, const char* str) {
	int d = oldCount;
	for (int i = 0, l = (int)strlen(str); i < l; ++i) {
		if (! isSeparator(str[i])) {
			numbers[d] = (str[i] >= 'A') ? ((str[i] - 'A') + 10) : (str[i] - '0');
			++d;
			if (d >= maxGridCells) {
				break;
			}
		}
	}
	return d;
}

static int parseAlphabet(wchar_t* alphabet, int oldCount, const char* str) {
	int d = oldCount;
	for (int i = 0, l = (int)strlen(str); i < l; ++i) {
		if (! isSeparator(str[i])) {
			alphabet[d] = str[i];
			++d;
			if (d >= maxGridSize) {
				break;
			}
		}
	}
	return d;
}

static int INIParser(void* user, const char* section, const char* name, const char* value) {
	(void)section;
	Config* config = (Config*)user;
	PARSE_INT(gridSize, config->gridSize);
	PARSE_BOOL(knights, config->rules->knights);
	PARSE_BOOL(diagonals, config->rules->diagonals);
	PARSE_BOOL(magicSquare, config->rules->magicSquare);
	if (! strcmp(name, "numbers")) {
		config->numberCount = parseNumbers(config->numbers, config->numberCount, value);
	}
	if (! strcmp(name, "alphabet")) {
		config->alphabetCount = parseAlphabet(config->alphabet, config->alphabetCount, value);
	}
	return 1;
}

#undef PARSE_INT
#undef PARSE_BOOL

static void setDefaultConfig(Config* config) {
	config->gridSize = 9;
	config->numberCount = 0;
	config->defaultAlphabet = 1;
	config->alphabetCount = 0;
	memset(config->numbers, 0, sizeof config->numbers);
	memset(config->alphabet, 0, sizeof config->alphabet);
}

int readConfigFromFile(Config* config, const char* fileName) {
	setDefaultConfig(config);
	int err = ini_parse(fileName, INIParser, config);
	if (err == -1) {
		printf("Failed to open file %s\n", fileName);
	}
	else if (err != 0) {
		printf("Failed to parse file %s\n", fileName);
	}
	if (config->alphabetCount > 0) {
		if (config->alphabetCount == config->gridSize) {
  			config->defaultAlphabet = 0;
  		}
		else {
			printf("Incomplete parsing of alphabet in file %s\n", fileName);
		}
	}
	return err == 0;
}

int writeConfigToFile(const Config* config, const char* fileName) {
	FILE* file = fopen(fileName, "w");
	if (! file) {
		return 0;
	}
	fprintf(file, "gridSize: %d", config->gridSize);
	fprintf(file, "knights: %d", config->rules->knights);
	fprintf(file, "diagonals: %d", config->rules->diagonals);
	fprintf(file, "magicSquare: %d", config->rules->magicSquare);
	return 1;
}

