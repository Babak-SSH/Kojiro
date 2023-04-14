#ifndef POSITION_H
#define POSITION_H

#include <cstdint>
#include <string>
#include <cstring>
#include <sys/types.h>

#include "cons.h"
#include "bitboard.h"
#include "types.h"
#include "pawn.h"
#include "knight.h"
#include "bishop.h"
#include "rook.h"
#include "queen.h"
#include "king.h"
#include "move.h"
#include "magic.h"


namespace Kojiro {

namespace Zobrist {
	extern uint64_t psq[12][64];
	extern uint64_t enpassant[64];
	extern uint64_t castling[16];
	extern uint64_t side;
}

struct StateInfo {
    uint64_t bitboards[12];
    uint64_t occupancies[3];

    int castle;
    int side;
    int rule50;
    int enpassant;
    int play_count;
	u_int64_t key; // position hash key 

    StateInfo* previous;
};

// forward declaring Thread class to avoid including recursively
class Thread;

class Position {

    private:
        int ply;
        uint64_t repetitionTable[1000];
        int repetitionIndex;
        Thread* thisTh;
        StateInfo* st;

    public:
        /**
         * @brief Initialize hash keys.
         * 
         */
        static void init_hash();

        Position() = default;
        Position(const Position&) = delete;

        /**
         * @brief parse fen and set the variables.
         * 
         * @param fen 
         * @param state 
         * @param thread 
         * @return Position& 
         */
        Position& parse_fen(const std::string& fen, StateInfo* st, Thread* th);

        /**
         * @brief Get the fen string
         * 
         * @return std::string 
         */
        std::string get_fen();

        /**
         * @brief return an ASCII representation of the position.
         * 
         */
        void log_board();

        /**
         * @brief generate hash key.
         * 
         * @return uint64_t 
         */
        uint64_t generate_hash_key();

        /**
         * @brief take state back.
         * 
         */
        void take_back();

        /**
         * @brief check if side that will play is in check
         * 
         * @return true if the side that should play is in check
         * @return false if the side that should play is not in check 
         */
        bool is_check();

        /**
         * @brief do the move on that state.
         * 
         * @param move 
         * @param move_flag 
         * @param newST 
         * @return int 
         */
        int make_move(int move, int move_flag, StateInfo& newST, int depth=-1);

        /**
         * @brief Set the piece in given square on piece bitboard. 
         * 
         * @param p 
         * @param sq 
         */
        void set_piece(Piece p, int sq);
        
        /**
         * @brief  check if square is under attack.
         * 
         * @param square 
         * @param side 
         * @return true 
         * @return false 
         */
        bool is_square_attacked(int square, int side) const;

        int enpassant() const;
        int side() const;
        int castle() const;
        int play_count() const;
        u_int64_t bitboards(int p) const;
        u_int64_t occupancies(int color) const;
        u_int64_t key() const;
        int get_ply() const;
        uint64_t repetition_table(int index) const;
        int repetition_index() const;
        StateInfo* state() const;
        Thread* thread() const;
};

inline int Position::enpassant() const {
    return st->enpassant;
}

inline int Position::side() const {
    return st->side;
}

inline int Position::castle() const {
    return st->castle;
}

inline int Position::play_count() const {
    return st->play_count;
}

inline u_int64_t Position::bitboards(int p) const {
    return st->bitboards[p];
}

inline u_int64_t Position::occupancies(int color) const {
    return st->occupancies[color];
}

inline u_int64_t Position::key() const {
    return st->key;
}

inline int Position::get_ply() const {
    return ply;
}

inline uint64_t Position::repetition_table(int index) const {
    return repetitionTable[index];
}

inline int Position::repetition_index() const {
    return repetitionIndex;
}

inline StateInfo* Position::state() const {
    return st;
}

inline Thread* Position::thread() const {
    return thisTh;
}

/// @todo get_attacks function with piece parameter to get rid of army of ifs.(maybe template)
inline bool Position::is_square_attacked(int square, int side) const {
    // pawn
    if (pawn_attacks[!side][square] & st->bitboards[P+(6*side)]) return 1;
    // knight
    if (knight_attacks[square] & st->bitboards[N+(6*side)]) return 1;
    // bishop
    if (get_bishop_attacks(square, st->occupancies[NO_COLOR]) & st->bitboards[B+(6*side)]) return 1;
    // rook
    if (get_rook_attacks(square, st->occupancies[NO_COLOR]) & st->bitboards[R+(6*side)]) return 1;
    // queen
    if (get_queen_attacks(square, st->occupancies[NO_COLOR]) & st->bitboards[Q+(6*side)]) return 1;
    // king
    if (king_attacks[square] & st->bitboards[K+(6*side)]) return 1;

    return 0;
}

} // namespace Kojiro

#endif