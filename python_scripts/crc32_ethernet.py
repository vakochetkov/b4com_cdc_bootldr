from crccheck.crc import Crc32, CrcXmodem, Crc32Mpeg2
from crccheck.checksum import Checksum32

# both methods are valid and give right answer
# data = [ bytearray.fromhex("000000AA"), bytearray.fromhex("00000000"), bytearray.fromhex("0000ABCD") ]
data = bytearray.fromhex("04030201")
# crc = bytearray.fromhex("FFFFFFFF")

crcinst = Crc32Mpeg2()

# for d in data:
    # crcinst.process(d)
crcinst.process(data)

crcbytes = crcinst.finalbytes()
crchex = crcinst.finalhex()
crcint = crcinst.final()

print(crcbytes)
print(crchex)
print(crcint)
print(0xA3771348)

