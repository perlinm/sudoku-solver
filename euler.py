#!/usr/bin/env python3

import sys, os, tempfile
import subprocess as sp

if len(sys.argv) != 2:
    print("usage: {} [puzzle file]".format(sys.argv[0]))
    exit(1)

master_file = sys.argv[1]
if not os.path.isfile(master_file):
    print("the provided file does not exist")
    exit(1)

puzzle_count = 0
solved_puzzles = 0
with open(master_file, "r") as f:

    for line in f:
        puzzle = line.strip()
        if len(puzzle) != 81: continue
        puzzle_count += 1

        puzzle_file = tempfile.NamedTemporaryFile('w',delete=False)
        puzzle_file.write(puzzle+"\n")
        puzzle_file.close()
        cmd_list = ["./solve-sudoku.exe",
                    "--puzzle", puzzle_file.name]
        output, _ = sp.Popen(cmd_list, stdout=sp.PIPE).communicate()
        output = output.decode("utf-8").strip()

        if "not solved" in output:
            print("puzzle {}:".format(puzzle_count))
            print(" ".join(cmd_list))
            print(output)
            print()
        else:
            solved_puzzles += 1

print("solved: {}/{}".format(solved_puzzles,puzzle_count))
