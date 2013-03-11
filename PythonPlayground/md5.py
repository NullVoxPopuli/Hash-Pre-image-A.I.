import math
 
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

def bring_from_dead(val):
    print(val)
    if (val < 0):
        return val + 0x100000000
    elif (val > 0xFFFFFFFF):
        return bring_from_dead(val - 0x100000000)
    else:
        return val

def left_rotate(x, amount):
    x &= 0xFFFFFFFF
    return ((x<<amount) | (x>>(32-amount))) & 0xFFFFFFFF

def right_rotate(x, amount):
    unrotated = ((x>>amount) | (x<<(32-amount)))
    return bring_from_dead(unrotated)
 
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
        for i in range(0,16):
            f = (b & c) | (~b & d)
            g = i
            
            to_rotate = a + f + constants[i] + int.from_bytes(chunk[4*g:4*g+4], byteorder='little')
            new_b = (b + left_rotate(to_rotate, rotate_amounts[i])) & 0xFFFFFFFF
            a, b, c, d = d, new_b, b, c
        for i in range(16, 32):
            f = (d & b) | (~d & c)
            g = ( 5*i + 1 )%16
            
            to_rotate = a + f + constants[i] + int.from_bytes(chunk[4*g:4*g+4], byteorder='little')
            new_b = (b + left_rotate(to_rotate, rotate_amounts[i])) & 0xFFFFFFFF
            a, b, c, d = d, new_b, b, c
        for i in range(32, 48):
            f = b ^ c ^ d
            g = ( 3*i + 5 )%16
            
            to_rotate = a + f + constants[i] + int.from_bytes(chunk[4*g:4*g+4], byteorder='little')
            new_b = (b + left_rotate(to_rotate, rotate_amounts[i])) & 0xFFFFFFFF
            a, b, c, d = d, new_b, b, c
        for i in range(48, 64):
            #print('Before:\t', a, '\t', b, '\t', c,'\t', d)
            f = c ^ (b | ~d)
            g = ( 7*i )%16
            
            #print('message piece:\t\t', int.from_bytes(chunk[4*g:4*g+4], byteorder='little'))
            to_rotate = a + f + constants[i] + int.from_bytes(chunk[4*g:4*g+4], byteorder='little')
            #print('part to rotate: ', to_rotate)
            rotated = left_rotate(to_rotate, rotate_amounts[i])
            
            new_b = (b + rotated) & 0xFFFFFFFF
            
            old_b = new_b - rotated
            if (old_b < 0):
                old_b += 0x100000000
            
            unrotated = right_rotate(rotated, rotate_amounts[i])

            if (not rotated == unrotated):
                print('rot: ', rotated, 'un: ', unrotated)
                numberWrong += 1

            a, b, c, d = d, new_b, b, c
                #print('After:\t', a, '\t', b, '\t', c,'\t', d)
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