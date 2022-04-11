#ifndef PERFT_H
#define PERFT_H

#include <stdlib.h>
#include <sys/time.h>

#include "../src/bitboard.h"
#include "../src/magic.h"
#include "../src/types.h"
#include "../src/position.h"
#include "../src/pawn.h"
#include "../src/knight.h"
#include "../src/king.h"
#include "../src/bishop.h"
#include "../src/rook.h"
#include "../src/queen.h"
#include "../src/movegen.h"

int get_time_ms();

void init_all();

static inline void perft_driver(int depth);

void perft_test(int depth);

#endif