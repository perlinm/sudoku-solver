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
  if (value != 0) this->flags = {};
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
  sudoku puzzle;
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
      puzzle.cells.push_back(cell(row,column,value));
      cell_index++;
    }
  }

  // solve puzzle
  while(!puzzle.solved()) {

    bool update = false;
    update |= puzzle.set_cells_with_one_flag();
    update |= puzzle.update_flags();

    // for each group
    for (group g: { group::row, group::column, group::block }) {
      for (uint p = 0; p < 9; p++) {

        // for each value
        for (uint n = 1; n <= 9; n++) {

          // collect cells which could contain this value
          vector<uint> candidate_indices = {};

          // for each cell in this group
          for (uint i: group_indices(g,p)) {
            // if the cell flags contain this value, it is a candidate cell
            if (in_vector(n,puzzle.cells.at(i).flags)) {
              candidate_indices.push_back(i);
            }
          }

          // if there is only one candidate cell for this value, set the value
          if (candidate_indices.size() == 1) {
            puzzle.cells.at(candidate_indices.at(0)).flags = {n};
            update = true;
          }
        }
      }
    }

    if (!update) {
      cout << "puzzle not solved!\n";
      puzzle.print();
      return 1;
      break;
    }
  }

  puzzle.print();
  return 0;

}
