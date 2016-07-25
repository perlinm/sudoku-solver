using namespace std;

// check whether val is in vec
inline bool in_vector(const uint val, const vector<uint>& vec) {
  return (find(vec.begin(), vec.end(), val) != vec.end());
}

// return indices of all subgroups of a given size from a group of a given size
vector<vector<uint>> subgroups(const uint group_size, const uint subgroup_size) {
  assert(group_size > subgroup_size);
  const uint max_group_index = group_size-1;
  const uint max_subgroup_index = subgroup_size-1;

  vector<vector<uint>> collection = {};
  vector<uint> subgroup = {};
  for (uint n = 0; n < subgroup_size; n++) subgroup.push_back(n);
  collection.push_back(subgroup);

  while (true) {
    uint increment_pos = max_subgroup_index;
    uint max_pos_value = max_group_index;
    while (subgroup.at(increment_pos) == max_pos_value) {
      if (increment_pos == 0) return collection;
      increment_pos--;
      max_pos_value--;
    }
    subgroup.at(increment_pos) += 1;
    for (uint pos = increment_pos+1; pos < subgroup.size(); pos++) {
      subgroup.at(pos) = subgroup.at(pos-1)+1;
    }
    collection.push_back(subgroup);
  }
}

vector<uint> combine_lists(const vector<uint> A, const vector<uint> B) {
  vector<uint> C = A;
  for (uint i = 0; i < B.size(); i++) {
    if (!in_vector(B.at(i),C)) C.push_back(B.at(i));
  }
  return C;
}

// enum of group types
enum class group_type { row, column, block };

// cell struct; contains cell labels and methods
struct cell {
  const uint row_num;
  const uint column_num;
  uint value;
  vector<uint> flags = {1,2,3,4,5,6,7,8,9};

  cell(const uint row, const uint column, const uint value);

  uint row() const { return row_num; }
  uint column() const { return column_num; }
  uint block() const { return (column_num/3) + (row_num/3)*3; }

  uint group_num(const group_type g) const {
    if (g == group_type::row) return row();
    if (g == group_type::column) return column();
    if (g == group_type::block) return block();
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

// cell indices for a given row
vector<uint> row_indices(const uint row) {
  vector<uint> indices = {};
  for (uint i = 0; i < 9; i++) {
    indices.push_back(row*9 + i);
  }
  return indices;
}

// cell indices for a given column
vector<uint> column_indices(const uint column) {
  vector<uint> indices = {};
  for (uint i = 0; i < 9; i++) {
    indices.push_back(column + i*9);
  }
  return indices;
}

// cell indices for a given block
vector<uint> block_indices(const uint block) {
  vector<uint> indices = {};
  for (uint i = 0; i < 9; i++) {
    indices.push_back((block/3)*27 + (block%3)*3 + (i/3)*9+ i%3);
  }
  return indices;
}

// cell indices for a given type of group
vector<uint> group_indices(const group_type g, const uint num) {
  if (g == group_type::row) return row_indices(num);
  if (g == group_type::column) return column_indices(num);
  if (g == group_type::block) return block_indices(num);
  exit(1);
}

// sudoku puzzle struct; contains all cells and puzzle methods
struct sudoku {
  vector<cell> cells = {};

  sudoku(){};

  // method to update flags of all cells based on existing values
  bool update_flags() {
    bool update = false;
    for (uint i = 0; i < cells.size(); i++) {
      if (cells.at(i).value > 0) continue;
      for (group_type g: { group_type::row, group_type::column, group_type::block }) {
        for (uint j: group_indices(g,cells.at(i).group_num(g))) {
          if (cells.at(j).value == 0) continue;
          update |= cells.at(i).remove_flag(cells.at(j).value);
        }
      }
    }
    return update;
  }

  // method to scan each group of 9 cells and eliminate any impossible flags
  bool scan_groups_old() {
    bool update = false;
    // for each group of 9 cells
    for (group_type g: { group_type::row, group_type::column, group_type::block }) {
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

  // method to scan each group of 9 cells and eliminate any impossible flags;
  // if we find a subgroup of N cells mutually containing N values, eliminate
  //   all flags for all other values from these cells
  bool scan_groups(const uint subgroup_size) {
    bool update = false;
    // for each group of 9 cells
    for (group_type g: { group_type::row, group_type::column, group_type::block }) {
      for (uint p = 0; p < 9; p++) {
        vector<uint> cell_indices = group_indices(g,p);

        // collect list of [un]filled cells
        vector<uint> unfilled_cells = {};
        vector<uint> filled_cells = {};
        for (uint i: cell_indices) {
          if (cells.at(i).value == 0) unfilled_cells.push_back(i);
          else filled_cells.push_back(i);
        }
        if (unfilled_cells.size() <= subgroup_size) continue;

        // collect list of unused values
        vector<uint> unused_values = {};
        for (uint n = 1; n <= 9; n++) {
          bool unused = true;
          for (uint i: filled_cells) {
            if (cells.at(i).value == n) {
              unused = false;
              break;
            }
          }
          if (unused) unused_values.push_back(n);
        }
        assert(unfilled_cells.size() == unused_values.size());

        // list of candidate cells for each unused value;
        //  candidate_indices.at(i) is the list of (indices of) candidate cells
        //    for value unused_values.at(i)
        vector<vector<uint>> candidate_indices = {};

        // for each unused value
        for (uint n_i = 0; n_i < unused_values.size(); n_i++) {
          candidate_indices.push_back({});

          // for each unfilled cell
          for (uint i: unfilled_cells) {
            // if the cell flags contain this value, it is a candidate cell
            if (in_vector(unused_values.at(n_i),cells.at(i).flags)) {
              candidate_indices.at(n_i).push_back(i);
            }
          }
        }

        // if |union candidate cells lists for subgroup_size values| == subgroup_size,
        //   these cells can only contain these values
        //////////////////////////////////////////////////////////////////////////////////
        for (vector<uint> subgroup: subgroups(unfilled_cells.size(), subgroup_size)) {
          vector<uint> combined_candidates = {};
          vector<uint> combined_values = {};
          for (uint i: subgroup) {
            combined_candidates = combine_lists(combined_candidates,
                                                candidate_indices.at(i));
            combined_values.push_back(unused_values.at(i));
          }
          if (combined_candidates.size() == subgroup_size) {
            for (uint i: combined_candidates) {
              for (uint n: cells.at(i).flags) {
                if (!in_vector(n, combined_values)) {
                  cells.at(i).remove_flag(n);
                  update = true;
                }
              }
            }
          }
        }
      }
    }
    return update;
  }

  // method to set the value of cells with only one flag
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

  // is the puzzle solved?
  bool solved() {
    for (uint i = 0; i < 81; i++) {
      if (cells.at(i).value == 0) return false;
    }
    return true;
  }

  // print this puzzle
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
