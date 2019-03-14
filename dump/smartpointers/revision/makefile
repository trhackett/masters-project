
GCC = g++ -g -std=c++14 -Wall -Wextra
VALGRIND = valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all

main: Ptr.h main.cpp
	$(GCC) main.cpp -o main

mem-test: main
	$(VALGRIND) ./main

clean:
	rm main