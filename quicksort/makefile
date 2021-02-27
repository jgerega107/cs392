sort: sort.o quicksort.o
	gcc -g sort.o quicksort.o -o sort
sort.o: sort.c
	gcc -g -pedantic-errors -c sort.c
quicksort.o: quicksort.c quicksort.h
	gcc -g -pedantic-errors -c quicksort.c
clean:
	rm -f *.o sort sort.exe