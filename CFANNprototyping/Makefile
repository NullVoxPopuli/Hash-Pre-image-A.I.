all: cfann clean

cfann : src/main.o src/fann_utils.o
	g++ src/main.o src/fann_utils.o -lfann -L/usr/local/lib -o cfann

fann_utils.o: src/fann_utils.cpp fann_utils.h src/config.h
	g++ -c fann_utils.cpp -lfann -L/usr/local/lib 

main.o: src/main.cpp src/config.h src/main.h
	g++ -c src/main.cpp

clean: 
	rm -rf src/*o
	rm -rf *o