from pyfann import libfann
import hashlib

def train_network ():
    connection_rate = 1
    learning_rate = 0.7
    num_input = 16
    num_neurons_hidden = 128
    num_output = 16
    
    desired_error = 0.0001
    max_iterations = 100000
    iterations_between_reports = 1000
    
    ann = libfann.neural_net()
    ann.create_sparse_array(connection_rate, (num_input, num_neurons_hidden, num_output))
    ann.set_learning_rate(learning_rate)
    ann.set_activation_function_output(libfann.SIGMOID_SYMMETRIC_STEPWISE)
    
    ann.train_on_file("md5.data", max_iterations, iterations_between_reports, desired_error)
    
    ann.save("md5_float.net")


def test_network (input):
    ann = libfann.neural_net()
    ann.create_from_file("md5_float.net")
    ann.set_activation_function_output(libfann.THRESHOLD_SYMMETRIC)

    calc_out = ann.run(input)
    
    print(calc_out)
    print(change_FANN_to_hex(calc_out))
    
    
#    print("the input and the output are " + ( "the same" if (input == calc_out)  else "not the same"))

    ann.destroy()

#Make an input file containing the list of values given and their hashed values
def create_input_file(input):
    f = open('md5.data', 'w')
    #Write the header "#of_cases #of_inputs #of_outputs
    f.write(str(len(input)) + ' 16 16\n')
    for word in input:
        hashed_word = hash_to_16_bits(word)
        print hashed_word
        fann_binary = convert_binary_to_FANN_format(pad_word(change_to_binary(hashed_word, 10)))
        f.write(fann_binary + "\n")
        
        fann_binary = convert_binary_to_FANN_format(pad_word(change_to_binary(word, 16)))
        f.write(fann_binary + "\n")

  
  #temporary so that we can prove our NN works.  
def hash_to_16_bits(word):
    return str(hash(word)&65535) #2^16 - 1
    
def pad_word(bin_word):
    #Pad the hex input to the hash function with 0's to make it 512 bits long
    return bin_word + ('0' * (16 - len(bin_word)))
    
    
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
    binary_string = binary_string.replace("0", "-1 ")
    return binary_string

def convert_FANN_format_to_binary(fann_string):
    fann_string = fann_string.replace("-1 ", "0")
    fann_string = fann_string.replace("1 ", "1")
    return fann_string


#create_input_file(['aaa', 'bbb', '111'])

#train_network()

#test_network([1, 1, 1, 1, 1, 1, 1, -1, 1, -1, -1, 1, -1, 1, -1, -1])
test_network([1, -1, -1, 1, 1, -1, 1, 1, -1, -1, 1, -1, -1, -1, -1, -1])