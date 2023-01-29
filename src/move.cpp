#include "move.h"


namespace Kojiro {

/*
          binary move bits                               hexidecimal constants
    
    0000 0000 0000 0000 0011 1111    source square       0x3f
    0000 0000 0000 1111 1100 0000    target square       0xfc0
    0000 0000 1111 0000 0000 0000    piece               0xf000
    0000 1111 0000 0000 0000 0000    promoted piece      0xf0000
    0001 0000 0000 0000 0000 0000    capture flag        0x100000
    0010 0000 0000 0000 0000 0000    double push flag    0x200000
    0100 0000 0000 0000 0000 0000    enpassant flag      0x400000
    1000 0000 0000 0000 0000 0000    castling flag       0x800000
*/
/// @todo define structure or fluent interface
int encode_move(int source, int target, int piece, int promoted, int capture, int double_push, int enpassant, int castling){
    return ((source) |          \
    (target << 6) |     \
    (piece << 12) |     \
    (promoted << 16) |  \
    (capture << 20) |   \
    (double_push << 21) |    \
    (enpassant << 22) | \
    (castling << 23));
}

moveInfo decode_move(int move){
    moveInfo info;

    info.source = (move & 0x3f);
    info.target = ((move & 0xfc0) >> 6);
    info.piece = ((move & 0xf000) >> 12);
    info.promoted = ((move & 0xf0000) >> 16);
    info.capture = (move & 0x100000);
    info.double_push = (move & 0x200000);
    info.enpassant = (move & 0x400000);
    info.castling = (move & 0x800000);

    return info;
}

// get move ( used in UCI communication )
std::string get_move_string(int move)
{
    moveInfo info = decode_move(move);
	std::string move_string = "";
	std::stringstream ss;

	ss << convert_to_square[info.source] << convert_to_square[info.target];
    if (promoted_pieces[info.promoted])
        ss << info.promoted;
    ss << ' ';

	move_string = ss.str();

	return move_string;
}

// print move list (help to debug)
void print_move_list(moves *move_list)
{
    if (!move_list->count)
    {
        fmt::print("\n     No move in the move list!\n");
        return;
    }
    
    fmt::print("\n     move    piece     capture   double    enpass    castling\n\n");
    
    for (int move_count = 0; move_count < move_list->count; move_count++)
    {
        int move = move_list->moves[move_count];
        moveInfo info = decode_move(move);

        fmt::print("      %s%s%c   %c         %d         %d         %d         %d\n", convert_to_square[info.source],
                                                                                convert_to_square[info.target],
                                                                                info.promoted ? promoted_pieces[info.promoted] : ' ',
                                                                                ascii_pieces[info.piece],
                                                                                info.capture ? 1 : 0,
                                                                                info.double_push ? 1 : 0,
                                                                                info.enpassant ? 1 : 0,
                                                                                info.castling ? 1 : 0);
    }
    
    fmt::print("\n\n     Total number of moves: %d\n\n", move_list->count);
}
} // namespace Kojiro