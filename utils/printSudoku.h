#pragma once

#include <include/sudoku.h>
#include <wchar.h>

#ifdef __cplusplus
extern "C"
{
#endif

void printSudoku(const int* numbers, int dim, int blockDimX, int blockDimY, const wchar_t* alphabet);
void logError(enum GridCode code, int col, int row, int number);

#ifdef __cplusplus
}
#endif
