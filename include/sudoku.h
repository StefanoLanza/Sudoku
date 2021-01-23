#pragma once

#include "config.h"
#include <stdint.h>
#include <stdbool.h>

enum GridSize {
    gridSize_4,
    gridSize_6,
    gridSize_9,
#if LARGE_GRIDS
    gridSize_16,
    gridSize_25
#endif
	// TODO gridSize_36
};

enum CharacterSet {
	characterSet_numbers,
	characterSet_letters
};

enum ErrorCode {
    error_ok = 0,
    error_outOfMemory,
    error_invalidRules,
    error_invalidConfig,
    error_noSolution
};

enum GridCode {
    gridCode_col,
    gridCode_row,
    gridCode_square,
    gridCode_left_diagonal,
    gridCode_right_diagonal,
    gridCode_knight,
    gridCode_assigned,
    gridCode_magicSquare
};

typedef struct Rules {
#if EXTENDED_RULES
    bool diagonals;
    bool knights;
    bool magicSquare;
#endif
} Rules;

typedef struct SolveStats {
    int numIterations;
    int maxDepth;
} SolveStats;

typedef void (*Logger)(enum GridCode errorCode, int col, int row, int number);
typedef bool (*Progress)(int iteration);

typedef struct Grid {
    int* cells;
    int  dim;
    int  blockDimX;
    int  blockDimY;
} Grid;

#ifdef __cplusplus
extern "C" {
#endif

Grid* newGrid(enum GridSize size);
void deleteGrid(Grid* grid);
void setDefaultRules(Rules* rules);
void setGrid(Grid* grid, const int numbers[], int numberCount);
enum ErrorCode solveGrid(Grid* grid, const Rules* rules, SolveStats* stats, Logger logger, Progress progress);

#ifdef __cplusplus
}
#endif
