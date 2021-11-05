all: interpretator python clear

python: 
	rm -rf simple/*
	python3 s.py

interpretator: main.o interpretator.o
	g++ main.o interpretator.o -g -o main

main.o: 
	g++ main.cpp -c -g

interpretator.o:
	g++ interpretator.cpp -c -g

clear:
	rm -rf *.o