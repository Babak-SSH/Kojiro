#ifndef CONS_H
#define CONS_H

#include <string>
#include <cstdint>


extern const char *convert_to_square[64];

extern const uint64_t A_file;
extern const uint64_t not_A_file;
extern const uint64_t not_H_file;
extern const uint64_t not_AB_file;
extern const uint64_t not_GH_file;

extern const uint64_t RANK1;
extern const uint64_t RANK2;
extern const uint64_t RANK3;
extern const uint64_t RANK4;
extern const uint64_t RANK5;
extern const uint64_t RANK6;
extern const uint64_t RANK7;
extern const uint64_t RANK8;

extern const int bishop_relevant_bits[64];
extern const int rook_relevant_bits[64];

extern const int castling_rights[64];

// ASCII pieces
extern const std::string ascii_pieces;

// default start position fen
extern const std::string START_FEN;

#endif