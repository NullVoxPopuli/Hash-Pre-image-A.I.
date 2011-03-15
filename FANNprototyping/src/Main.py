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
        fann_binary = convert_binary_to_FANN_format(change_to_binary(m.hexdigest()))
        f.write(fann_binary + "\n")
        
        fann_binary = convert_binary_to_FANN_format(change_to_binary(pad_word(word)))
        f.write(fann_binary + "\n")

    
    
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
    
    
def change_to_binary(original, current_base):
    return bin(int(original, current_base))[2:]

def change_to_hex(original, current_base):
    return hex(int(original, current_base))

def convert_binary_to_FANN_format(binary_string):
    binary_string = binary_string.replace("1", "1 ")
    binary_string = binary_string.replace("0", "-1 ")
    return binary_string

def convert_FANN_format_to_binary(fann_string):
    fann_string = fann_string.replace("-1 ", "0")
    fann_string = fann_string.replace("1 ", "1")
    return fann_string

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