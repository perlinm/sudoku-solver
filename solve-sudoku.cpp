#include <iostream> // for standard output
#include <fstream> // for file input
#include <boost/filesystem.hpp> // filesystem path manipulation library
#include <boost/program_options.hpp> // options parsing library

#include "solve-sudoku.h"

using namespace std;
namespace po = boost::program_options;


cell::cell(const uint row, const uint column, const uint value) :
  row_num(row), column_num(column) { this->value = value; };


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
  if (inputs.count("puzzle") && !boost::filesystem::exists(puzzle_file)) {
    cout << "puzzle does not exist: " << puzzle_file << endl;
    return 1;
  }

  // read in puzzle
  vector<cell> puzzle = {};
  {
    ifstream input(puzzle_file);
    const string value_chars = "123456789";

    char c;
    uint value;
    uint pos = 0;
    while (pos < 81) {
      input.get(c);
      if (c == ' ') {
        value = 0;
      } else if (value_chars.find(c) != string::npos) {
        value = c - '0'; // convert char to int
      } else continue;

      puzzle.push_back(cell(pos/9,pos%9,value));
      pos++;
    }
  }

  // solve puzzle
  while(!solved(puzzle)) {

    // for each cell...
    for (uint i = 0; i < puzzle.size(); i++) {
      if (puzzle.at(i).value > 0) continue;

      // update flags in cell
      for (uint j = 0; j < puzzle.size(); j++) {
        if (i == j) continue;
        if (puzzle.at(i).row() == puzzle.at(j).row() ||
            puzzle.at(i).column() == puzzle.at(j).column() ||
            puzzle.at(i).block() == puzzle.at(j).block()) {
          puzzle.at(i).remove_flag(puzzle.at(j).value);
        }
      }

      // set value of cell if there is only one flag left
      if (puzzle.at(i).flags.size() == 1) {
        puzzle.at(i).value = puzzle.at(i).flags.at(0);
      }
    }

  }

  // print solution
  for (uint i = 0; i < puzzle.size(); i++) {
    const uint column = i%9;
    const uint row = i/9;

    cout << puzzle.at(i).value;

    if (column == 2 || column == 5) cout << "|";
    else if (i%9 == 8) cout << endl;
    else cout << " ";
    if (column == 8 && (row == 2 || row == 5)) cout << "-----------------\n";
  }

}
