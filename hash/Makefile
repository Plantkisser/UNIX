all: final.out

final.out: main.o hash.o
	gcc --coverage -o $@ $^
CFLAGS = -MD --coverage
-include *.d

clean: 
	rm *.d *.o *.out *.gcov *.gcno *.gcda
