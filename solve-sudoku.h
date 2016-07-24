using namespace std;

struct cell {
  const uint row_num;
  const uint column_num;
  uint value;
  vector<uint> flags = {1,2,3,4,5,6,7,8,9};

  cell(const uint row, const uint column, const uint value);

  uint row() const { return row_num; }
  uint column() const { return column_num; }
  uint block() const { return (column_num/3) + 3*(row_num/3); }

  bool remove_flag(const uint n) {
    for (uint i = 0; i < flags.size(); i++) {
      if (flags.at(i) == n) {
        flags.erase(flags.begin()+i);
        return true;
      }
    }
    return false;
  }
};

bool solved(const vector<cell>& puzzle) {
  for (uint i = 0; i < puzzle.size(); i++) {
    if (puzzle.at(i).value == 0) return false;
  }
  return true;
}

void print(const vector<cell>& puzzle) {
  for (uint i = 0; i < puzzle.size(); i++) {
    const uint row = i/9;
    const uint column = i%9;

    if (puzzle.at(i).value != 0) cout << puzzle.at(i).value;
    else cout << " ";

    if (column == 2 || column == 5) cout << "|";
    else if (i%9 == 8) cout << endl;
    else cout << " ";
    if (column == 8 && (row == 2 || row == 5)) cout << "-----------------\n";
  }
}
