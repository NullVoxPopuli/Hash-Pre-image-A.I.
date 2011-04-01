LIBS = -L/usr/local/lib -lfann -lGL -lGLU -lm -lglut -framework GLUT -framework OpenGL

all: hPif clean

hPif : src/main.o src/fann_utils.o src/hashes.o src/Config.o fann_gpu.o
	g++ -o hPif src/main.o src/fann_utils.o src/hashes.o src/Config.o $(LIBS)

fann_gpu : src/fann_gpu.cpp src/fann_gpu.h
	g++ -c src/fann_gpu.cpp

Config.o : src/Config.cpp src/Config.h
	g++ -c src/Config.cpp

hashes.o : src/hashes.cpp src/hashes.h
	g++ -c src/hashes.cpp

fann_utils.o: src/fann_utils.cpp fann_utils.h src/hashes.h src/Config.h
	g++ -c src/fann_utils.cpp 

main.o: src/main.cpp src/main.h src/hashes.h src/Config.h src/fann_gpu.h
	g++ -c src/main.cpp

clean: 
	rm -rf src/*o
	rm -rf *o