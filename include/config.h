#pragma once

// Set to 1 to enable 16x16 and 25x25 grids
#ifndef LARGE_GRIDS
#define LARGE_GRIDS 1
#endif

#ifndef EXTENDED_RULES
#define EXTENDED_RULES 1
#endif

#ifndef ENABLE_STATS
#define ENABLE_STATS 0
#endif

#ifndef ENABLE_PROGRESS
#define ENABLE_PROGRESS 0
#endif

#if LARGE_GRIDS
enum { maxGridSize = 25, maxGridCells = 25 * 25 };
#else
enum { maxGridSize = 9, maxGridCells = 9 * 9 };
#endif

