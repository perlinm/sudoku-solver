#include <iostream> // for standard output
#include <fstream> // for file input
#include <boost/algorithm/string.hpp> // string manipulation library
#include <boost/filesystem.hpp> // filesystem path manipulation library
#include <boost/program_options.hpp> // options parsing library

#include "solve-sudoku.h"

using namespace std;
namespace fs = boost::filesystem;
namespace po = boost::program_options;


cell::cell(const uint x, const uint y, const uint value) :
  x(x), y(y) { this->value = value; };


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
  if (inputs.count("puzzle") && !fs::exists(puzzle_file)) {
    cout << "puzzle does not exist: " << puzzle_file << endl;
    return 1;
  }

  // read in puzzle
  vector<cell> puzzle = {};
  {
    uint pos = 0;
    uint value = 0;
    char c;
    ifstream input(puzzle_file);
    while (input.get(c)) {
      if (c == ' ') {
        value = 0;
      } else if (value_chars.find(c) != string::npos) {
        value = c - '0'; // convert char to int
      } else continue;

      puzzle.push_back(cell(pos/9,pos%9,value));
      pos++;
    }
  }

  // print solution
  for (uint i = 0; i < puzzle.size(); i++) {
    const uint col = i%9;
    const uint row = i/9;

    cout << puzzle.at(i).value;

    if (i%9 == 8) cout << endl;
    if (col == 2 || col == 5) cout << "|";
    if (col == 8 && (row == 2 || row == 5)) cout << "-----------\n";

  }

}
