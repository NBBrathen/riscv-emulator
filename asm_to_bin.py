import struct

instructions = [
    # Main
    0x00C000EF, # jal x1, 12
    0x06300113, # addi x2, x0, 99
    0x00000063, # beq x0, x0, 0 (Infinite Loop)

    # Function (Address 12)
    0x04D00193, # addi x3, x0, 77
    0x00008067, # jalr x0, 0(x1)
]

filename = "test.bin"

with open(filename, "wb") as f:
    for instr in instructions:
        # Pack the integer as 4 bytes, Little Endian
        f.write(struct.pack("<I", instr))

print(f"Successfully created {filename} ({len(instructions) * 4} bytes)")
