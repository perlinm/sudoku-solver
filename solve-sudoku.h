#pragma once
#include <iostream> // for standard output

using namespace std;

// characters delimiting cells in input file
const string value_chars = "123456789";

// check whether val is in vec
inline bool in_vector(const uint val, const vector<uint>& vec) {
  return (find(vec.begin(), vec.end(), val) != vec.end());
}

// cell struct
struct cell {
  const uint x;
  const uint y;
  uint value = 0;
  vector<uint> flags = {};

  cell(const uint x, const uint y, const uint value = 0);

  uint row() const { return x; }
  uint col() const { return y; }
  uint block() const { return (y/3) + 3*(x/3); }

  void add_flag(const uint n) {
    if (!in_vector(n, flags)) {
      flags.push_back(n);
      sort(flags.begin(), flags.end());
    }
  }
  void del_flag(const uint n) {
    for (uint i = 0; i < flags.size(); i++) {
      if (flags.at(i) == n) flags.erase(flags.begin()+i);
    }
  }
};
