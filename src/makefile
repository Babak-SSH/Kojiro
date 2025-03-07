### Establish the operating system name
KERNEL = $(shell uname -s)
ifeq ($(KERNEL),Linux)
	OS = $(shell uname -o)
endif

### Target Windows OS
ifeq ($(OS),Windows_NT)
	ifneq ($(COMP),ndk)
		target_windows = yes
	endif
else ifeq ($(COMP),mingw)
	target_windows = yes
endif

CXX = g++
CXXFLAGS = -Wall -Werror -Wpedantic -Wextra -Wcast-qual -Wno-stringop-overflow -O3 -march=native -std=c++20 -flto -DUSE_PTHREADS
LDFLAGS =  -DUSE_PTHREADS
CXXFLAGS_DEBUG = -O3 -DDEBUG -g

### static for publishing w/o the need of dlls
ifeq ($(target_windows),yes)
	LDFLAGS += -static
endif

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
	@echo "... perft"
	@echo "... clean"
