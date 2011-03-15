from pyfann import libfann
import hashlib

def train_network ():
    connection_rate = 1
    learning_rate = 0.7
    num_input = 512
    num_neurons_hidden = 4
    num_output = 128
    
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

    print("the input and the output are " + ( "the same" if (input == calc_out)  else "not the same"))

    ann.destroy()

#Make an input file containing the list of values given and their hashed values
def create_input_file(input):
    f = open('md5.data', 'w')
    #Write the header "#of_cases #of_inputs #of_outputs
    f.write(str(len(input)) + ' 512 128\n')
    for word in input:
        m = hashlib.md5()
        m.update(word)
        store_hex_as_binary(m.hexdigest(), f)
        store_hex_as_binary(pad_word(word), f);
    
def store_hex_as_binary(val, file):
    #Store the hex value as 1's and -1's into the given file
    first_char_written = False;
    line_to_write = ""
    for char in val:
        if first_char_written:
            line_to_write += " "
        if char == '0' or char == '1' or char == '2' or char == '3' or char == '4' or char == '5' or char == '6' or char == '7':
            line_to_write +=  "-1"
        else:
            line_to_write += "1"
        if char == '0' or char == '1' or char == '2' or char == '3' or char == '8' or char == '9' or char == '10' or char == '11':
            line_to_write += " -1"
        else:
            line_to_write += " 1"
        if char == '0' or char == '1' or char == '4' or char == '5' or char == '8' or char == '9' or char == '12' or char == '13':
            line_to_write += " -1"
        else:
            line_to_write += " 1"
        if char == '0' or char == '2' or char == '4' or char == '6' or char == '8' or char == '10' or char == '12' or char == '14':
            line_to_write += " -1"
        else:
            line_to_write += " 1"
        first_char_written = True
    file.write(line_to_write)
    file.write('\n');
    
#def read_hex_value_string(line):
    #Find the value of the hex value encoded in the string
#    value = ""
#    index = 0
#    chars = line.split(" ")
#    for val in chars:
#        if 
    
def pad_word(hexword):
    #Pad the hex input to the hash function with 0's to make it 512 bits long
    return hexword + ('0' * (128 - len(hexword)))
    
#create_input_file(['aaa', 'bbb', '111'])
f = open('md5.data', 'r')
#for line in f:
#    read_hex_value_string(line)

train_network()
lastline = []
params = True
for line in f:
    if params:
        params = False
        continue
    lastline = line.split(" ")
    print(len(lastline))

lastline = [int(num_string) for num_string in lastline]
test_network(lastline)