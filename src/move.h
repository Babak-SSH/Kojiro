#ifndef MOVE_H
#define MOVE_H

#include <string>
#include <sstream>

#define FMT_HEADER_ONLY
#include "fmt/format.h"

#include "types.h"
#include "cons.h"


namespace Kojiro {

struct moveInfo
{
	/// source square
    int source;
	/// target square
    int target;
	/// piece type
    int piece;
	/// is_promoted
    int promoted;
	/// is_move_capture
    int capture;
	/// is_double_push
    int double_push;
	/// is_enpassant
    int enpassant;
	/// is_castling
    int castling;
};

/**
 * @brief encode move data
 * 
 * @param source 
 * @param target 
 * @param piece 
 * @param promoted 
 * @param capture 
 * @param double_push 
 * @param enpassant 
 * @param castling 
 * @return int 
 */
int encode_move(int source, int target, int piece, int promoted, int capture, int double_push, int enpassant, int castling);

/**
 * @brief decode move.
 * 
 * @param move 
 * @return moveInfo 
 */
moveInfo decode_move(int move);

/**
 * @brief Get move as human readable string.
 * 
 * @param move 
 * @return std::string 
 */
std::string get_move_string(int move);

/**
 * @brief print move list.
 * 
 * @param move_list 
 */
void print_move_list(moves *move_list);

/// @todo check if making these are good practice or not.
/// check for capture in move
inline int get_move_capture(int move){
	return (move & 0x100000);
}

/// check for promotion in move
inline int get_move_promoted(int move){
	return ((move & 0xf0000) >> 16);
}

} // namespace Kojiro

#endif