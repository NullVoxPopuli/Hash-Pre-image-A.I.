network_objects = src/neural_networks/basic.o src/neural_networks/shared.o src/neural_networks/swarm.o
main_objects = src/main.o src/fann_utils.o src/Config.o
hash_objects = src/hashes.o src/hashes/Murmur.o 

all: hPif clean

hPif : $(main_objects) $(hash_objects) $(network_objects) 
	g++ -o hPif $(main_objects) $(hash_objects) $(network_objects) -lfann -L/usr/local/lib 

shared.o : src/neural_networks/shared.cpp src/neural_network/shared.h
	g++ -c src/neural_networks/shared.cpp

swarm.o : src/neural_networks/swarm.cpp src/neural_network/swarm.h
	g++ -c src/neural_networks/swarm.cpp

basic.o : src/neural_networks/basic.cpp src/neural_network/basic.h
	g++ -c src/neural_networks/basic.cpp

Murmur.o : src/hashes/Murmur.cpp src/hashes/Murmur.h
	g++ -c src/hashes/Murmur.cpp

Config.o : src/Config.cpp src/Config.h
	g++ -c src/Config.cpp

hashes.o : src/hashes.cpp src/hashes.h
	g++ -c src/hashes.cpp

fann_utils.o: src/fann_utils.cpp fann_utils.h
	g++ -c src/fann_utils.cpp 

main.o: src/main.cpp src/main.h 
	g++ -c src/main.cpp

clean: 
	rm -rf src/neural_networks/*.o
	rm -rf src/hashes/*.o
	rm -rf src/*.o
	rm -rf *.o