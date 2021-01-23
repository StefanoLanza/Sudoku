**Sudoku** is a C library for solving Sudokus.

# FEATURES
- C API
- Optimized backtracking algorithm
- Supported sizes: 4, 6, 9, 16, 25
- Traditional Sudoku rules
- Extended rules: 
    - diagonals: both diagonals must also contain the digits
    - knights: cells that are a knight's move apart (in chess) cannot contain the same digit
    - magic square: the central box must form a magic square

# REQUIREMENTS
* A C compiler for the library
* A C++ 17 compiler for the unit test, example and solver application

# FOLDERS
* include: header files
* src: source files
* external: third party libraries
* examples: example project
* solver: solver application
* tests: unit test
* sudokus: test sudokus
* utils: utilities
* bin: sample configuration files

# INSTALLATION
* Clone the repository to a local folder:
  git clone https://github.com/StefanoLanza/Sudoku.git
* Install [Premake](https://premake.github.io/) 
* On Windows
  * Run ```premake5.exe vs201X``` (either vs2017 or vs2019) to generate a Visual Studio solution
  * Open build/vs2019/Sudoku.sln
  * Select a build configuration (Release, Debug, Win32, x64)
  * Build and run the Example1, Solver or UnitTest project
* On MacOS
  * Run ```premake xcode4``` to generate an XCode workspace
  * Open build/xcode4/Sudoku.xcworkspace
  * Select a scheme under Product/Scheme/
  * Build and run the selected scheme
* For integration in your own project
  * Add the contents of the include and src folder to your project build pipeline. Please see premake5.lua as a reference. 

# SOLVER
The repository contains a solver application. The application reads a configuration file which describes the rules, grid size and initial 
numbers. The application then attempts to solve the Sudoku using a backtracking algorithm. On success, it prints the complete grid. 

# TODO
- [ ] Write documentation
- [ ] Replace recursion with iteration in the solver and measure performance difference
