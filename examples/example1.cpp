#include <include/magicSquare.h>
#include <include/sudoku.h>
#include <sudokus/testSudokus.h>
#include <utils/printSudoku.h>

#include <cassert>
#include <chrono>
#include <cstdarg>
#include <iostream>

static void printGrid(const Grid* grid) {
	printSudoku(grid->cells, grid->dim, grid->blockDimX, grid->blockDimY, NULL);
}

#if ENABLE_STATS
static void printStats(const SolveStats* stats) {
	printf("Num iterations: %d\n", stats->numIterations);
	printf("Max depth: %d\n", stats->maxDepth);
}
#endif

#if ENABLE_PROGRESS
static bool printProgress(int iteration) {
	if ((iteration % 1000) == 0) {
		printf(".");
	}
	if ((iteration % 80000) == 0) {
		printf("\n");
	}
	return true;
}
#endif

int main(int /*argc*/, char* /*argv*/[]) {
	Progress progressCbk = NULL;
	Rules rules = defaultRules();

#if 0 // 4x4
    Grid* grid = newGrid(gridSize_4);
    const int* sudoku = sudoku_4_00;
#elif 1
	Grid* grid = newGrid(gridSize_6);
	const int* sudoku = sudoku_6_00;
#elif 1 // 9x9
	Grid* grid = newGrid(gridSize_9);
#if 1
	const int* sudoku = sudoku_9_00;
#else
	const int* sudoku = sudoku_9_4;
	rules.diagonals = true;
	rules.magicSquare = true;
	rules.knights = true;
#endif
#else // 16x16
	Grid* grid = newGrid(gridSize_16);
	const int* sudoku = sudoku_16_00;
#endif

#if ENABLE_PROGRESS
	progressCbk = &printProgress;
#endif

	setGrid(grid, sudoku, -1);

	SolveStats stats;
	const auto startTime = std::chrono::steady_clock::now();
	const enum ErrorCode res = solveGrid(grid, &rules, &stats, &logError, progressCbk);
	const auto endTime = std::chrono::steady_clock::now();
	printf("\n");
	if (res == error_ok) {
		printGrid(grid);
		printf("\n");
		const auto elapsedMicros = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
		printf("Elapsed time: %.4f msec\n", static_cast<double>(elapsedMicros) / 1e3);
#if ENABLE_STATS
		printStats(&stats);
#endif
	}
	else if (res == error_noSolution) {
		printf("Grid has no solution\n");
	}
	else if (res == error_outOfMemory) {
		printf("Out of memory\n");
	}
	else if (res == error_invalidRules) {
		printf("Invalid rules\n");
	}

	deleteGrid(grid);
	return 0;
}
