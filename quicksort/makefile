#Name: Jacob Gerega
#Date: 3/2/21
#Pledge: I pledge my honor that I have abided by the Stevens Honor System.
sort: sort.o quicksort.o
	gcc -g sort.o quicksort.o -o sort
sort.o: sort.c
	gcc -g -pedantic-errors -c sort.c
quicksort.o: quicksort.c quicksort.h
	gcc -g -pedantic-errors -c quicksort.c
clean:
	rm -f *.o sort sort.exe