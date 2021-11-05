all: interpretator clear

interpretator: main.o interpretator.o
	g++ main.o interpretator.o -o main

main.o: 
	g++ main.cpp -c

interpretator.o:
	g++ interpretator.cpp -c

clear:
	rm -rf *.o