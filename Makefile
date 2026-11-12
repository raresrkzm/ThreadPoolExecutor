build:
	gcc tema2.c -o tema2 -Wall -g -std=c11
run:
	./tema2 in_file out_file
clean:
	rm -f *.o tema2