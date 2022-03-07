/***********************************************\
=================================================
                QUEEN ATTACKS
=================================================
\***********************************************/


#include "queen.h"


// getting queen's attacks by mixing rook and bishop attacks.
uint64_t get_queen_attacks(int square, uint64_t occupancy) {
	return (get_bishop_attacks(square, occupancy) | get_rook_attacks(square, occupancy));
}
