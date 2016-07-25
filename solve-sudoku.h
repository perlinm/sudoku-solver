using namespace std;

// check whether val is in vec
inline bool in_vector(const uint val, const vector<uint>& vec) {
  return (find(vec.begin(), vec.end(), val) != vec.end());
}

enum class group { row, column, block };

struct cell {
  const uint row_num;
  const uint column_num;
  uint value;
  vector<uint> flags = {1,2,3,4,5,6,7,8,9};

  cell(const uint row, const uint column, const uint value);

  uint row() const { return row_num; }
  uint column() const { return column_num; }
  uint block() const { return (column_num/3) + (row_num/3)*3; }

  uint group(const group g) const {
    if (g == group::row) return row();
    if (g == group::column) return column();
    if (g == group::block) return block();
    exit(1);
  }

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

vector<uint> row_indices(const uint row) {
  vector<uint> indices = {};
  for (uint i = 0; i < 9; i++) {
    indices.push_back(row*9 + i);
  }
  return indices;
}

vector<uint> column_indices(const uint column) {
  vector<uint> indices = {};
  for (uint i = 0; i < 9; i++) {
    indices.push_back(column + i*9);
  }
  return indices;
}

vector<uint> block_indices(const uint block) {
  vector<uint> indices = {};
  for (uint i = 0; i < 9; i++) {
    indices.push_back((block/3)*27 + (block%3)*3 + (i/3)*9+ i%3);
  }
  return indices;
}

vector<uint> group_indices(const group g, const uint num) {
  if (g == group::row) return row_indices(num);
  if (g == group::column) return column_indices(num);
  if (g == group::block) return block_indices(num);
  exit(1);
}

struct sudoku {
  vector<cell> cells = {};

  sudoku(){};

  bool update_flags() {
    bool update = false;
    for (uint i = 0; i < cells.size(); i++) {
      if (cells.at(i).value > 0) continue;
      for (group g: { group::row, group::column, group::block }) {
        for (uint j: group_indices(g,cells.at(i).group(g))) {
          if (cells.at(j).value == 0) continue;
          update |= cells.at(i).remove_flag(cells.at(j).value);
        }
      }
    }
    return update;
  }

  bool scan_groups() {
    bool update = false;
    // for each group of 9 cells
    for (group g: { group::row, group::column, group::block }) {
      for (uint p = 0; p < 9; p++) {

        // for each value (i.e. 1..9)
        for (uint n = 1; n <= 9; n++) {

          // collect indices of cells which could contain this value
          vector<uint> candidate_indices = {};

          // for each cell in this group
          for (uint i: group_indices(g,p)) {
            // if the cell flags contain this value, it is a candidate cell
            if (in_vector(n,cells.at(i).flags)) {
              candidate_indices.push_back(i);
            }
          }

          // if there is only one candidate cell for this value,
          //   remove all other flags from that cell, i.e. "almost" set the value.
          if (candidate_indices.size() == 1) {
            cells.at(candidate_indices.at(0)).flags = {n};
            update = true;
          }
        }
      }
    }
    return update;
  }

  bool set_cells_with_one_flag() {
    bool update = false;
    for (uint i = 0; i < cells.size(); i++) {
      if (cells.at(i).flags.size() == 1) {
        cells.at(i).value = cells.at(i).flags.at(0);
        cells.at(i).flags = {};
        update = true;
      }
    }
    return update;
  }

  bool solved() {
    for (uint i = 0; i < 81; i++) {
      if (cells.at(i).value == 0) return false;
    }
    return true;
  }

  void print() {
    for (uint i = 0; i < 81; i++) {
      const uint row = i/9;
      const uint column = i%9;

      if (cells.at(i).value != 0) cout << cells.at(i).value;
      else cout << " ";

      if (column == 2 || column == 5) cout << "|";
      else if (i%9 == 8) cout << endl;
      else cout << " ";
      if (column == 8 && (row == 2 || row == 5)) cout << "-----------------\n";
    }
  }

};
