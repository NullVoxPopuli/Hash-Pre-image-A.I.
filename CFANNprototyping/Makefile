
cfann : main.o fann_utils.o
	g++ main.o fann_utils.o -lfann -L/usr/local/lib -o cfann

fann_utils.o: fann_utils.cpp fann_utils.h
	g++ -c fann_utils.cpp

main.o: main.cpp main.h
	g++ -c main.cpp

clean: 
	rm -rf *o cfann