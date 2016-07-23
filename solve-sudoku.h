using namespace std;

// cell struct
struct cell {
  const uint row_num;
  const uint column_num;
  uint value = 0;
  vector<uint> flags = {1,2,3,4,5,6,7,8,9};

  cell(const uint row, const uint column, const uint value);

  uint row() const { return row_num; }
  uint column() const { return column_num; }
  uint block() const { return (column_num/3) + 3*(row_num/3); }

  void remove_flag(const uint n) {
    for (uint i = 0; i < flags.size(); i++) {
      if (flags.at(i) == n) flags.erase(flags.begin()+i);
    }
  }
};

bool solved(const vector<cell>& puzzle) {
  for (uint i = 0; i < puzzle.size(); i++) {
    if (puzzle.at(i).value == 0) return false;
  }
  return true;
}
