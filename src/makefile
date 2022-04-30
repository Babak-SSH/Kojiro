CXX = g++
CXXFLAGS_MAIN = -Ofast -std=c++20
CXXFLAGS_DEBUG = -Ofast -g

OBJECTS = ./kojiro.o ./bishop.o ./cons.o ./king.o ./movegen.o ./position.o ./magic.o ./rook.o ./bitboard.o ./knight.o ./pawn.o ./queen.o ./uci.o ./eval.o ./search.o

main: $(OBJECTS)
	$(CXX) $(CXXFLAGS_MAIN) -o $@ $^

debug: $(OBJECTS)
	$(CXX) $(CXXFLAGS_DEBUG) -o $@ $^

$(OBJECTS): ./bishop.h ./cons.h ./king.h ./movegen.h ./position.h ./magic.h ./rook.h ./bitboard.h ./knight.h ./pawn.h ./queen.h ./uci.h ./eval.h ./search.h ./logger.h

perft:
	g++ -Ofast ../tests/perft.cpp ./bishop.cpp ./cons.cpp ./king.cpp ./movegen.cpp ./position.cpp ./magic.cpp ./rook.cpp ./bitboard.cpp ./knight.cpp ./pawn.cpp ./queen.cpp ./eval.cpp ./search.cpp -o perft

clean:
	rm ./debug
	rm ./main
	rm ./perft