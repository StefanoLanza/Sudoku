#include "printSudoku.h"
#include <locale.h>
#include <stdio.h>

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

static const wchar_t defaultAlphabet[36] = {
	L'1',L'2',L'3',L'4',L'5',L'6',L'7',L'8',L'9',
	L'A',L'B',L'C',L'D',L'E',L'F',L'G',L'H',L'I',
	L'J',L'K',L'L',L'M',L'N',L'O',L'P',L'Q',L'R',
	L'S',L'T',L'U',L'V',L'W',L'X',L'Y',L'Z'
};


static wchar_t numberToASCII(int number, int dim, const wchar_t* alphabet) {
	if (number < 1 || number > dim) {
		return L' ';
	}
	return alphabet[number - 1];
}

void printSudoku(const int* numbers, int dim, int blockDimX, int blockDimY, const wchar_t* alphabet) {
	if (alphabet == NULL) {
		alphabet = defaultAlphabet;
    }
	setlocale(LC_ALL, "");
	const int xSep = blockDimX * 4;
	const int ySep = blockDimY * 2;
	const int xdim = dim * 4 + 1;
	const int ydim = dim * 2 + 1;
	const int* t = numbers;

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
					wchar_t c = numberToASCII(*t, dim, alphabet);
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
