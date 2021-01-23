#include "sudoku.h"

#if EXTENDED_RULES
#include "magicSquare.h"
#endif

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define invalidNumber 0
#if LARGE_GRIDS
typedef uint32_t Mask;
#else
typedef uint16_t Mask;
#endif

typedef struct GridTemp {
	int dim;
	int squareDimX;
	int squareDimY;
	int completed;
	int numCells;
#if LARGE_GRIDS
	int  cells[25 * 25];
	Mask cellMask[25 * 25];
#else
	int  cells[9 * 9];
	Mask cellMask[9 * 9];
#endif
	int firstCellIdx;
} GridTemp;

typedef struct Alloc {
	char*  memory;
	size_t size;
	size_t offset;
} Alloc;

#ifdef _MSC_VER
#include <intrin.h>

static inline int __builtin_ctzl(unsigned x) {
	unsigned long ret = 0;
	_BitScanForward(&ret, x);
	return (int)ret;
}

#endif

static int getCol(int cellIdx, const GridTemp* grid) {
	return (int)cellIdx % grid->dim;
}

static int getRow(int cellIdx, const GridTemp* grid) {
	return (int)cellIdx / grid->dim;
}

static int nextCellIndex(int cellIdx, const GridTemp* grid) {
	int next = cellIdx + 1;
	if (next == grid->numCells) {
		next = 0;
	}
	return next;
}

static void maskSequence(GridTemp* bt, int number, int firstCell, int nextCell) {
	const Mask andMask = ~(1u << number);
	for (int i = 0, cell = firstCell; i < bt->dim; ++i, cell += nextCell) {
		bt->cellMask[cell] &= andMask;
	}
}

static inline int getCellIndex(int col, int row, int gridDim) {
	return row * gridDim + col;
}

static inline bool isNumberAssigned(Mask cellMask, int number) {
	return (cellMask & (1u << number)) == 0;
}

static bool checkSequence(const GridTemp* bt, int number, int firstCell, int nextCell) {
	Mask accumMask = 0;
	for (int i = 0, cell = firstCell; i < bt->dim; ++i, cell += nextCell) {
		accumMask |= bt->cellMask[cell];
	}
	return isNumberAssigned(accumMask,
	                        number); // true if no cell can contain the number
}

static Mask checkCell(const GridTemp* bt, int col, int row) {
	return bt->cellMask[bt->dim * row + col];
}

static void maskCell(GridTemp* bt, int col, int row, Mask andMask) {
	bt->cellMask[bt->dim * row + col] &= andMask;
}

static void maskRow(GridTemp* bt, int row, int number) {
	maskSequence(bt, number, row * bt->dim, 1);
}

static void maskColumn(GridTemp* bt, int col, int number) {
	maskSequence(bt, number, col, bt->dim);
}

static void maskBlock(GridTemp* bt, int col, int row, int number) {
	const int  scol = (col / bt->squareDimX) * bt->squareDimX;
	const int  srow = (row / bt->squareDimY) * bt->squareDimY;
	const Mask andMask = ~(1u << number);
	for (int r = srow; r < srow + bt->squareDimY; ++r) {
		for (int c = scol; c < scol + bt->squareDimX; ++c) {
			maskCell(bt, c, r, andMask);
		}
	}
}

static bool checkRow(const GridTemp* bt, int row, int number) {
	return checkSequence(bt, number, row * bt->dim, 1);
}

static bool checkColumn(const GridTemp* bt, int col, int number) {
	return checkSequence(bt, number, col, bt->dim);
}

static bool checkBlock(const GridTemp* bt, int col, int row, int number) {
	const int scol = (col / bt->squareDimX) * bt->squareDimX;
	const int srow = (row / bt->squareDimY) * bt->squareDimY;
	Mask      accumMask = 0;
	for (int r = srow; r < srow + bt->squareDimY; ++r) {
		for (int c = scol; c < scol + bt->squareDimX; ++c) {
			accumMask |= bt->cellMask[bt->dim * r + c];
		}
	}
	return isNumberAssigned(accumMask, number);
}

#if EXTENDED_RULES

static void maskDiagonals(GridTemp* bt, int col, int row, int number) {
	if (col == row) {
		maskSequence(bt, number, 0, bt->dim + 1);
	}
	if (col + row == bt->dim - 1) {
		maskSequence(bt, number, bt->dim - 1, bt->dim - 1);
	}
}

static void maskKnights(GridTemp* bt, int col, int row, int number) {
	const int  d = bt->dim;
	const Mask andMask = ~(1u << number);
	if (row >= 2) {
		if (col >= 1) {
			maskCell(bt, col - 1, row - 2, andMask);
		}
		if (col <= d - 2) {
			maskCell(bt, col + 1, row - 2, andMask);
		}
	}
	if (row <= d - 3) {
		if (col >= 1) {
			maskCell(bt, col - 1, row + 2, andMask);
		}
		if (col <= d - 2) {
			maskCell(bt, col + 1, row + 2, andMask);
		}
	}
	if (col >= 2) {
		if (row >= 1) {
			maskCell(bt, col - 2, row - 1, andMask);
		}
		if (row <= d - 2) {
			maskCell(bt, col - 2, row + 1, andMask);
		}
	}
	if (col <= d - 3) {
		if (row >= 1) {
			maskCell(bt, col + 2, row - 1, andMask);
		}
		if (row <= d - 2) {
			maskCell(bt, col + 2, row + 1, andMask);
		}
	}
}

static bool checkKnights(const GridTemp* bt, int col, int row, int number) {
	const int d = bt->dim;
	Mask      accumMask = 0;
	if (row >= 2) {
		if (col >= 1) {
			accumMask |= checkCell(bt, col - 1, row - 2);
		}
		if (col <= d - 2) {
			accumMask |= checkCell(bt, col + 1, row - 2);
		}
	}
	if (row <= d - 3) {
		if (col >= 1) {
			accumMask |= checkCell(bt, col - 1, row + 2);
		}
		if (col <= d - 2) {
			accumMask |= checkCell(bt, col + 1, row + 2);
		}
	}
	if (col >= 2) {
		if (row >= 1) {
			accumMask |= checkCell(bt, col - 2, row - 1);
		}
		if (row <= d - 2) {
			accumMask |= checkCell(bt, col - 2, row + 1);
		}
	}
	if (col <= d - 3) {
		if (row >= 1) {
			accumMask |= checkCell(bt, col + 2, row - 1);
		}
		if (row <= d - 2) {
			accumMask |= checkCell(bt, col + 2, row + 1);
		}
	}
	return isNumberAssigned(accumMask, number);
}

static bool checkLeftDiagonal(const GridTemp* bt, int col, int row, int number) {
	if (col == row) {
		return checkSequence(bt, number, 0, bt->dim + 1);
	}
	return false;
}

static bool checkRightDiagonal(const GridTemp* bt, int col, int row, int number) {
	if (col + row == bt->dim - 1) {
		return checkSequence(bt, number, bt->dim - 1, bt->dim - 1);
	}
	return false;
}

static bool checkMiddleMagicSquare(const int* cells, int squareDim, int pitch, int col, int row) {
	const int scol = (col / squareDim) * squareDim;
	const int srow = (row / squareDim) * squareDim;
	// Only middle square
	if (scol != squareDim || srow != squareDim) {
		return true;
	}
	return checkMagicSquare(cells + pitch * srow + scol, squareDim, pitch);
}

#endif

// TODO replace recursion with iterations. Alloc internally
static int solveIter(GridTemp* bt, const Rules* rules, int cellIdx, SolveStats* stats, int depth, Progress progress, Alloc* alloc) {
#if ENABLE_STATS
	// Update stats
	if (depth > stats->maxDepth) {
		stats->maxDepth = depth;
	}
	stats->numIterations++;
#endif
#if ENABLE_PROGRESS
	if (progress) {
		// Inform user of progress
		if (! progress(stats->numIterations)) {
			return -1; // Abort
		}
	}
#endif

	const int nextCellIdx = nextCellIndex(cellIdx, bt);
	const int col = getCol(cellIdx, bt);
	const int row = getRow(cellIdx, bt);

	// Iterate over candidates for this cell
	const Mask candidates = bt->cellMask[cellIdx];
	Mask bitset = candidates;
	while (bitset != 0) {
		const int number = __builtin_ctzl(bitset);
		assert(number >= 1 && number <= bt->dim);
		bitset ^= (bitset & -bitset);
		{
			assert(! checkRow(bt, row, number));
			assert(! checkColumn(bt, col, number));
			assert(! checkBlock(bt, col, row, number));

		#if EXTENDED_RULES
			if (rules->diagonals) {
				assert(! checkLeftDiagonal(bt, col, row, number));
				assert(! checkRightDiagonal(bt, col, row, number));
			}
		#endif

			bt->cells[cellIdx] = number;
		#if EXTENDED_RULES
			if (rules->magicSquare) {
				if (! checkMiddleMagicSquare(bt->cells, bt->squareDimX, bt->dim, col, row)) {
					// Revert
					bt->cells[cellIdx] = invalidNumber;
					continue;
				}
			}
		#endif

			const size_t maskSize = (sizeof(Mask) * bt->numCells);
			assert(alloc->offset + maskSize <= alloc->size);
			Mask* state = (Mask*)(alloc->memory + alloc->offset);
			alloc->offset += maskSize;

			memcpy(state, bt->cellMask, sizeof(Mask) * bt->numCells);
			maskRow(bt, row, number);
			maskColumn(bt, col, number);
			maskBlock(bt, col, row, number);
		#if EXTENDED_RULES
			if (rules->diagonals) {
				maskDiagonals(bt, col, row, number);
			}
			if (rules->knights) {
				maskKnights(bt, col, row, number);
			}
		#endif
			++bt->completed;
			if (bt->completed == bt->numCells) {
				return 1;
			}

			int res = 0;
			if (nextCellIdx != bt->firstCellIdx) {
				res = solveIter(bt, rules, nextCellIdx, stats, depth + 1, progress, alloc);
			}

			assert(alloc->offset >= maskSize);
			alloc->offset -= maskSize;

			if (res == 1) {
				return 1;
			}
			else if (res == 0) {
				// Revert
				bt->cells[cellIdx] = invalidNumber;
				memcpy(bt->cellMask, state, sizeof(Mask) * bt->numCells);
				--bt->completed;
				// Try next candidate number
			}
			else {
				return -1; // aborted
			}
		}
	}
	return 0; // invalid
}

static int initBacktrackingData(GridTemp* bt, const Grid* grid, const Rules* rules, Logger logger) {
	bt->dim = grid->dim;
	bt->squareDimX = grid->blockDimX;
	bt->squareDimY = grid->blockDimY;
	bt->completed = 0;
	bt->numCells = grid->dim * grid->dim;
	if (rules->magicSquare) {
		bt->firstCellIdx = getCellIndex(grid->blockDimX, grid->blockDimY, grid->dim);
	}
	else {
		bt->firstCellIdx = 0; // TODO new strategies
	}

	(void)rules;
	memcpy(bt->cells, grid->cells, sizeof(int) * bt->numCells);

	Mask allMask = 0;
	for (int i = 1; i <= grid->dim; ++i) {
		allMask |= (1u << i);
	}

	for (int r = 0; r < bt->numCells; ++r) {
		bt->cellMask[r] = allMask;
	}

	int res = 1;
	for (int row = 0, cell = 0; row < grid->dim; ++row) {
		for (int col = 0; col < grid->dim; ++col, ++cell) {
			if (grid->cells[cell] == invalidNumber) {
				continue;
			}
			const int number = grid->cells[cell];

			if (isNumberAssigned(bt->cellMask[cell], number)) {
				res = 0;
			}

#if EXTENDED_RULES
			if (rules->magicSquare) {
				if (! checkMiddleMagicSquare(grid->cells, grid->blockDimX, grid->dim, col, row)) {
					if (logger) {
						logger(gridCode_magicSquare, col, row, number);
					}
					res = 0;
				}
			}
#endif
			if (checkRow(bt, row, number)) {
				if (logger) {
					logger(gridCode_row, col, row, number);
				}
				res = 0;
			}
			if (checkColumn(bt, col, number)) {
				if (logger) {
					logger(gridCode_col, col, row, number);
				}
				res = 0;
			}
			if (checkBlock(bt, col, row, number)) {
				if (logger) {
					logger(gridCode_square, col, row, number);
				}
				res = 0;
			}
#if EXTENDED_RULES
			if (rules->diagonals && checkLeftDiagonal(bt, col, row, number)) {
				if (logger) {
					logger(gridCode_left_diagonal, col, row, number);
				}
				res = 0;
			}
			if (rules->diagonals && checkRightDiagonal(bt, col, row, number)) {
				if (logger) {
					logger(gridCode_right_diagonal, col, row, number);
				}
				res = 0;
			}
			if (rules->knights && checkKnights(bt, col, row, number)) {
				if (logger) {
					logger(gridCode_knight, col, row, number);
				}
				res = 0;
			}
#endif
			if (res) {
				maskRow(bt, row, number);
				maskColumn(bt, col, number);
				maskBlock(bt, col, row, number);
#if EXTENDED_RULES
				if (rules->diagonals) {
					maskDiagonals(bt, col, row, number);
				}
				if (rules->knights) {
					maskKnights(bt, col, row, number);
				}
#endif
				bt->cellMask[cell] = (1u << number);
			}
		}
	}
	return res;
}

Grid* newGrid(enum GridSize size) {
	static const int dim[] = {
		4,
		6,
		9,
#if LARGE_GRIDS
		16,
		25
#endif
	};
	static const int blockDimX[] = {
		2,
		3,
		3,
#if LARGE_GRIDS
		4,
		5
#endif
	};
	static const int blockDimY[] = {
		2,
		2,
		3,
#if LARGE_GRIDS
		4,
		5
#endif
	};
	Grid* grid = malloc(sizeof(Grid));
	if (NULL == grid) {
		return NULL;
	}
	grid->dim = dim[size];
	grid->blockDimX = blockDimX[size];
	grid->blockDimY = blockDimY[size];
	grid->cells = malloc(sizeof(int) * grid->dim * grid->dim);
	if (NULL == grid->cells) {
		free(grid);
		return NULL;
	}
	memset(grid->cells, 0, sizeof(int) * grid->dim * grid->dim);
	return grid;
}

void deleteGrid(Grid* grid) {
	if (grid) {
		free(grid->cells);
		free(grid);
	}
}

void setDefaultRules(Rules* rules) {
	(void)rules;
#if EXTENDED_RULES
	rules->diagonals = false;
	rules->knights = false;
	rules->magicSquare = false;
#endif
}

void setGrid(Grid* grid, const int numbers[], int numberCount) {
	int count = grid->dim * grid->dim;
	if (numberCount < 0) {
		numberCount = count;
	}
	else if (count > numberCount) {
		count = numberCount;
	}
	for (int i = 0; i < count; ++i) {
		int n = numbers[i];
		if (n < 1 || n > grid->dim) {
			n = invalidNumber;
		}
		grid->cells[i] = n;
	}
}

enum ErrorCode solveGrid(Grid* grid, const Rules* rules, SolveStats* stats, Logger logger, Progress progress) {
	GridTemp bt;

	if (rules->magicSquare && grid->dim != 9) {
		return error_invalidRules;
	}

	const size_t maxIterations = grid->dim * grid->dim;
	Alloc        alloc;
	alloc.size = sizeof(Mask) * (grid->dim * grid->dim) * maxIterations;
	alloc.memory = malloc(alloc.size);
	if (alloc.memory == NULL) {
		return error_outOfMemory;
	}
	alloc.offset = 0;

	SolveStats localStats;
	if (stats == NULL) {
		stats = &localStats;
	}

	int ec = error_noSolution;
	if (initBacktrackingData(&bt, grid, rules, logger)) {
		stats->numIterations = 0;
		stats->maxDepth = 0;
		const int res = solveIter(&bt, rules, bt.firstCellIdx, stats, 0, progress, &alloc);
		if (res == 1) {
			memcpy(grid->cells, bt.cells, sizeof(int) * bt.numCells);
			ec = error_ok;
		}
		else {
			ec = error_noSolution;
		}
	}
	else {
		ec = error_invalidConfig;
	}

	free(alloc.memory);
	return ec;
}
