


# reference: https://github.com/BStarcheus/compiler/blob/master/Makefile
C = clang
c = clang++
FLAGS = -std=c++11 -g -w

LLVM = `llvm-config --cxxflags --ldflags --system-libs --libs`
SRC = $(wildcard src/*.cpp)

main: $(SRC) runtime 
	$(c) $(FLAGS) $(SRC) -o main $(LLVM)
runtime: src/runtime.c 
	$(C) -c src/runtime.c -o runtime.o