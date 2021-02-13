from crccheck.crc import Crc32Mpeg2
from hexformat.intelhex import IntelHex

FILENAME = 'genhex'

print('Enter HEX size you want to generate')
size = int(input(), 0) # 0x hex notaion is supported

print('Enter start address')
offset = int(input(), 0)

inhex = IntelHex(variant='I32HEX')

pattern = []
for addr in range(offset, offset + size):
    pattern.append((addr + 1) % 256)

inhex.fill(offset, size, pattern)

print('Result:')
hexdict = inhex.todict()
addrs = list(hexdict.keys())
vals = list(hexdict.values())

print('==========')
print('START:      0x{:08X} / {:d}'.format(addrs[0], addrs[0]))
print('END:        0x{:08X} / {:d}'.format(addrs[-1], addrs[-1]))
print('SIZE:       0x{:08X} / {:d}'.format(inhex.usedsize(),inhex.usedsize()))
print('First word: 0x{:08X} / {:d}'.format(vals[0], vals[0]))
print('Last word:  0x{:08X} / {:d}'.format(vals[-1], vals[-1]))
print('==========')

print('Write to {:s}'.format(FILENAME))
print('Generate HEX...')
inhex.toihexfile(FILENAME, variant='I32HEX')
print('Generate BIN...')
inhex.tobinfile(FILENAME)
print('Done.')




