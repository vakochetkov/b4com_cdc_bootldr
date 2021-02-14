import sys
import time
import argparse

import serial
from crccheck.crc import Crc32Mpeg2
from hexformat.intelhex import IntelHex

# flasher follows the bootloader protocol
EXEC_PATH = '../Debug/b4com_cdc_bootldr.bin'
EXEC_FORMAT = 'bin'
EXEC_OFFSET = '0x08008000'
PORT = 'COM14'

def wordFromBytes(b1, b2, b3, b4):
    return ((b1 << 24) | (b2 << 16) | (b3 << 8) | (b4))

def getInfo(inhex : IntelHex):
    print('Info:')
    hexdict = inhex.todict()
    addrs = list(hexdict.keys())
    vals = list(hexdict.values())

    print('==========')
    print('START:      0x{:08X} / {:d}'.format(addrs[0], addrs[0]))
    print('END:        0x{:08X} / {:d}'.format(addrs[-1], addrs[-1]))
    print('SIZE:       0x{:08X} / {:d}'.format(inhex.usedsize(),inhex.usedsize()))

    w1 = wordFromBytes(vals[0],vals[1],vals[2],vals[3])
    w2 = wordFromBytes(vals[-4],vals[-3],vals[-2],vals[-1])
    print('First word: 0x{:08X} / {:d}'.format(w1, w1))
    print('Last word:  0x{:08X} / {:d}'.format(w2, w2))
    print('==========')

class Timeout():
    def __init__(self):
        self.counter = 0
        self.ticks = 0
        self.isAlarm = False

    def set(self, ticks):
        self.isAlarm = False
        self.counter = 0
        self.ticks = ticks

    def wait(self):
        time.sleep(float(1/1000)) # in ms
        self.counter += 1
        if self.counter >= self.ticks:
            self.isAlarm = True
    

print('*** FLASHER START ***')

parser = argparse.ArgumentParser()
parser.add_argument('-p', '--port', type=str, required=True)
parser.add_argument('-f', '--file', type=str, required=True)
parser.add_argument('-o', '--offset', type=str)

argv = sys.argv
if len(argv[1:]) == 0:
    print('You should set path to HEX as the first cmd arg or BIN and OFFSET as first two args')
    print('Use predefined instead: ' + EXEC_PATH)
else:
    args = parser.parse_args(argv[1:])
    PORT = args.port

    if args.offset == None:
        print('Its probably HEX, for BIN define OFFSET as the second cmd argument')
        EXEC_FORMAT = 'hex'
        EXEC_PATH = args.file
    else:
        EXEC_FORMAT = 'bin'
        EXEC_PATH = args.file
        EXEC_OFFSET = args.offset
        print('Parse like its BIN, offset is ' + EXEC_OFFSET)

inhex = IntelHex(variant='I32HEX')
print(EXEC_PATH, EXEC_OFFSET, PORT)

if EXEC_FORMAT == 'bin':
    inhex.loadbinfile(EXEC_PATH)
elif EXEC_FORMAT == 'hex':
    inhex.loadihexfile(EXEC_PATH)

getInfo(inhex)
ihexdict = inhex.todict()
# print(ihexdict)

timeout = Timeout()
serial = serial.Serial(
                        port=PORT,
                        baudrate=500000,
                        parity=serial.PARITY_NONE,
                        stopbits=serial.STOPBITS_ONE,
                        bytesize=serial.EIGHTBITS,
                        timeout=0.25,
                        write_timeout=0.25
                        )

# stage 1
print('> START stage')
serial.write(b'BTLDR_START\n')

timeout.set(2000)
while not timeout.isAlarm and serial.inWaiting() < 5:
    timeout.wait()
if timeout.isAlarm:
    print('START stage - no response')
    # sys.exit(1)


response = serial.read(serial.inWaiting())
if b'BTLDR_ACK\n' in response:
    print('START stage - ACK')
else:
    print('START stage - error')
    print(response)
    # sys.exit(1)

# stage 2
print('> DEVNAME stage')
serial.write(b'BTLDR_B4COM_OUTLET_V2\n')

timeout.set(2000)
while not timeout.isAlarm and serial.inWaiting() < 5:
    timeout.wait()
if timeout.isAlarm:
    print('DEVNAME stage - no response')
    # sys.exit(1)

response = serial.read(serial.inWaiting())
if b'BTLDR_ACK\n' in response:
    print('DEVNAME stage - ACK')
else:
    print('DEVNAME stage - error')
    print(response)
    # sys.exit(1)

# stage 3
print('> FWPARAM stage')
size = inhex.usedsize().to_bytes(4, byteorder='little')
addr = int(EXEC_OFFSET, 0).to_bytes(4, byteorder='little')
data = list(inhex.todict().values())

crcinst = Crc32Mpeg2()
crcinst.process(data)
crc = crcinst.final().to_bytes(4, byteorder='little')
chunk = int(128).to_bytes(4, byteorder='little')
num = (int(inhex.usedsize() / 128) + (inhex.usedsize() % 128 > 0)).to_bytes(4, byteorder='little')

param = b'abcd_ 1_2 BTLDR_' + b'S' + size + b'A' + addr + b'C' + crc + b'B' + chunk + b'N' + num + b'\n'
print('size: {:d} addr: 0x{:08X} crc: 0x{:08X} chunk: {:d} num: {:d}'.format( 
        int.from_bytes(size, byteorder='little'),int.from_bytes(addr, byteorder='little'),int.from_bytes(crc, byteorder='little'),  
        int.from_bytes(chunk, byteorder='little'),int.from_bytes(num, byteorder='little')))     
print(param)

serial.write(param)

timeout.set(2000)
while not timeout.isAlarm and serial.inWaiting() < 5:
    timeout.wait()
if timeout.isAlarm:
    print('FWPARAM stage - no response')
    # sys.exit(1)

response = serial.read(serial.inWaiting())
if b'BTLDR_ACK\n' in response:
    print('FWPARAM stage - ACK')
else:
    print('FWPARAM stage - error')
    print(response)
    # sys.exit(1)

print('*** FLASHER FINISHED ***')