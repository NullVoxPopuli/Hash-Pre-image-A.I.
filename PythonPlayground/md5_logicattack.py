import math

state_resolved = 500
state_needs_resolved = 501

state_constant = 400
state_mutable = 401
state_mutated = 402

carrychange_none = 500
carrychange_totrue = 501
carrychange_tofalse = 502

class Tracker:

    def __init__(self):
        self.listOfLists = []

    def getCarryTrackingLevel(self, level):
        while level > len(self.listOfLists)-1:
            self.listOfLists.append([])
        return self.listOfLists[level]

    def activateAll(self):
        i = 0
        oldCarryTracking = list(self.listOfLists)
        self.listOfLists = []
        oldLen = len(oldCarryTracking)
        carryTrackingList = []
        while i < oldLen:
            wrapperList = oldCarryTracking[i]
            for nodeWrapper in wrapperList:
                nodeWrapper.activate()
            i += 1
        if oldLen > 0:
            self.activateAll()

carryTracker = Tracker()

class MeshNode:

    def __init__(self, val, s, mesh, level):
        self.value = val
        self.state = s
        self.changeListeners = []
        self.Mesh = mesh
        self.Level = level
    
    def getValue(self):
        return self.value
    
    def setValue(self, val):
        if self.isMutable():
            #print(id(self), ' set to give', '1' if val else '0')
            self.state = state_mutated
            self.value = val
            return True
        #print(id(self), 'refused change')
        return False
    
    def getState(self):
        return self.state

    def isMutable(self):
        return self.state == state_mutable

    def notifyChangeListeners(self):
        for node in self.changeListeners:
            node.resolve()

class MeshLayer:
    
    def __init__(self, state):
        self.nodes = [MeshNode(False, state, self, 0), MeshNode(False, state, self, 0), MeshNode(False, state, self, 0), MeshNode(False, state, self, 0),
                      MeshNode(False, state, self, 0), MeshNode(False, state, self, 0), MeshNode(False, state, self, 0), MeshNode(False, state, self, 0),
                      MeshNode(False, state, self, 0), MeshNode(False, state, self, 0), MeshNode(False, state, self, 0), MeshNode(False, state, self, 0),
                      MeshNode(False, state, self, 0), MeshNode(False, state, self, 0), MeshNode(False, state, self, 0), MeshNode(False, state, self, 0),
                      
                      MeshNode(False, state, self, 0), MeshNode(False, state, self, 0), MeshNode(False, state, self, 0), MeshNode(False, state, self, 0),
                      MeshNode(False, state, self, 0), MeshNode(False, state, self, 0), MeshNode(False, state, self, 0), MeshNode(False, state, self, 0),
                      MeshNode(False, state, self, 0), MeshNode(False, state, self, 0), MeshNode(False, state, self, 0), MeshNode(False, state, self, 0),
                      MeshNode(False, state, self, 0), MeshNode(False, state, self, 0), MeshNode(False, state, self, 0), MeshNode(False, state, self, 0),
                      
                      MeshNode(False, state, self, 0), MeshNode(False, state, self, 0), MeshNode(False, state, self, 0), MeshNode(False, state, self, 0),
                      MeshNode(False, state, self, 0), MeshNode(False, state, self, 0), MeshNode(False, state, self, 0), MeshNode(False, state, self, 0),
                      MeshNode(False, state, self, 0), MeshNode(False, state, self, 0), MeshNode(False, state, self, 0), MeshNode(False, state, self, 0),
                      MeshNode(False, state, self, 0), MeshNode(False, state, self, 0), MeshNode(False, state, self, 0), MeshNode(False, state, self, 0),
                      
                      MeshNode(False, state, self, 0), MeshNode(False, state, self, 0), MeshNode(False, state, self, 0), MeshNode(False, state, self, 0),
                      MeshNode(False, state, self, 0), MeshNode(False, state, self, 0), MeshNode(False, state, self, 0), MeshNode(False, state, self, 0),
                      MeshNode(False, state, self, 0), MeshNode(False, state, self, 0), MeshNode(False, state, self, 0), MeshNode(False, state, self, 0),
                      MeshNode(False, state, self, 0), MeshNode(False, state, self, 0), MeshNode(False, state, self, 0), MeshNode(False, state, self, 0)]
    
    def printLayer(self):
        str = ''
        i = 31
        while i > -1:
            str += '1' if self.nodes[i].getValue() else '0'
            i -= 1
        print(str)
    
    def layerForNumber(number, state):
        resultLayer = MeshLayer(state)
        index = 0
    
        while number > 0:
            newNode = resultLayer.nodes[index]
            state = newNode.state
            if number & 1 == 1:
                newNode.value = True
            else:
                newNode.value = False
            newNode.state = state
        
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

    def __init__(self, a, mesh):
        super(NotMeshNode, self).__init__(False, state_mutable, mesh, a.Level+1)
        self.A = a

    def getValue(self):
        return (not self.A.getValue())

class NotMesh(MeshLayer):
    
    def __init__(self, input_mesh):
        super(NotMesh, self).__init__(state_mutable)
        i = 0
        while i < 32:
            self.nodes[i] = NotMeshNode(input_mesh.nodes[i], self)
            i += 1

class AndMeshNode(MeshNode):
    
    def __init__(self, a, b, mesh):
        level = max(a.Level, b.Level)
        super(AndMeshNode, self).__init__(False, state_mutable, mesh, level+1)
        self.A = a
        self.B = b
    
    def getValue(self):
        return (self.A.getValue() and self.B.getValue())
    
    def setValue(self, val):
        self.value = val

class AndMesh(MeshLayer):
    
    def __init__(self, meshA, meshB):
        super(AndMesh, self).__init__(state_mutable)
        i = 0
        while i < 32:
            self.nodes[i] = AndMeshNode(meshA.nodes[i], meshB.nodes[i], self)
            i += 1

class OrMeshNode(MeshNode):

    def __init__(self, a, b, mesh):
        level = max(a.Level, b.Level)
        super(OrMeshNode, self).__init__(False, state_mutable, mesh, level+1)
        self.A = a
        self.B = b

    def getValue(self):
        return (self.A.getValue() or self.B.getValue())

class OrMesh(MeshLayer):

    def __init__(self, meshA, meshB):
        super(OrMesh, self).__init__(state_mutable)
        i = 0
        while i < 32:
            self.nodes[i] = OrMeshNode(meshA.nodes[i], meshB.nodes[i], self)
            i += 1

class AddNodeWrapper():

    def __init__(self, node, shouldTakeCarry):
        self.Node = node
        self.ShouldTake = shouldTakeCarry

    def activate(self):
#print('cantakecarry: ', self.Node.setShouldTakeCarry(self.ShouldTake))
        self.Node.setShouldTakeCarry(self.ShouldTake)

class AddMeshNode(MeshNode):

    def __init__(self, a, b, previous, mesh):
        level = max(a.Level, b.Level)
        super(AddMeshNode, self).__init__(False, state_mutable, mesh, level+1)
        a.changeListeners.append(self)
        self.A = a
        b.changeListeners.append(self)
        self.B = b
        self.carry = False
        self.Prev = previous
        self.Next = None
        self.value = -1

    def getValue(self):
        if self.value == -1:
            #print(id(self), 'is evaluating its result')
            val = 0
            self.carry = False

            if self.A.getValue():
                #print('a')
                val += 1
            if self.B.getValue():
                #print('b')
                val += 1
    
            if self.Prev is not None:
                if self.Prev.value == -1:
                    self.Prev.getValue()
                if self.Prev.carry:
                    #print('car')
                    val += 1
    
            if val > 1:
                self.carry = True
            else:
                self.carry = False

#print('val', val)
            if val == 1 or val == 3:
                self.value = 1
            else:
                self.value = 0

    #print(id(self), 'returning value of ', '1' if self.value == 1 else '0')
        return self.value == 1

    def setShouldTakeCarry(self, takeCarry):
        #print('')
        #print('>>>>', id(self), 'is forced to take carry of', '1' if takeCarry else '0')
        
        #print('A (', id(self.A), '):', '1' if self.A.getValue() else '0', ', B (', id(self.B), '):', '1' if self.B.getValue() else '0', 'Ans:', '1' if self.value == 1 else '0', 'give carry:', self.carry)
        
        if not takeCarry:
            if self.A.getValue() and self.B.getValue() and self.getValue():
                if self.setNodeTo(self.A, False, carrychange_tofalse):
                    return True
                elif self.setNodeTo(self.B, False, carrychange_tofalse):
                    return True
                return False
            elif (not self.A.getValue()) and (not self.B.getValue()) and self.getValue():
                if not self.setNodeTo(self.A, True, carrychange_none):
                    return self.setNodeTo(self.B, True, carrychange_none)
                return True
            elif self.A.getValue() and (not self.getValue()):
                if self.setNodeTo(self.B, True, carrychange_none):
                    return True
                if self.setNodeTo(self.A, False, carrychange_tofalse):
                    return True
                return False
            elif self.B.getValue() and (not self.getValue()):
                if self.setNodeTo(self.A, True, carrychange_none):
                    return True
                if self.setNodeTo(self.B, False, carrychange_tofalse):
                    return True
                return False
        else:
            if self.A.getValue() and self.B.getValue() and (not self.getValue()):
                if self.setNodeTo(self.A, False, carrychange_none):
                    return True
                return self.setNodeTo(self.B, False, carrychange_none)
            elif not self.A.getValue() and not self.B.getValue() and (not self.getValue()):
                if self.setNodeTo(self.A, True, carrychange_totrue):
                    return True
                return self.setNodeTo(self.B, True, carrychange_totrue)
            elif self.A.getValue() and self.getValue():
                if self.setNodeTo(self.A, False, carrychange_none):
                    return True
                return self.setNodeTo(self.B, True, carrychange_totrue)
            elif self.B.getValue() and self.getValue():
                if self.setNodeTo(self.B, False, carrychange_none):
                    return True
                return self.setNodeTo(self.A, True, carrychange_totrue)

                    #print('hopeful for no anomaly')
        return True

    def refreshCachedAnswer(self):
        self.value = -1
        self.getValue()

    def resolve(self):
        
        #print('')
        #print('====== resolving', id(self), '======')
        
        val = self.value
        car = self.carry
        
        self.setValue(val)
    
        if self.value == val and (not self.carry == car):
            carryTracker.getCarryTrackingLevel(self.Next.Level).append(AddNodeWrapper(self.Next, self.carry))

    def setValue(self, val):
        self.refreshCachedAnswer()
        #print('')
        #print('>>>>', id(self), 'is forced to give answer of', '1' if val else '0')
        #print('A (', id(self.A), '):', '1' if self.A.getValue() else '0', ', B (', id(self.B), '):', '1' if self.B.getValue() else '0', 'taking carry:', self.Prev.carry, 'from', id(self.Prev), 'Ans:', '1' if self.value == 1 else '0', 'give carry:', self.carry)
        if not (val ^ (self.value == 1) ):
            #print('no change needed')
            return True
        
        self.value = 1 if val else 0
                
        if not val:
            if self.Prev.carry:
                if self.A.getValue() and self.B.getValue():
                    #print('setting a or b to false')
                    if not self.setNodeTo(self.A, False, carrychange_none):
                        return self.setNodeTo(self.B, False, carrychange_none)
                    return True
                elif not self.A.getValue() and not self.B.getValue():
                    #print('setting a or b to true and starting carry')
                    if not self.setNodeTo(self.A, True, carrychange_totrue):
                        return self.setNodeTo(self.B, True, carrychange_totrue)
                    return True
                else:
                    print('anomaly 01200318')
            else:
                if self.A.getValue() and self.B.getValue():
                    print('anomaly 09010319')
                elif self.A.getValue():
                    if not self.setNodeTo(self.A, False, carrychange_none):
                        return self.setNodeTo(self.B, True, carrychange_totrue)
                    return True
                elif self.B.getValue():
                    if not self.setNodeTo(self.B, False, carrychange_none):
                        return self.setNodeTo(self.A, True, carrychange_totrue)
                    return True
                else:
                    print('anomaly 01220318')

        else:
            if self.Prev.carry:
                if self.B.getValue() and self.A.getValue():
                    print('anomaly 08570319')
                elif self.A.getValue():
                    if not self.setNodeTo(self.B, True, carrychange_none):
                        return self.setNodeTo(self.A, False, carrychange_tofalse)
                    return True
                elif self.B.getValue():
                    if not self.setNodeTo(self.A, True, carrychange_none):
                        return self.setNodeTo(self.B, False, carrychange_tofalse)
                    return True
                else:
                    print('anomaly 01225318')
            else:
                if self.A.getValue() and self.B.getValue():
                    if not self.setNodeTo(self.A, False, carrychange_tofalse):
                        return self.setNodeTo(self.B, False, carrychange_tofalse)
                    return True
                elif not self.A.getValue() and not self.B.getValue():
                    if not self.setNodeTo(self.A, True, carrychange_none):
                        return self.setNodeTo(self.B, True, carrychange_none)
                    return True
                else:
                    print('anomaly 01300318')

                #print('')
#print('<<<<', id(self), 'providing forced answer now')

    def setNodeTo(self, node, val, carrychange):
        setSuccessful = node.setValue(val)
        if setSuccessful and not (carrychange == carrychange_none):
            changeCarryTo = carrychange == carrychange_totrue
            self.carry = changeCarryTo
            #print(id(self), 'now giving a carry of', changeCarryTo, 'to', id(self.Next), 'that has a level of', self.Next.Level)
            carryTracker.getCarryTrackingLevel(self.Next.Level).append(AddNodeWrapper(self.Next, changeCarryTo))
        if setSuccessful:
            node.notifyChangeListeners()
        return setSuccessful

class EmptyNode(MeshNode):
    
    def __init__(self):
        super(EmptyNode, self).__init__(self, state_constant, None, 0)
        self.carry = False
        self.value = 0

    def setShouldTakeCarry(self, takeCarry):
        return True

class AddMesh(MeshLayer):

    def __init__(self, meshA, meshB):
        super(AddMesh, self).__init__(state_mutable)
        i = 0
        previousNode = EmptyNode()
        while i < 32:
            self.nodes[i] = AddMeshNode(meshA.nodes[i], meshB.nodes[i], previousNode, self)
            previousNode.Next = self.nodes[i]
            previousNode = self.nodes[i]
            i += 1
        previousNode.Next = EmptyNode()

class LeftRotateMesh(MeshLayer):

    def __init__(self, input_mesh, amount):
        super(LeftRotateMesh, self).__init__(state_mutable)
        i = 0
        index = 32 - amount
        while i < 32:
            if index == 32:
                index = 0
            
            self.nodes[i] = input_mesh.nodes[index]
            index += 1
            i += 1

class XorMeshNode(MeshNode):
    def __init__(self, a, b, mesh):
        level = max(a.Level, b.Level)
        super(XorMeshNode, self).__init__(False, state_mutable, mesh, level+1)
        self.A = a
        self.B = b
    
    def getValue(self):
        return (self.A.getValue() ^ self.B.getValue())

class XorMesh(MeshLayer):
    
    def __init__(self, meshA, meshB):
        super(XorMesh, self).__init__(state_mutable)
        i = 0
        while i < 32:
            self.nodes[i] = XorMeshNode(meshA.nodes[i], meshB.nodes[i], self)
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
            
        aMesh = MeshLayer.layerForNumber(a, state_constant)
        bMesh = MeshLayer.layerForNumber(b, state_constant)
        cMesh = MeshLayer.layerForNumber(c, state_constant)
        dMesh = MeshLayer.layerForNumber(d, state_constant)
        
        messageMeshes = []
        for i in range(16):
            messageNumber = int.from_bytes(chunk[4*i:4*i+4], byteorder='little')
            messageMeshes.append(MeshLayer.layerForNumber(messageNumber, state_constant))
        
        for i in range(0,16):
            fMesh = OrMesh(AndMesh(bMesh, cMesh), AndMesh(dMesh, NotMesh(bMesh)))
            g = i
            
            constantMesh = MeshLayer.layerForNumber(constants[i], state_constant)
            
            aTestMesh = MeshLayer.layerForNumber(aMesh.toNumber(), state_mutable)
            fTestMesh = MeshLayer.layerForNumber(fMesh.toNumber(), state_mutable)
            cTestMesh = MeshLayer.layerForNumber(constants[i], state_constant)
            messageTestMesh = MeshLayer.layerForNumber(messageMeshes[g].toNumber(), state_mutable)
            
            eightOff = AddMesh(AddMesh(AddMesh(aTestMesh, fTestMesh), cTestMesh), messageTestMesh)
            eightOff.toNumber()
            eightOff.nodes[3].setValue(not eightOff.nodes[3].getValue())
            
            carryTracker.activateAll()
        
            toRotateMesh = AddMesh(AddMesh(AddMesh(aMesh, fMesh), constantMesh), messageMeshes[g])
            
            num = toRotateMesh.toNumber() & 0xffffffff
            testNum = (aTestMesh.toNumber() + fTestMesh.toNumber() + cTestMesh.toNumber() + messageTestMesh.toNumber()) & 0xffffffff
            if not (num == testNum + 8 or num == testNum - 8):
                print(aMesh.toNumber(), '+', fMesh.toNumber(), '+', constantMesh.toNumber(), '+', messageMeshes[g].toNumber(), '=', num & 0xffffffff)
                print(aTestMesh.toNumber(), '+', fTestMesh.toNumber(), '+', cTestMesh.toNumber(), '+', messageTestMesh.toNumber(), '!=', num & 0xffffffff, '+/- 8')
                print('my ans:', testNum, 'actual:', num, 'difference:', testNum-num)
                numberWrong += 1
            
            newBMesh = AddMesh(bMesh, LeftRotateMesh(toRotateMesh, rotate_amounts[i]))
            aMesh, bMesh, cMesh, dMesh = dMesh, newBMesh, bMesh, cMesh
        
        for i in range(16, 32):
            fMesh = OrMesh(AndMesh(dMesh, bMesh), AndMesh(cMesh, NotMesh(dMesh)))
            g = ( 5*i + 1 )%16

            constantMesh = MeshLayer.layerForNumber(constants[i], state_constant)

            toRotateMesh = AddMesh(AddMesh(AddMesh(aMesh, fMesh), constantMesh), messageMeshes[g])
            newBMesh = AddMesh(bMesh, LeftRotateMesh(toRotateMesh, rotate_amounts[i]))
            aMesh, bMesh, cMesh, dMesh = dMesh, newBMesh, bMesh, cMesh

        for i in range(32, 48):
            fMesh = XorMesh(XorMesh(bMesh, cMesh), dMesh)
            g = ( 3*i + 5 )%16
            
            constantMesh = MeshLayer.layerForNumber(constants[i], state_constant)
            
            toRotateMesh = AddMesh(AddMesh(AddMesh(aMesh ,fMesh), constantMesh), messageMeshes[g])
            newBMesh = AddMesh(bMesh, LeftRotateMesh(toRotateMesh, rotate_amounts[i]))
            aMesh, bMesh, cMesh, dMesh = dMesh, newBMesh, bMesh, cMesh

        for i in range(48, 64):
            fMesh = XorMesh(cMesh, OrMesh(bMesh, NotMesh(dMesh)))
            g = ( 7*i )%16
            
            constantMesh = MeshLayer.layerForNumber(constants[i], state_constant)
            
            toRotateMesh = AddMesh(AddMesh(AddMesh(aMesh, fMesh), constantMesh), messageMeshes[g])
            newBMesh = AddMesh(bMesh, LeftRotateMesh(toRotateMesh, rotate_amounts[i]))
            aMesh, bMesh, cMesh, dMesh = dMesh, newBMesh, bMesh, cMesh

        a, b, c, d = aMesh.toNumber(), bMesh.toNumber(), cMesh.toNumber(), dMesh.toNumber()
        for i, val in enumerate([a, b, c, d]):
            hash_pieces[i] += val
            hash_pieces[i] &= 0xFFFFFFFF

        print('WRONG: ', numberWrong)
 
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
    for message in demo:
        testsPassed = md5_to_hex(md5(message)) == output[i]
        if (not testsPassed):
            break
        i += 1

    print('Tests passed: ', testsPassed)


    one = MeshLayer.layerForNumber(469894840, state_mutable)
    two = MeshLayer.layerForNumber(3776483511, state_mutable)
    three = MeshLayer.layerForNumber(4249261313, state_constant)
    four = MeshLayer.layerForNumber(1948283493, state_constant)
    
    result = AddMesh(AddMesh(AddMesh(one, two), three), four)

    print((one.toNumber() + two.toNumber() + three.toNumber() + four.toNumber()) & 0xffffffff, '=', result.toNumber())

    print('canset', result.nodes[3].setValue(not result.nodes[3].getValue()))
    carryTracker.activateAll()

    print((one.toNumber() + two.toNumber() + three.toNumber() + four.toNumber()) & 0xffffffff, '=', result.toNumber())
