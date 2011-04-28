main_objects = src/main.o src/fann_utils.o src/Config.o

network_objects = src/neural_network_basic.o
hash_objects = src/hashes.o src/hashes/Murmur.o 
cloud_objects = src/cloud/client.o src/cloud/server.o

all_objects = $(main_objects) $(hash_objects) $(network_objects) $(cloud_objects)

all: hPif clean

hPif : $(all_objects)
	g++ -o hPif $(all_objects) -lfann -L/usr/local/lib 

src/cloud/client.o : src/cloud/chat_client.cpp src/cloud/chat_message.hpp
	g++ -c src/cloud/chat_client.cpp -o $@

src/cloud/server.o : src/cloud/chat_server.cpp src/cloud/chat_message.hpp src/cloud/chat_server.h
	g++ -c src/cloud/chat_server.cpp -o $@

neural_network_basic.o : src/neural_network_basic.cpp src/neural_network_basic.h
	g++ -c src/neural_network_basic.cpp

hashes/Murmur.o : src/hashes/Murmur.cpp src/hashes/Murmur.h
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
	rm -rf src/cloud/*.o
	rm -rf src/hashes/*.o
	rm -rf src/*.o
	rm -rf *.o