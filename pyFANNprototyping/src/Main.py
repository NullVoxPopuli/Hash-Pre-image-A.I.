from pyfann import libfann
from random import randint

def train_network ():
    connection_rate = .5
    learning_rate = .7
    num_input = 16
    num_neurons_hidden = 1000
    num_output = 16
    
    desired_error = 0.0001
    max_iterations = 1000
    iterations_between_reports = 50
    
    ann = libfann.neural_net()
    ann.create_sparse_array(connection_rate, (num_input, num_neurons_hidden, num_output))
    ann.set_learning_rate(learning_rate)
    ann.set_training_algorithm(libfann.TRAIN_BATCH)
    ann.set_activation_function_hidden(libfann.SIGMOID_SYMMETRIC)
    ann.set_activation_function_output(libfann.SIGMOID_SYMMETRIC)
    
    ann.train_on_file("sixteen_bit_hexes.data", max_iterations, iterations_between_reports, desired_error)
    
    print("Connection rate: %f, learning rate: %f, hidden neurons: %f" % (connection_rate, learning_rate, num_neurons_hidden))
    
    ann.save("sixteen_float.net")


def test_network (input):
    ann = libfann.neural_net()
    ann.create_from_file("sixteen_float.net")
    ann.set_activation_function_output(libfann.THRESHOLD_SYMMETRIC)

    calc_out = ann.run(convert_binary_to_FANN_list(pad_word(change_to_binary(hash_to_16_bits(input), 10))))
    
    print('ANN gives that you gave: ' + change_FANN_to_hex(calc_out))
    
    
#    print("the input and the output are " + ( "the same" if (input == calc_out)  else "not the same"))

    ann.destroy()

def create_input_file(numberOfValues):
    f = open('/home/skaggskd/workspace/FANNtest/data_file.fanndata', 'w')
    max = 256
    i = 0
    f.write(str(numberOfValues) + ' 8 8\n')
#    while (i < numberOfValues):
    for value_to_use in range(256):
        i += 1
#        value_to_use = str(randint(0, max))
        value_to_use = str(value_to_use)
        hashed_value = hash_to_16_bits(value_to_use)
        fann_binary_input = convert_binary_to_FANN_format(pad_word(change_to_binary(hashed_value, 10)))
        fann_binary_output = convert_binary_to_FANN_format(pad_word(change_to_binary(value_to_use, 10)))
#        f.write(hex(int(value_to_use, 10)) + '\n')
        f.write(fann_binary_input + '\n')
#        f.write(hex(int(hashed_value, 10)) + '\n')
        f.write(fann_binary_output + '\n')
        

#Make an input file containing the list of values given and their hashed values
#def create_input_file(input):
#    f = open('md5.data', 'w')
#    #Write the header "#of_cases #of_inputs #of_outputs
#    f.write(str(len(input)) + ' 16 16\n')
#    for word in input:
#        hashed_word = hash_to_16_bits(word)
#        print hashed_word
#        fann_binary = convert_binary_to_FANN_format(pad_word(change_to_binary(hashed_word, 10)))
#        f.write(fann_binary + "\n")
#        
#        fann_binary = convert_binary_to_FANN_format(pad_word(change_to_binary(word, 16)))
#        f.write(fann_binary + "\n")

  
#temporary so that we can prove our NN works.  
def hash_to_16_bits(word):
    hash = str(myhash(word))
    print(hex(int(word, 10)) + ' -> ' + hex(int(hash, 10)))
    return hash #2^16 - 1

def myhash(word):
    secondhalf = int(word, 10) >> 4
    firsthalf = int(word, 10) & 0x0f
    secondhalf |= firsthalf
    return (firsthalf << 4 | secondhalf)
    
def pad_word(bin_word):
    #Pad the hex input to the hash function with 0's to make it 512 bits long
    return ('0' * (8 - len(bin_word))) + bin_word
    
    
def change_to_binary(original, current_base):
    return bin(int(original, current_base))[2:]

def change_to_hex(original, current_base):
    return hex(int(original, current_base))

def change_FANN_to_hex(output_list):
    for index, object in enumerate(output_list):
        if (object == -1):
            output_list[index] = 0

    bin_string = ''
    for element in output_list:
        bin_string += str(int(element))
    return change_to_hex(bin_string, 2)
    

def convert_binary_to_FANN_format(binary_string):
    binary_string = binary_string.replace("1", "1 ")
    binary_string = binary_string.replace("0", "0 ")
    return binary_string
#test_network('AAAA')
def convert_binary_to_FANN_list(binary_string):
    list = []
    for char in binary_string:
        if (char == '0'):
            list.append(-1)
        else:
            list.append(1)
    return list

def convert_FANN_format_to_binary(fann_string):
    fann_string = fann_string.replace("-1 ", "0")
    fann_string = fann_string.replace("1 ", "1")
    return fann_string


create_input_file(256)

#train_network()

#test_network('1234')
#test_network([1, -1, -1, 1, 1, -1, 1, 1, -1, -1, 1, -1, -1, -1, -1, -1])