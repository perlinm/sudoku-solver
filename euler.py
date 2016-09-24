#!/usr/bin/env python3

import tempfile
import subprocess as sp

puzzle_count = 0
solved_puzzles = 0
with open("puzzles.txt",'r') as f:

    for line in f:

        if "Grid" in line:
            puzzle = ""
            puzzle_count += 1
            continue
        else: puzzle += line.strip()

        if len(puzzle) == 81:
            puzzle_file = tempfile.NamedTemporaryFile('w',delete=False)
            puzzle_file.write(puzzle+"\n")
            puzzle_file.close()
            cmd_list = ["./solve-sudoku.exe",
                        "--puzzle", puzzle_file.name]
            output, _ = sp.Popen(cmd_list, stdout=sp.PIPE).communicate()
            output = output.decode("utf-8").strip()

            if "not solved" in output:
                print("\npuzzle {}:".format(puzzle_count))
                print(" ".join(cmd_list))
                print(output)
            else:
                solved_puzzles += 1

print("solved: {}/{}".format(solved_puzzles,puzzle_count))
