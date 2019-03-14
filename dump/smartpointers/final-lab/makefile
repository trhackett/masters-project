
GCC = g++ -g -std=c++14 -Wall -Wextra
VALGRIND = valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all

ptrtest: SmartPtr.h BasePtr.h UniquePtr.h SharedPtr.h ptr-test.cpp
	$(GCC) ptr-test.cpp -o ptrtest

test-ptr: ptrtest
	./ptrtest
	$(VALGRIND) ./ptrtest

clean:
	rm sim ptrtest

sim: simulate.cpp simulation.h simulation.cpp SmartPtr.h UniquePtr.h SharedPtr.h
	$(GCC) simulation.cpp simulate.cpp -o sim

test-sim: sim
	$(VALGRIND) ./sim