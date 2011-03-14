from pyfann import libfann
import hashlib

def train_network ():
    connection_rate = 1
    learning_rate = 0.7
    num_input = 2
    num_neurons_hidden = 4
    num_output = 1
    
    desired_error = 0.0001
    max_iterations = 100000
    iterations_between_reports = 1000
    
    ann = libfann.neural_net()
    ann.create_sparse_array(connection_rate, (num_input, num_neurons_hidden, num_output))
    ann.set_learning_rate(learning_rate)
    ann.set_activation_function_output(libfann.SIGMOID_SYMMETRIC_STEPWISE)
    
    ann.train_on_file("xor.data", max_iterations, iterations_between_reports, desired_error)
    
    ann.save("xor_float.net")


def test_network (x, y):
    ann = libfann.neural_net()
    ann.create_from_file("xor_float.net")
    calc_out = ann.run([x, y])
    print(calc_out)
    ann.destroy()

#Make an input file containing the list of values given and their hashed values
def create_input_file(input):
    f = open('md5.data', 'w')
    #Write the header "#of_cases #of_inputs #of_outputs
    f.write(str(len(input)) + ' 512 128\n')
    for word in input:
        m = hashlib.md5()
        m.update(word)
        store_hex_bits(m.hexdigest(), f)
        store_hex_bits(pad_word(word), f);
    
def store_hex_bits(val, file):
    #Store the hex value as 1's and -1's into the given file
    first_char_written = False;
    for char in val:
        if first_char_written:
            file.write(" ")
        if char == '0' or char == '1' or char == '2' or char == '3' or char == '4' or char == '5' or char == '6' or char == '7':
            file.write("-1")
        else:
            file.write("1")
        if char == '0' or char == '1' or char == '2' or char == '3' or char == '8' or char == '9' or char == '10' or char == '11':
            file.write(" -1")
        else:
            file.write(" 1")
        if char == '0' or char == '1' or char == '4' or char == '5' or char == '8' or char == '9' or char == '12' or char == '13':
            file.write(" -1")
        else:
            file.write(" 1")
        if char == '0' or char == '2' or char == '4' or char == '6' or char == '8' or char == '10' or char == '12' or char == '14':
            file.write(" -1")
        else:
            file.write(" 1")
        first_char_written = True
    file.write('\n');
    
def read_hex_value_string(line):
    #Find the value of the hex value encoded in the string
    value = ""
    index = 0
    chars = line.split(" ")
    for val in chars:
        if 
    
def pad_word(hexword):
    #Pad the hex input to the hash function with 0's to make it 512 bits long
    return hexword + ('0' * (32 - len(hexword)))
    
#create_input_file(['aaa', 'bbb', '111'])
f = open('md5.data', 'r')
for line in f:
    read_hex_value_string(line)