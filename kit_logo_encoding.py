#!/bin/python3


def invert(char):
    return " " if char == "@" else "@"


# needs to be monochrome
raw = """
                @@@@@        @@@@@@@  @@@@@@ @@@@@@@@@@@@@@
          @@@    @@@@      @@@@@@@    @@@@@@ @@@@@@@@@@@@@@
         @@@@@   @@@@    @@@@@@@@     @@@@@@     @@@@@@    
          @@@@@   @@@   @@@@@@@       @@@@@@     @@@@@@    
    @@@     @@@@  @@@  @@@@@@         @@@@@@     @@@@@@    
   @@@@@@    @@@@  @@  @@@@@@         @@@@@@     @@@@@@    
    @@@@@@@@   @@@ @@  @@@@@@@        @@@@@@     @@@@@@    
         @@@@@   @@ @    @@@@@@@      @@@@@@     @@@@@@    
 @@@@        @@@  @ @      @@@@@@@    @@@@@@     @@@@@@    
@@@@@@@@@@@@@    @@          @@@@@@   @@@@@@     @@@@@@    
@@@@@@@@@@@@@@@@@@@@@         @@@@@@@ @@@@@@     @@@@@@    
"""

lines = raw.strip("\n").split("\n")
width = len(lines[0])
height = len(lines)
single_line = "".join(lines)
first_char = single_line[0]

run_length_encoding = [0]
cur_char = first_char
for i in single_line:
    if i == cur_char:
        run_length_encoding[-1] += 1
    else:
        run_length_encoding.append(1)
        cur_char = i

print(f"run_length_encoding: {run_length_encoding}")
print(f"first char: '{first_char}'")
print(f"width: {width}")


cur_char = first_char
new_single_line = ""
for i in run_length_encoding:
    new_single_line += i * cur_char
    cur_char = invert(cur_char)
new_lines = []
for i in range(0, len(new_single_line), width):
    new_lines.append(new_single_line[i : i + width])
out = "\n".join(new_lines)
print(out)

print(
    f"compression: 1-{len(run_length_encoding)}/{len(raw)} = {1-len(run_length_encoding)/len(raw)}"
)
