import sys

instructions = {
    "bits",
    "movb",
    "int",
    "hlt",
    "jmp",
    "times",
    "dw",
}

reg_codes = {
    "ah": 0xb4,
    "al": 0xb0,
}

filename = sys.argv[1]
outname = filename.replace('.S', '.bin')
out_file = open(outname, "wb+")
symbols = {}
with open(filename, "r+") as source:
    offset = 0
    for line in source.readlines():
        tokens = line.strip().split()
        instruction, args = tokens[0].strip(), tokens[1:]
        if not (instruction in instructions):
            symbol = instruction[:-1]
            symbols[symbol] = offset

        print(instruction, args, offset)
        match instruction:
            case 'movb':
                immediate = args[0].strip(',')
                reg = args[1][1:]
                if immediate[1] == "0":
                    immediate = int(immediate[1:], 16)
                elif immediate[1] == "'":
                    if len(immediate) == 2:
                        reg = args[2][1:]
                        immediate = ord(' ')
                    else:
                        immediate = ord(immediate[2])

                print(immediate, reg)

                reg_code = reg_codes[reg]
                out_file.write(bytearray([reg_code, immediate]))
                offset += 2
            case 'int':
                immediate = int(args[0][1:], 16)
                print(immediate)
                out_file.write(b"\xcd" + immediate.to_bytes(1, 'little'))
                offset += 2
            case 'hlt':
                out_file.write(b"\xf4")
                offset += 1
            case 'jmp':
                print("offset is", offset)
                relative_offset = 0x12c - offset
                out_file.write(b"\xeb" + relative_offset.to_bytes(2, 'little'))
                offset += 2
            case 'times':
                remaining = 509 - offset
                out_file.write(bytes(remaining))
                offset += 2
            case 'dw':
                immediate = bytes(reversed(bytes.fromhex(args[0][3:])))
                print("dw", immediate)
                out_file.write(immediate)
                offset += 2
            case 'bits':
                pass

out_file.close()