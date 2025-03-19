### Establish the operating system name
ifeq ($(OS),Windows_NT)
	ifneq ($(COMP),ndk)
		target_windows = yes
	endif
else ifeq ($(COMP),mingw)
	target_windows = yes
else
	KERNEL = $(shell uname -s)
	ifeq ($(KERNEL),Linux)
		OS = $(shell uname -o)
	endif
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
		  ./thread.o ./logger.o ./timeman.o ./tt.o ./perft.o


kojiro: $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^ -pthread

debug: $(OBJECTS)
	$(CXX) $(CXXFLAGS_DEBUG) -o $@ $^ -pthread

clean:
	@rm -f kojiro kojiro.exe *.o 

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... main (the default if no target is provided)"
	@echo "... debug"
	@echo "... clean"
