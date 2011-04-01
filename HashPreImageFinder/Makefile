all: hPif clean

hPif : src/main.o src/fann_utils.o src/hashes.o src/config.o
	g++ src/main.o src/fann_utils.o src/hashes.o src/config.o -lfann -L/usr/local/lib -o hPif

config.o : src/Config.cpp src/Config.h
	g++ -c src/Config.cpp

hashes.o : src/hashes.cpp src/hashes.h
	g++ -c src/hashes.cpp

fann_utils.o: src/fann_utils.cpp fann_utils.h src/Config.h src/hashes.h
	g++ -c src/fann_utils.cpp -lfann -L/usr/local/lib 

main.o: src/main.cpp src/Config.h src/main.h src/hashes.h
	g++ -c src/main.cpp

clean: 
	rm -rf src/*o
	rm -rf *o