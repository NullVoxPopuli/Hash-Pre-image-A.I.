all: hPif clean

hPif : src/main.o src/fann_utils.o src/hashes.o
	g++ src/main.o src/fann_utils.o src/hashes.o -lfann -L/usr/local/lib -o hPif

hashes.o : src/hashes.cpp src/hashes.h
	g++ -c src/hashes.cpp

fann_utils.o: src/fann_utils.cpp fann_utils.h src/config.h src/hashes.h
	g++ -c src/fann_utils.cpp -lfann -L/usr/local/lib 

main.o: src/main.cpp src/config.h src/main.h src/hashes.h
	g++ -c src/main.cpp

clean: 
	rm -rf src/*o
	rm -rf *o