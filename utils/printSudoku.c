#include "printSudoku.h"
#include <locale.h>
#include <stdio.h>
#include <wchar.h>

static void printRow(int cols, int sep, wchar_t defaultc, wchar_t firstc, wchar_t lastc, wchar_t sep0_c, wchar_t sep1_c) {
	wprintf(L"%lc", firstc);
	for (int x = 1; x < cols - 1; ++x) {
		wchar_t c = defaultc;
		if (x % sep == 0) {
			c = sep0_c;
		}
		else if (x % 4 == 0) {
			c = sep1_c;
		}
		wprintf(L"%lc", c);
	}
	wprintf(L"%lc\n", lastc);
}

static wchar_t numberToASCII(int number, int dim) {
	if (number < 1 || number > dim) {
		return L' ';
	}
	return (number <= 9 ? '0' : ('A' - 10)) + number;
}

void printSudoku(const int* t, int dim, int blockDimX, int blockDimY) {
	setlocale(LC_ALL, "");
	const int xSep = blockDimX * 4;
	const int ySep = blockDimY * 2;
	const int xdim = dim * 4 + 1;
	const int ydim = dim * 2 + 1;

	printRow(xdim, xSep, L'═', L'╔', L'╗', L'╦', L'╤');
	for (int y = 1; y < ydim - 1; ++y) {
		if (y % ySep == 0) {
			printRow(xdim, xSep, L'═', L'╠', L'╣', L'╬', L'╪');
		}
		else if (y % 2 == 0) {
			printRow(xdim, xSep, L'─', L'╟', L'╢', L'╫', L'┼');
		}
		else {
			for (int x = 0; x < xdim; ++x) {
				if ((x - 2) % 4 == 0) {
					wchar_t c = numberToASCII(*t, dim);
					wprintf(L"%lc", c);
					t++;
				}
				else if (x % xSep == 0) {
					wprintf(L"%lc", L'║');
				}
				else if (x % 4 == 0) {
					wprintf(L"%lc", L'│');
				}
				else {
					wprintf(L"%lc", L' ');
				}
			}
			wprintf(L"%lc", L'\n');
		}
	}
	printRow(xdim, xSep, L'═', L'╚', L'╝', L'╩', L'╧');
}

void logError(enum GridCode code, int col, int row, int number) {
	const char* errStr[] = {
		"col", "row", "square", "left diagonal", "right diagonal", "knight", "assigned", "magic square",
	};
	printf("Invalid cell (code: %s) [%d, %d] number: %d\n", errStr[code], col, row, number);
}
