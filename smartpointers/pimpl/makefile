
GCC = g++ -g -std=c++14 -Wall -Wextra
VALGRIND = valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all

main: SmartPtr.h BasePtr.h UniquePtr.h SharedPtr.h main.cpp
	$(GCC) main.cpp -o main

test: main
	./main
	$(VALGRIND) ./main

clean:
	rm main sim

sim: simulate.cpp simulation.h simulation.cpp SmartPtr.h UniquePtr.h SharedPtr.h
	$(GCC) simulation.cpp simulate.cpp -o sim

test-sim: sim
	$(VALGRIND) ./sim