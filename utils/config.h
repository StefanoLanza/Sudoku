#pragma once


#ifdef __cplusplus
extern "C"
{
#endif

struct Rules;

typedef struct Config {
	int gridSize;
	int numbers[36 * 36];
	int numberCount;
	struct Rules* rules;
} Config;

int readConfigFromFile(Config* config, const char* fileName);
int writeConfigToFile(const Config* config, const char* fileName);

#ifdef __cplusplus
}
#endif
