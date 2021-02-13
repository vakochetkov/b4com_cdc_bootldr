import sys

from crccheck.crc import Crc32Mpeg2
from intelhex import IntelHex

EXEC_PATH = 'Debug/b4com_cdc_bootldr.bin'
EXEC_FORMAT = 'bin'
EXEC_OFFSET = '0'

print('*** FLASHER START ***')

argv = sys.argv
if len(argv[1:]) == 0:
    print('You should pass path to HEX as the first cmd arg or BIN and OFFSET as first two args')
    print('Use predefined instead: ' + EXEC_PATH)
elif len(argv[1:]) == 1:
    print('Its probably HEX, for BIN define OFFSET as the second cmd argument')
    EXEC_FORMAT = 'hex'
else:
    EXEC_FORMAT = 'bin'
    EXEC_PATH = argv[1]
    EXEC_OFFSET = argv[2]
    print('Parse it like BIN, offset is ' + EXEC_OFFSET)

if EXEC_FORMAT == 'bin':
    inhex = IntelHex()
    inhex.fromfile(EXEC_PATH, format=EXEC_FORMAT)

elif EXEC_FORMAT == 'hex':
    pass

print('*** FLASHER FINISHED ***')