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

EXE = kojiro

CXX = g++
WFLAGS = -Wall -Wpedantic -Wextra -Wcast-qual -Wno-stringop-overflow -Wno-deprecated
AFLAGS = $(WFLAGS) -g -std=c++20
CFLAGS = -O3 $(AFLAGS) -flto -march=native
RFLAGS = -O3 $(AFLAGS) -flto
CXXFLAGS_DEBUG = -O1 $(AFLAGS) -Werror -DDEBUG

LDFLAGS =  -Wl,--whole-archive -lpthread -Wl,--no-whole-archive -lm

# Different instruction sets targeting different architectures. For the AVX sets, we consider them a series where each level contains
# all previous sets instructions

# Nehalem (2008)
SSE4FLAGS = -DUSE_SSE4 -msse -msse2 -msse3 -mssse3 -msse4 -msse4.1 -msse4.2
# Sandy Bridge (2011)
AVXFLAGS = -DUSE_AVX $(SSE4FLAGS) -mavx -mfma
# Haswell (2013)
AVX2FLAGS = -DUSE_AVX2 $(AVXFLAGS) -mavx2 -DUSE_POPCNT -mpopcnt
# Skylake (2017)
AVX512FLAGS = -DUSE_AVX512 $(AVX2FLAGS) -mavx512f -mavx512cd -mavx512vl -mavx512dq -mavx512bw
# Skylake (2019)
AVX512VNNIFLAGS = -DUSE_AVX512_VNNI $(AVX512FLAGS) -mavx512ifma -mavx512vbmi -mavx512vbmi2 -mavx512bitalg -mavx512vnni -mavx512vpopcntdq

# AMD processors before Zen 3 implement PEXT in microcode, so we support bmi as a optional orthogonal configuration, on AVX2 and newer
PEXTFLAGS = -DUSE_PEXT -mbmi -mbmi2

### static for publishing w/o the need of dlls for windows
ifeq ($(target_windows),yes)
	LDFLAGS += -static
endif

OBJECTS = ./kojiro.o ./bishop.o ./cons.o ./king.o ./move.o ./movegen.o ./position.o ./magic.o \
		  ./rook.o ./bitboard.o ./knight.o ./pawn.o ./queen.o ./uci.o ./eval.o ./search.o \
		  ./thread.o ./logger.o ./timeman.o ./tt.o ./perft.o


# Help
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "help"
	@echo "native"
	@echo "debug"
	@echo "clean"

.PHONY: help build native debug clean\
		legacy sse4 avx avx2 avx2-pext avx512 avx512-pext avx512vnni avx512vnni-pext release
#-------------------------------------------------------------------
# native builds optimized by supported instruction sets.

native: CXXFLAGS += $(CFLAGS)
native: LDFLAGS += -flto -pthread
native: build	

#-------------------------------------------------------------------
# debug build.

debug: $(OBJECTS)
	$(CXX) $(CXXFLAGS_DEBUG) -o  $^

#----------------------------------------------------------------------------------------------------------------------
# Release builds that are statically linked and target specific instruction sets

legacy: CXXFLAGS += $(RFLAGS)
legacy: LDFLAGS += -flto -static
legacy: EXE := $(addsuffix -legacy,$(EXE))
legacy: build

sse4: CXXFLAGS += $(RFLAGS) $(SSE4FLAGS)
sse4: LDFLAGS += -flto -static 
sse4: EXE := $(addsuffix -sse4,$(EXE))
sse4: build

avx: CXXFLAGS += $(RFLAGS) $(AVXFLAGS)
avx: LDFLAGS += -flto -static 
avx: EXE := $(addsuffix -avx,$(EXE))
avx: build

avx2: CXXFLAGS += $(RFLAGS) $(AVX2FLAGS)
avx2: LDFLAGS += -flto -static 
avx2: EXE := $(addsuffix -avx2,$(EXE))
avx2: build

avx2-pext: CXXFLAGS += $(RFLAGS) $(AVX2FLAGS) $(PEXTFLAGS)
avx2-pext: LDFLAGS += -flto -static 
avx2-pext: EXE := $(addsuffix -avx2-pext,$(EXE))
avx2-pext: build

avx512: CXXFLAGS += $(RFLAGS) $(AVX512FLAGS)
avx512: LDFLAGS += -flto -static 
avx512: EXE := $(addsuffix -avx512,$(EXE))
avx512: build

avx512-pext: CXXFLAGS += $(RFLAGS) $(AVX512FLAGS) $(PEXTFLAGS)
avx512-pext: LDFLAGS += -flto -static 
avx512-pext: EXE := $(addsuffix -avx512-pext,$(EXE))
avx512-pext: build

avx512vnni: CXXFLAGS += $(RFLAGS) $(AVX512VNNIFLAGS)
avx512vnni: LDFLAGS += -flto -static 
avx512vnni: EXE := $(addsuffix -avx512vnni,$(EXE))
avx512vnni: build

avx512vnni-pext: CXXFLAGS += $(RFLAGS) $(AVX512VNNIFLAGS) $(PEXTFLAGS)
avx512vnni-pext: LDFLAGS += -flto -static 
avx512vnni-pext: EXE := $(addsuffix -avx512vnni-pext,$(EXE))
avx512vnni-pext: build

#--------------------------------------------------------------------
build: $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $(EXE) $^

clean:
	@rm -f kojiro kojiro.exe kojiro-* *.o 

default:
	native	