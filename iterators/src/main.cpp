#include <iostream>
#include <vector>

using Array2D = std::vector<std::vector<char>>;

Array2D generate_array() {
  Array2D arr;
  arr.push_back({0, 0, 0, 117, 0, 0, 110});
  arr.push_back({0, 108, 101, 0, 97, 0, 0, 0, 0});
  arr.push_back({115, 0, 104, 0});
  arr.push_back({0, 0, 32, 99});
  arr.push_back({114, 0, 0, 0, 101, 97, 0, 0, 0, 0, 0, 0, 116, 0, 0});
  arr.push_back({0, 0, 105, 0, 0, 118, 0});
  arr.push_back({105, 116, 0, 121, 0, 0});
  return arr;
}

class Iterator {
 public:
  using value_type = char;
  using pointer = char*;
  using refereence = char&;

  // TODO: Define the constructor
  const char& operator*() const {
    // TODO: Your code here
    return 0;
  }

  const char* operator->() const {
    // TODO: Your code here
    return 0;
  }

  Iterator& operator++() {
    // TODO: Your code here
    return *this;
  }

  bool operator==(const Iterator& other) const {
    // TODO: Your code here
    return true;
  }

  bool operator!=(const Iterator& other) const {
    // No need to change this
    return !(*this == other);
  }
};

// * Iterate over every character in an Array2D in a flattend way, i.e., every
//   char in a row, then the next row.
// * You SHOULD skip the null characters (e.g. 0 or '\0').
class Flatten {
 public:
  explicit Flatten(const Array2D& arr) : arr_(arr) {
    // TODO: Your code here
  }

  Iterator begin() const {
    // TODO: Your code here
    return Iterator{};
  }
  Iterator end() const {
    // TODO: Your code here
    return Iterator{};
  }

 private:
  const Array2D& arr_;
};

int main() {
  auto arr = generate_array();
  for (char c : Flatten(arr)) {
    std::cout << c;
  }
  std::cout << std::endl;
  return 0;
}
