####################################################################################
# PATHS and system dependent logic
####################################################################################
#mac boost is in opt/local/lib because of macports
MAC_BOOST_PATH = -L/opt/local/lib/
LINUX_BOOST_PATH = -L/usr/lib/
DEFAULT_PATH = -L/usr/local/lib/
BOOST_PATH = $(DEFAULT_PATH)

UNAME=$(shell uname)
ifeq ($(UNAME), Darwin)
BOOST_PATH = $(MAC_BOOST_PATH)
endif
ifeq ($(UNAME), Linux)
BOOST_PATH = $(LINUX_BOOST_PATH)
endif

####################################################################################
# objects
####################################################################################
main_objects = src/main.o src/fann_utils.o src/Config.o

network_objects = src/neural_networks/basic.o src/neural_networks/shared.o src/neural_networks/swarm.o
hash_objects = src/hashes.o src/hashes/Murmur.o 

all_objects = $(main_objects) $(hash_objects) $(network_objects) 

####################################################################################
# libraries
####################################################################################
FANN = -lfann
BOOST_NETWORKING = -lboost_thread-mt
BOOST_FILESYSTEM = -lboost_filesystem
BOOST_CORE = -lboost_system

BOOST_FILE_IO = $(BOOST_CORE) $(BOOST_FILESYSTEM)

####################################################################################
# compile the actual code
####################################################################################

all: hPif clean


hPif : $(all_objects)
	g++ -o hPif $(all_objects) $(FANN) $(BOOST_FILE_IO) $(DEFAULT_PATH) $(BOOST_PATH)

shared.o : src/neural_networks/shared.cpp src/neural_network/shared.h
	g++ -c src/neural_networks/shared.cpp $(BOOST_FILE_IO)

swarm.o : src/neural_networks/swarm.cpp src/neural_network/swarm.h src/neural_network/shared.h
	g++ -c src/neural_networks/swarm.cpp $(BOOST_FILE_IO)

basic.o : src/neural_networks/basic.cpp src/neural_network/basic.h src/neural_network/shared.h
	g++ -c src/neural_networks/basic.cpp $(BOOST_FILE_IO)

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