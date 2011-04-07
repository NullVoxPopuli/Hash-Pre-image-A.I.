all: hPif clean

hPif : src/main.o src/fann_utils.o src/hashes.o src/Config.o src/hashes/Murmur.o
	g++ -o hPif src/main.o src/fann_utils.o src/hashes.o src/Config.o -lfann -L/usr/local/lib 

Murmur.o : src/hashes/Murmur.cpp src/hashes/Murmur.h
	g++ -c src/hashes/Murmur.cpp

Config.o : src/Config.cpp src/Config.h
	g++ -c src/Config.cpp

hashes.o : src/hashes.cpp src/hashes.h
	g++ -c src/hashes.cpp

fann_utils.o: src/fann_utils.cpp fann_utils.h src/hashes.h src/Config.h
	g++ -c src/fann_utils.cpp 

main.o: src/main.cpp src/main.h src/hashes.h src/Config.h 
	g++ -c src/main.cpp

clean: 
	rm -rf *o