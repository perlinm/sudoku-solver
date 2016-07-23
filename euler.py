#!/usr/bin/env python3

import tempfile
import subprocess as sp

with open("puzzles.txt",'r') as f:

    for line in f:

        if "Grid" in line:
            puzzle = ""
            print(line.strip())
            continue
        else: puzzle += line.strip()

        if len(puzzle) == 81:
            puzzle_file = tempfile.NamedTemporaryFile('w',delete=False)
            puzzle_file.write(puzzle+"\n")
            puzzle_file.close()
            cmd_list = ["./solve-sudoku.exe",
                        "--puzzle", puzzle_file.name]
            print(" ".join(cmd_list))
            output, _ = sp.Popen(cmd_list, stdout=sp.PIPE).communicate()
            output = output.decode("utf-8").strip()
            print(output)
