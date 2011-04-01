all: hPif clean

hPif : src/main.o src/fann_utils.o src/hashes.o src/Config.o
	g++ -o hPif src/main.o src/fann_utils.o src/hashes.o src/Config.o -lfann -L/usr/local/lib 

Config.o : src/Config.cpp src/Config.h
	g++ -c src/Config.cpp

hashes.o : src/hashes.cpp src/hashes.h
	g++ -c src/hashes.cpp

fann_utils.o: src/fann_utils.cpp fann_utils.h src/hashes.h src/Config.h
	g++ -c src/fann_utils.cpp 

main.o: src/main.cpp src/main.h src/hashes.h src/Config.h 
	g++ -c src/main.cpp

clean: 
	rm -rf src/*o
	rm -rf *o