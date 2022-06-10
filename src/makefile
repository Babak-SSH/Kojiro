CXX = g++
CXXFLAGS = -Ofast -std=c++20 -lrt -DUSE_PTHREADS
LDFLAGS = -lrt -DUSE_PTHREADS
CXXFLAGS_DEBUG = -Ofast -g

OBJECTS = ./kojiro.o ./bishop.o ./cons.o ./king.o ./movegen.o ./position.o ./magic.o ./rook.o ./bitboard.o ./knight.o ./pawn.o ./queen.o ./uci.o ./eval.o ./search.o ./thread.o ./logger.o ./timeman.o ./tt.o

main: $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^ -pthread

debug: $(OBJECTS)
	$(CXX) $(CXXFLAGS_DEBUG) -o $@ $^

$(OBJECTS): ./bishop.h ./cons.h ./king.h ./movegen.h ./position.h ./magic.h ./rook.h ./bitboard.h ./knight.h ./pawn.h ./queen.h ./uci.h ./eval.h ./search.h ./logger.h ./thread.h ./timeman.h ./types.h ./misc.h ./tt.h

perft:
	g++ -Ofast ../tests/perft.cpp ./bishop.cpp ./cons.cpp ./king.cpp ./movegen.cpp ./position.cpp ./magic.cpp ./rook.cpp ./bitboard.cpp ./knight.cpp ./pawn.cpp ./queen.cpp ./eval.cpp ./search.cpp ./thread.cpp ./logger.cpp ./timeman.cpp -o perft

clean:
	rm ./debug
	rm ./main
	rm ./perft

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... main (the default if no target is provided)"
	@echo "... debug"
	@echo "...perft"
	@echo "... clean"