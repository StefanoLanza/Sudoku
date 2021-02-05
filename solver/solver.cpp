#include <include/magicSquare.h>
#include <include/sudoku.h>
#include <utils/printSudoku.h>
#include <utils/fileConfig.h>

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdarg>
#include <iostream>

struct Options {
	bool printSolutionAsArray;
};

static void defaultOptions(Options* options) {
	options->printSolutionAsArray = false;
}

static int parseOptions(Options* options, int argc, const char* argv[]) {
	if (argc < 2) {
		return 0;
	}
	if (argc > 2) {
		if (! strcmp(argv[2], "-printSolutionAsArray")) {
			options->printSolutionAsArray = true;
		}
	}
	return 1;
}

static void printGrid(const Grid* grid, const wchar_t* alphabet) {
	printSudoku(grid->cells, grid->dim, grid->blockDimX, grid->blockDimY, alphabet);
}

#if ENABLE_STATS
static void printStats(const SolveStats* stats) {
	printf("Num iterations: %d\n", stats->numIterations);
	printf("Max depth: %d\n", stats->maxDepth);
}
#endif

static void printUsage() {
	printf("Usage: sudoku_solver configFile [-printSolutionAsCString]\n");
}

static void printConfig(const Config& config) {
	printf("Grid size: %d\n", config.gridSize);
	if (config.defaultAlphabet) {
		printf("Alphabet: default\n");
	}
	else {
		printf("Alphabet: ");
		for (int i = 0; i < config.alphabetCount; ++i) {
			printf("%c,", config.alphabet[i]);
		}
		printf("\n");
	}
	printf("Rules:\n");
	printf("  Diagonals:    %s\n", config.rules->diagonals ? "true" : "false");
	printf("  Knights:      %s\n", config.rules->knights ? "true" : "false");
	printf("  Magic square: %s\n", config.rules->magicSquare ? "true" : "false");
}

static void printSolutionAsArray(const Grid* grid) {
	printf("const int solution[] = {\n");
	const int* nptr = grid->cells;
	for (int y = 0; y < grid->dim; ++y) {
		printf("\t");
		for (int x = 0; x < grid->dim; ++x, ++nptr) {
			int n = *nptr;
			printf("%c,", n < 10 ? ('0' + n) : ('A' + n - 10));
		}
		printf("\n");
	}
	printf("};\n");
}

int __cdecl main(int argc, const char* argv[]) {
	printf("Sudoku Solver\n");
	printf("© Stefano Lanza - steflanz@gmail.com\n\n");

	if (argc < 2) {
		printUsage();
		return 0;
	}
	
	Options options;
	defaultOptions(&options);
	if (0 == parseOptions(&options, argc, argv)) {
		return 0;
	}
	
	Rules rules;
	setDefaultRules(&rules);

	Config config;
    config.rules = &rules;

	const char* iniFile = argv[1];
	if (! readConfigFromFile(&config, iniFile)) {
		return 0;
	}

	printConfig(config);

	GridSize gridSize;
	if (config.gridSize == 4) {
		gridSize = gridSize_4;
	}
	else if (config.gridSize == 6) {
		gridSize = gridSize_6;
	}
	else if (config.gridSize == 9) {
		gridSize = gridSize_9;
	}
	else if (config.gridSize == 16) {
		gridSize = gridSize_16;
	}
	else if (config.gridSize == 25) {
		gridSize = gridSize_25;
	}
	else {
		printf("Invalid grid size %d", config.gridSize);
		return 0;
	}
	Grid* grid = newGrid(gridSize);
	setGrid(grid, config.numbers, config.numberCount);

	printf("\nInput grid\n");
	printSudoku(grid->cells, grid->dim, grid->blockDimX, grid->blockDimY, config.defaultAlphabet ? NULL : config.alphabet);

	SolveStats stats;
	const auto startTime = std::chrono::steady_clock::now();
	const enum ErrorCode res = solveGrid(grid, &rules, &stats, &logError, NULL);
	const auto endTime = std::chrono::steady_clock::now();
	printf("\n");
	printf("Computing...\n");
	if (res == error_ok) {
		printf("Output grid\n");
		printGrid(grid, config.defaultAlphabet ? NULL : config.alphabet);
		printf("\n");
		const auto elapsedMicros = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
		printf("Elapsed time: %.4f msec\n", static_cast<double>(elapsedMicros) / 1e3);
#if ENABLE_STATS
		printStats(&stats);
#endif
		if (options.printSolutionAsArray) {
			printf("\n");
			printSolutionAsArray(grid);
		}
	}
	else if (res == error_invalidConfig) {
		printf("Error: Invalid input grid\n");
	}
	else if (res == error_noSolution) {
		printf("Grid has no solution\n");
	}
	else if (res == error_outOfMemory) {
		printf("Error: Out of memory\n");
	}
	else if (res == error_invalidRules) {
		printf("Error: Invalid rules\n");
	}

	deleteGrid(grid);
	return 0;
}
