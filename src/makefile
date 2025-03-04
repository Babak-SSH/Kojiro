CXX = g++
CXXFLAGS = -Wall -Werror -Wpedantic -Wextra -Wno-stringop-overflow -Ofast -std=c++20 -lrt -DUSE_PTHREADS
LDFLAGS = -lrt -DUSE_PTHREADS
CXXFLAGS_DEBUG = -Ofast -DDEBUG -g

OBJECTS = ./kojiro.o ./bishop.o ./cons.o ./king.o ./move.o ./movegen.o ./position.o ./magic.o \
		  ./rook.o ./bitboard.o ./knight.o ./pawn.o ./queen.o ./uci.o ./eval.o ./search.o \
		  ./thread.o ./logger.o ./timeman.o ./tt.o

TEST_OBJECTS = ../tests/perft.o ./bishop.o ./cons.o ./king.o ./move.o ./movegen.o ./position.o \
			   ./magic.o ./rook.o ./bitboard.o ./knight.o ./pawn.o ./queen.o ./uci.o ./eval.o \
			   ./search.o ./thread.o ./logger.o ./timeman.o ./tt.o


kojiro: $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^ -pthread

debug: $(OBJECTS)
	$(CXX) $(CXXFLAGS_DEBUG) -o $@ $^ -pthread

perft: $(TEST_OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^ -pthread

clean:
	rm ./debug
	rm ./kojiro
	rm ./perft

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... main (the default if no target is provided)"
	@echo "... debug"
	@echo "...perft"
	@echo "... clean"
