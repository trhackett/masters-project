
GCC = g++ -g -std=c++14 -Wall -Wextra
VALGRIND = valgrind --tool=memcheck --leak-check=yes

main: SmartPtr.h BasePtr.h UniquePtr.h SharedPtr.h main.cpp
	$(GCC) main.cpp -o main

test: main
	./main
	$(VALGRIND) ./main

clean:
	rm main