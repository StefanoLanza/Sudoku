#include "magicSquare.h"

// FIXME defs.h
#define invalidNumber 0

static bool checkMagicSquareSequence(const int* cells, int squareDim, int pitch, int col, int row, int dcol, int drow, int sum) {
	int tmpSum = 0;
	int valid = 0;
	for (int i = 0; i < squareDim; ++i) {
		int c = cells[col + row * pitch];
		tmpSum += c;
		valid += c != invalidNumber;
		col += dcol;
		row += drow;
	}
	return ((valid == squareDim && tmpSum == sum) || (valid < squareDim && tmpSum < sum));
}

bool checkMagicSquare(const int* cells, int squareDim, int pitch) {
	if (squareDim <= 2) {
		return false; // does not exist
	}
	static const int sumTable[4] = {
		15,
		34,
		65,
		111,
	};
	const int sum = sumTable[squareDim - 3];
	// Check rows
	for (int i = 0; i < squareDim; ++i) {
		if (! checkMagicSquareSequence(cells, squareDim, pitch, 0, i, 1, 0, sum)) {
			return false;
		}
	}

	// Check columns
	for (int i = 0; i < squareDim; ++i) {
		if (! checkMagicSquareSequence(cells, squareDim, pitch, i, 0, 0, 1, sum)) {
			return false;
		}
	}

	// Check left diagonal
	if (! checkMagicSquareSequence(cells, squareDim, pitch, 0, 0, 1, 1, sum)) {
		return false;
	}

	// Check right diagonal
	if (! checkMagicSquareSequence(cells, squareDim, pitch, squareDim - 1, 0, -1, 1, sum)) {
		return false;
	}

	return true;
}
