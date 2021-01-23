#include <include/magicSquare.h>
#include <include/sudoku.h>
#include <iostream>
#include <sudokus/testSudokus.h>

#define CATCH_CONFIG_RUNNER
#include <Catch-master/single_include/catch2/catch.hpp>

static bool compareGrids(const int* lhs, const int* rhs, int gridSize) {
	return ! memcmp(lhs, rhs, sizeof(int) * gridSize * gridSize);
}

int __cdecl main(int argc, char* argv[]) {
	return Catch::Session().run(argc, argv);
}

TEST_CASE("Magic Square") {
	SECTION("Valid") {
		// clang-format off
		const int magicSquare[] = { 
			2, 7, 6, 
			9, 5, 1, 
			4, 3, 8 };
		// clang-format on
		CHECK(checkMagicSquare(magicSquare, 3, 3));
	}

	SECTION("Invalid") {
		// clang-format off
		const int magicSquare[] = { 
			2, 7, 6, 
			9, 5, 2, 
			4, 3, 8 };
		// clang-format on
		CHECK_FALSE(checkMagicSquare(magicSquare, 3, 3));
	}
}

TEST_CASE("Sudoku") {
	Grid* grid = NULL;
	Rules rules;
	setDefaultRules(&rules);
	enum ErrorCode res;
	const int* solution = NULL;
	SECTION("4x4") {
		grid = newGrid(gridSize_4);
		setGrid(grid, sudoku_4_00, -1);
		res = solveGrid(grid, &rules, NULL, NULL, NULL);
		solution = sudoku_4_00_sol;
	}
	SECTION("6x6") {
		grid = newGrid(gridSize_6);
		setGrid(grid, sudoku_6_00, -1);
		res = solveGrid(grid, &rules, NULL, NULL, NULL);
		solution = sudoku_6_00_sol;
	}
	SECTION("9x9") {
		grid = newGrid(gridSize_9);
		setGrid(grid, sudoku_9_00, -1);
		res = solveGrid(grid, &rules, NULL, NULL, NULL);
		solution = sudoku_9_00_sol;
	}
#if LARGE_GRIDS
	SECTION("16x16") {
		grid = newGrid(gridSize_16);
		setGrid(grid, sudoku_16_00, -1);
		res = solveGrid(grid, &rules, NULL, NULL, NULL);
		solution = sudoku_16_00_sol;
	}
#endif
	CHECK(res == error_ok);
	CHECK(compareGrids(grid->cells, solution, grid->dim));
	deleteGrid(grid);
}
