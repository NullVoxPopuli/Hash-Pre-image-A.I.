import math

state_mutable = 777
state_fixed = 778
state_changed = 779

class MeshNode:

    def __init__(self, val, mesh):
        self.value = val
        self.Mesh = mesh
        self.changeListeners = []
    
    def getValue(self):
        return self.value
    
    def setValue(self, val):
        self.value = val

class MeshLayer:
    
    def __init__(self):
        self.nodes = [MeshNode(False, self), MeshNode(False, self), MeshNode(False, self), MeshNode(False, self),
                      MeshNode(False, self), MeshNode(False, self), MeshNode(False, self), MeshNode(False, self),
                      MeshNode(False, self), MeshNode(False, self), MeshNode(False, self), MeshNode(False, self),
                      MeshNode(False, self), MeshNode(False, self), MeshNode(False, self), MeshNode(False, self),
                      
                      MeshNode(False, self), MeshNode(False, self), MeshNode(False, self), MeshNode(False, self),
                      MeshNode(False, self), MeshNode(False, self), MeshNode(False, self), MeshNode(False, self),
                      MeshNode(False, self), MeshNode(False, self), MeshNode(False, self), MeshNode(False, self),
                      MeshNode(False, self), MeshNode(False, self), MeshNode(False, self), MeshNode(False, self),
                      
                      MeshNode(False, self), MeshNode(False, self), MeshNode(False, self), MeshNode(False, self),
                      MeshNode(False, self), MeshNode(False, self), MeshNode(False, self), MeshNode(False, self),
                      MeshNode(False, self), MeshNode(False, self), MeshNode(False, self), MeshNode(False, self),
                      MeshNode(False, self), MeshNode(False, self), MeshNode(False, self), MeshNode(False, self),
                      
                      MeshNode(False, self), MeshNode(False, self), MeshNode(False, self), MeshNode(False, self),
                      MeshNode(False, self), MeshNode(False, self), MeshNode(False, self), MeshNode(False, self),
                      MeshNode(False, self), MeshNode(False, self), MeshNode(False, self), MeshNode(False, self),
                      MeshNode(False, self), MeshNode(False, self), MeshNode(False, self), MeshNode(False, self)]
        self.state = state_mutable
    
    def layerForNumber(number):
        resultLayer = MeshLayer()
        index = 0
    
        while number > 0:
            newNode = MeshNode(False, resultLayer)
            if number & 1 == 1:
                newNode.setValue(True)
            else:
                newNode.setValue(False)
            resultLayer.nodes[index] = newNode
        
            number >>= 1
            index += 1

        return resultLayer

    def toNumber(self):
        step = 1
        index = 1
        result = 0
        
        for node in self.nodes:
            if node.getValue():
                result += step
            step <<= 1
        
        return result;

class NotMeshNode(MeshNode):

    def __init__(self, a):
        self.A = a

    def getValue(self):
        return (not self.A.getValue())

class NotMesh(MeshLayer):
    
    def __init__(self, input_mesh):
        super(NotMesh, self).__init__()
        i = 0
        while i < 32:
            self.nodes[i] = NotMeshNode(input_mesh.nodes[i])
            i += 1

class AndMeshNode(MeshNode):
    
    def __init__(self, a, b):
        self.A = a
        self.B = b
    
    def getValue(self):
        return (self.A.getValue() and self.B.getValue())
    
    def setValue(self, val):
        self.value = val

class AndMesh(MeshLayer):
    
    def __init__(self, meshA, meshB):
        super(AndMesh, self).__init__()
        i = 0
        while i < 32:
            self.nodes[i] = AndMeshNode(meshA.nodes[i], meshB.nodes[i])
            i += 1

class OrMeshNode(MeshNode):

    def __init__(self, a, b):
        self.A = a
        self.B = b

    def getValue(self):
        return (self.A.getValue() or self.B.getValue())

class OrMesh(MeshLayer):

    def __init__(self, meshA, meshB):
        super(OrMesh, self).__init__()
        i = 0
        while i < 32:
            self.nodes[i] = OrMeshNode(meshA.nodes[i], meshB.nodes[i])
            i += 1

class AddMeshNode(MeshNode):

    def __init__(self, a, b, cb, mesh):
        super(AddMeshNode, self).__init__(-1, mesh)

        self.A = a
        self.A.changeListeners.append(self)
        
        self.B = b
        self.B.changeListeners.append(self)
        
        self.carry = False
        self.CB = cb
        self.value = -1
        self.Mesh = mesh

    def getValue(self):
        if self.value == -1:
            val = 0
            self.carry = False

            if self.A.getValue():
                val += 1
            if self.B.getValue():
                val += 1
    
            if self.CB is not None:
                if self.CB.value == -1:
                    self.CB.getValue()
                if self.CB.carry:
                    val += 1
    
            if val > 1:
                self.carry = True

            if val == 1 or val == 3:
                self.value = 1
            else:
                self.value = 0

        return self.value == 1
    
    def setShouldGiveCarry(self, giveCarry):
        if not (giveCarry ^ self.carry):
            print('anomaly 11380315')

        if self.carry:
            #need to stop giving a carry
            self.carry = False
            if self.value == 1:
                #giving a carry and a 1
                if self.CB.carry:
                    self.A.setValue(False)
                    self.B.setValue(False)
                else:
                    print('anomaly 11490315')
            else:
                #giving a carry and a 0
                if self.CB.carry:
                    ###need to set whichever's 1 to 0 and stop the carry that we're getting
                    if self.A.getValue():
                        self.A.setValue(False)
                    elif self.B.getValue():
                        self.B.setValue(False)
                    else:
                        print('anomaly 12060315')
                    self.CB.setShouldGiveCarry(False)
                else:
                    #both are 1
                    self.A.setValue(False)
                    self.B.setValue(False)
                        
        else:
            #need to give a carry
            self.carry = True
            if self.value == 1:
                #not giving a carry, but giving a 1
                if self.CB.carry:
                    self.A.setValue(True)
                    self.B.setValue(True)
                else:
                    ###need to set whichever's 0 to 1 and get a carry
                    if not self.A.getValue():
                        self.A.setValue(True)
                    elif not self.B.getValue():
                        self.B.setValue(True)
                    else:
                        print('anomaly 12070315')
                    self.CB.setShouldGiveCarry(True)
            else:
                # no carry and 0
                self.A.setValue(True)
                self.B.setValue(True)

    def adjust(self, cause):
        self.resolve()

    def setValue(self, val):
        if not (val ^ (self.value == 1) ):
            print('anomaly 11340315')
            return
        
        self.value = 1 if val else 0
        self.resolve()
        for node in self.changeListeners:
            node.resolve()

    def resolve(self):
        if not self.value:
            #need to give 0, were giving a 1
            if self.carry:
                #we were giving a carry and need to keep doing that
                #only possible state A:1 B:1 C:1
                self.A.setValue(False)
            else:
                #were not giving a carry
                if not self.CB.carry:
                    #not receiving a carry
                    #one of them was one and it needs to be zero
                    if self.A.getValue():
                        self.A.setValue(False)
                    elif self.B.getValue():
                        self.B.setValue(False)
                    else:
                        print('anomaly 10070315')
                else:
                    #receiving a carry
                    #can't give a carry, everything was 0 since we were giving a 1
                    #need to stop the carry to give a 0
                    self.CB.setShouldGiveCarry(False)

        else:
            #need to give 1, were giving a 0
            if self.carry:
                #we were giving a carry
                if self.CB.carry:
                    #A or B was 1 and we need them both to be
                    if not self.A.getValue():
                        self.A.setValue(True)
                    elif not self.B.getValue():
                        self.B.setValue(True)
                    else:
                        print('anomaly 10580315')
                else:
                    #A and B were 1, we need a carry
                    ###
                    self.CB.setShouldGiveCarry(True)
            else:
                #weren't giving a carry, both A and B were 0
                self.A.setValue(True)

class EmptyCB(MeshNode):
    
    def __init__(self):
        self.carry = False
        self.value = 0

class AddMesh(MeshLayer):

    def __init__(self, meshA, meshB):
        super(AddMesh, self).__init__()
        i = 0
        previousNode = EmptyCB()
        while i < 32:
            self.nodes[i] = AddMeshNode(meshA.nodes[i], meshB.nodes[i], previousNode, self)
            previousNode = self.nodes[i]
            i += 1

class LeftRotateMesh(MeshLayer):

    def __init__(self, input_mesh, amount):
        super(LeftRotateMesh, self).__init__()
        i = 0
        index = 32 - amount
        while i < 32:
            if index == 32:
                index = 0
            
            self.nodes[i] = input_mesh.nodes[index]
            index += 1
            i += 1

class XorMeshNode(MeshNode):
    def __init__(self, a, b):
        self.A = a
        self.B = b
        
    def getValue(self):
        return (self.A.getValue() ^ self.B.getValue())

class XorMesh(MeshLayer):
    
    def __init__(self, meshA, meshB):
        super(XorMesh, self).__init__()
        i = 0
        while i < 32:
            self.nodes[i] = XorMeshNode(meshA.nodes[i], meshB.nodes[i])
            i += 1

rotate_amounts = [7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
                  5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
                  4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
                  6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21]
 
#constants = [int(abs(math.sin(i+1)) * 2**32) & 0xFFFFFFFF for i in range(64)]

constants = [0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
             0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
             0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
             0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
             
             0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
             0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
             0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
             0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
             
             0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
             0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
             0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
             0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
             
             0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
             0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
             0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
             0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391]
 
init_values = [0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476]

def left_rotate(x, amount):
    x &= 0xFFFFFFFF
    return ((x<<amount) | (x>>(32-amount))) & 0xFFFFFFFF

def right_rotate(x, amount):
    x &= 0xFFFFFFFF
    return ((x<<(32-amount) | x>>amount)) & 0xFFFFFFFF
 
def md5(message):
 
    message = bytearray(message) #copy our input into a mutable buffer
    orig_len_in_bits = 8*len(message)
    message.append(0x80)
    while len(message)%64 != 56:
        message.append(0)
    message += orig_len_in_bits.to_bytes(8, byteorder='little')
 
    hash_pieces = init_values[:]

    for chunk_ofst in range(0, len(message), 64):
        a, b, c, d = hash_pieces
        chunk = message[chunk_ofst:chunk_ofst+64]
        numberWrong = 0
            
        aMesh = MeshLayer.layerForNumber(a)
        bMesh = MeshLayer.layerForNumber(b)
        cMesh = MeshLayer.layerForNumber(c)
        dMesh = MeshLayer.layerForNumber(d)
        
        messageMeshes = []
        for i in range(16):
            messageNumber = int.from_bytes(chunk[4*i:4*i+4], byteorder='little')
            messageMeshes.append(MeshLayer.layerForNumber(messageNumber))
        
        for i in range(0,16):
            fMesh = OrMesh(AndMesh(bMesh, cMesh), AndMesh(dMesh, NotMesh(bMesh)))
            g = i
            
            constantMesh = MeshLayer.layerForNumber(constants[i])
            
            toRotateMesh = AddMesh(AddMesh(AddMesh(aMesh, fMesh), constantMesh), messageMeshes[g])
            
            newBMesh = AddMesh(bMesh, LeftRotateMesh(toRotateMesh, rotate_amounts[i]))
            aMesh, bMesh, cMesh, dMesh = dMesh, newBMesh, bMesh, cMesh
        
        for i in range(16, 32):
            fMesh = OrMesh(AndMesh(dMesh, bMesh), AndMesh(cMesh, NotMesh(dMesh)))
            g = ( 5*i + 1 )%16

            constantMesh = MeshLayer.layerForNumber(constants[i])

            toRotateMesh = AddMesh(AddMesh(AddMesh(aMesh, fMesh), constantMesh), messageMeshes[g])
            newBMesh = AddMesh(bMesh, LeftRotateMesh(toRotateMesh, rotate_amounts[i]))
            aMesh, bMesh, cMesh, dMesh = dMesh, newBMesh, bMesh, cMesh

        for i in range(32, 48):
            fMesh = XorMesh(XorMesh(bMesh, cMesh), dMesh)
            g = ( 3*i + 5 )%16
            
            constantMesh = MeshLayer.layerForNumber(constants[i])
            
            toRotateMesh = AddMesh(AddMesh(AddMesh(aMesh ,fMesh), constantMesh), messageMeshes[g])
            newBMesh = AddMesh(bMesh, LeftRotateMesh(toRotateMesh, rotate_amounts[i]))
            aMesh, bMesh, cMesh, dMesh = dMesh, newBMesh, bMesh, cMesh

        for i in range(48, 64):
            fMesh = XorMesh(cMesh, OrMesh(bMesh, NotMesh(dMesh)))
            g = ( 7*i )%16
            
            constantMesh = MeshLayer.layerForNumber(constants[i])
            
            toRotateMesh = AddMesh(AddMesh(AddMesh(aMesh, fMesh), constantMesh), messageMeshes[g])
            newBMesh = AddMesh(bMesh, LeftRotateMesh(toRotateMesh, rotate_amounts[i]))
            aMesh, bMesh, cMesh, dMesh = dMesh, newBMesh, bMesh, cMesh

        a, b, c, d = aMesh.toNumber(), bMesh.toNumber(), cMesh.toNumber(), dMesh.toNumber()
        for i, val in enumerate([a, b, c, d]):
            hash_pieces[i] += val
            hash_pieces[i] &= 0xFFFFFFFF

                #print('WRONG: ', numberWrong)
 
    return sum(x<<(32*i) for i, x in enumerate(hash_pieces))
 
def md5_to_hex(digest):
    raw = digest.to_bytes(16, byteorder='little')
    return '{:032x}'.format(int.from_bytes(raw, byteorder='big'))

if __name__=='__main__':
    demo = [b"",
            b"a",
            b"abc",
            b"message digest",
            b"abcdefghijklmnopqrstuvwxyz",
            b"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
            b"12345678901234567890123456789012345678901234567890123456789012345678901234567890",
            b"The quick brown fox jumps over the lazy dog",
            b"The quick brown fox jumps over the lazy dog.",]
    output = ['d41d8cd98f00b204e9800998ecf8427e',
              '0cc175b9c0f1b6a831c399e269772661',
              '900150983cd24fb0d6963f7d28e17f72',
              'f96b697d7cb7938d525a2f31aaf161d0',
              'c3fcd3d76192e4007dfb496cca67e13b',
              'd174ab98d277d9f5a5611c2c9f419d9f',
              '57edf4a22be3c955ac49da2e2107b67a',
              '9e107d9d372bb6826bd81d3542a419d6',
              'e4d909c290d0fb1ca068ffaddf22cbd0'];
    
    testsPassed = True
    i = 0
    ##for message in demo:
        ##testsPassed = md5_to_hex(md5(message)) == output[i]
        ##if (not testsPassed):
            ##break
        ##i += 1

    ##print('Tests passed: ', testsPassed)


    one = MeshLayer.layerForNumber(7)
    two = MeshLayer.layerForNumber(9)

    a = AddMesh(two, one)

    result = AddMesh(a, two)
    result.state = state_fixed

    print(one.toNumber(), '+', two.toNumber(), '+', two.toNumber(), '=', result.toNumber())

    result.nodes[3].setValue(not result.nodes[3].getValue())

    print(one.toNumber(), '+', two.toNumber(), '+', two.toNumber(), '=', result.toNumber())