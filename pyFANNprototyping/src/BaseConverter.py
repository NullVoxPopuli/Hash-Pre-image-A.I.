# for converting to and from different bases

import struct

def binaryToBase64(binary):
    return struct.pack('I', binary).encode('base64')