#ifndef PERFT_H
#define PERFT_H

#include <stdlib.h>
#include <sys/time.h>

#include "bitboard.h"
#include "magic.h"
#include "types.h"
#include "move.h"
#include "position.h"
#include "pawn.h"
#include "knight.h"
#include "king.h"
#include "bishop.h"
#include "rook.h"
#include "queen.h"
#include "movegen.h"


/**
 * @brief Get the time in ms
 * 
 * @return int 
 */
int get_time_ms();

/**
 * @brief main recursive perft driver 
 * 
 * @param depth 
 */
void perft_driver(int depth, Kojiro::Position& pos);

/**
 * @brief perft test calling perft driver
 * 
 * @param depth 
 */
void perft(int depth, Kojiro::Position& pos);

int main(int argc, char* argv[]);

#endif