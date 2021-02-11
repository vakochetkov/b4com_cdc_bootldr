from crccheck.crc import Crc32, CrcXmodem, Crc32Mpeg2
from crccheck.checksum import Checksum32

data = bytearray.fromhex("F407A5C2")
crc = bytearray.fromhex("FFFFFFFF")

crcinst = Crc32Mpeg2()

crcinst.process(data)

crcbytes = crcinst.finalbytes()
crchex = crcinst.finalhex()
crcint = crcinst.final()

print(crcbytes)
print(crchex)
print(crcint)
print(0xB5E8B5CD)
