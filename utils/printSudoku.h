#pragma once

#include <include/sudoku.h>

#ifdef __cplusplus
extern "C"
{
#endif

	void printSudoku(const int* t, int dim, int blockDimX, int blockDimY);
	void logError(enum GridCode code, int col, int row, int number);

#ifdef __cplusplus
}
#endif
