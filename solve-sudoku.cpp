#include <iostream> // for standard output
#include <fstream> // for file input
#include <boost/filesystem.hpp> // filesystem path manipulation library
#include <boost/program_options.hpp> // options parsing library

#include "solve-sudoku.h"

using namespace std;
namespace po = boost::program_options;


cell::cell(const uint row, const uint column, const uint value) :
  row_num(row), column_num(column) {
  this->value = value;
  if (value != 0) this->flags = { value };
};


int main(const int arg_num, const char *arg_vec[]) {

  // define and input options
  string puzzle_file;
  po::options_description options("Options");
  options.add_options()
    ("help,h", "produce help message")
    ("puzzle", po::value<string>(&puzzle_file), "puzzle to solve")
    ;

  // collect inputs
  po::variables_map inputs;
  po::store(parse_command_line(arg_num, arg_vec, options), inputs);
  po::notify(inputs);

  // if requested, print help text
  if (inputs.count("help")) {
    cout << options;
    return 0;
  }

  // check that we have a puzzle to work with
  if (!inputs.count("puzzle")) {
    cout << "no puzzle provided\n";
    return 0;
  }
  if (!boost::filesystem::exists(puzzle_file)) {
    cout << "puzzle does not exist: " << puzzle_file << endl;
    return 1;
  }

  // read in and initialize puzzle
  vector<cell> puzzle = {};
  {
    ifstream input(puzzle_file);
    const string value_chars = "123456789";

    uint value;
    char input_char;
    uint cell_index = 0;
    while (cell_index < 81) {
      if (input.eof()) {
        cout << "unexpected EOF in puzzle file: " << puzzle_file << endl;
        return -1;
      }

      input.get(input_char);
      if (input_char == ' ' || input_char == '_' || input_char == '0') {
        value = 0;
      } else if (value_chars.find(input_char) != string::npos) {
        value = input_char - '0'; // convert char to int
      } else continue;

      const uint row = cell_index/9;
      const uint column = cell_index%9;
      puzzle.push_back(cell(row,column,value));
      cell_index++;
    }
  }

  // solve puzzle
  while(!solved(puzzle)) {

    uint updates = 0;
    // for each cell...
    for (uint i = 0; i < puzzle.size(); i++) {
      if (puzzle.at(i).value > 0) continue;

      // ... update flags in cell ...
      for (uint j = 0; j < puzzle.size(); j++) {
        if (i == j) continue;
        if (puzzle.at(i).row() == puzzle.at(j).row() ||
            puzzle.at(i).column() == puzzle.at(j).column() ||
            puzzle.at(i).block() == puzzle.at(j).block()) {
          updates += puzzle.at(i).remove_flag(puzzle.at(j).value);
        }
      }

      // ... and set value of cell if there is only one flag left
      if (puzzle.at(i).flags.size() == 1) {
        puzzle.at(i).value = puzzle.at(i).flags.at(0);
        updates++;
      }
    }

    if (updates == 0) {
      cout << "puzzle is unsolvable!\n";
      break;
    }
  }

  print(puzzle);
}
