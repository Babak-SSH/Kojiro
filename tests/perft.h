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

/**
 * @brief Get the time in ms
 * 
 * @return int 
 */
int get_time_ms();

/**
 * @brief initilize all bitboards in both color
 * 
 */
void init_all();

/**
 * @brief main recursive perft driver 
 * 
 * @param depth 
 */
static inline void perft_driver(int depth);

/**
 * @brief perft test calling perft driver
 * 
 * @param depth 
 */
void perft_test(int depth);

#endif