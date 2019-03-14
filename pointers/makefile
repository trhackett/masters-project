
GCC = g++ -g -std=c++14 -Wall -Wextra
VALGRIND = valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all

main: LeaklessPtr.h main.cpp
	$(GCC) main.cpp -o main

mem-test: main
	$(VALGRIND) ./main

sim: simulation.h simulation.cpp simulate.cpp LeaklessPtr.h
	$(GCC) simulation.cpp simulate.cpp -o sim

sim-mem: sim
	$(VALGRIND) ./sim

clean:
	rm main sim