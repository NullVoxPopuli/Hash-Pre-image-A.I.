# Setup

    sudo apt-get install g++ libboost-all-dev libfann-dev

# Build

    make


## Help

    Available Options: 
         -mTrnData 123      [50000] Max Number of Training Data Sets
         -genTrain      Generates the training file to train the neural network with
         -train <mode>      Trains the network using the specified training mode
         -test 1010101001   Runs a trained network with the given input (should be the hash you
                     are trying to find the pre-image for) [Needs to be Implemented]
         -autoTest strt end n    Auto test a trained newwork between two values for n number of values.
         -file          Instead of standard out, outputting to the console, including this will have
                     all output go to a file with the date and time


    Selecting Hashes: 
         -kennys_hash_16     [default] simple 16 bit hash
         -kennys_hash_8      simple 8 bit hash
         -prestons_hash_8    8 bit hash based off murmur
         -murmur         32 bit hash

         -add_one        test to see if network can undo addition
         -x_squared      test to see if nework can undo squaring
         -lookup_table       test to see if network con learn a lookup table
         -multiply_halves    slightly more complicated than simple multiplication

    Training Modes: 
         block      Trains the network using dynamically generated, random data
         file       Trains the network using a pre-generated data file


    Network Configuration: 
         -nnil in,...,out   Number of neurons in each layer
         -nb        [16] Number of bits the network should expect for the input


    Network Training Constants:
         -de 0.00001        [1e-05] Desired Error. You may use 'auto' instead of a number.
         -lrate 0.73        [0.7] Learning Rate
         -epochs 10000      [5000] Number of training iterations (epochs)
         -reports 500       [1000]After how many epochs should should the training report each time?


    Developer Options: 
         -bypass    Used for skipping all normal functionality and for testing specific functions


     Example: ./hPif -genTrain -train -test 1111000011110000
             Generates the training file, trains the network, and tests the network (assuming the 
                network is set up for a 16 bit hash) with the value F0F0

    * Numbers contained in brackets ( [number] ) are the default value...
