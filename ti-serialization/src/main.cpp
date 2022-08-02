#include <iostream>
#include <string>
#include <vector>

#include "serialization.h"

enum class MyBool {
  kYes,
  kNo,
};

class Foo {
 public:
  bool operator==(const Foo &other) const {
    return EqualsExcludingY(other) && (y_ == other.y_);
  }

  bool EqualsExcludingY(const Foo &other) const {
    return (str == other.str) && (x == other.x) && (vec_ == other.vec_) &&
           (flag_ == other.flag_);
  }

  std::string str;
  int x{0};

  std::vector<int> &vec() {
    return vec_;
  }

  MyBool &flag() {
    return flag_;
  }

  float &y() {
    return y_;
  }

 private:
  std::vector<int> vec_;
  MyBool flag_{MyBool::kNo};
  float y_{0};

 public:                           // Why public?
  TI_IO_DEF(str, x, vec_, flag_);  // no `y_`
};

int main() {
  Foo foo{};
  foo.str = "taichi";
  foo.x = 42;
  foo.vec() = {1, 2, 3, 4, 5};
  foo.flag() = MyBool::kYes;
  foo.y() = 123.4f;

  // Serializes `foo` to binary format
  BinaryOutputSerializer bin_output;
  bin_output.initialize();
  bin_output(foo);
  bin_output.finalize();

  // Deserializes the data in `bin_output` back to `deser_foo`
  Foo deser_foo{};
  BinaryInputSerializer bin_input;
  bin_input.initialize(bin_output.data.data());
  bin_input(deser_foo);

  std::cout << std::boolalpha;
  std::cout << "foo == deser_foo: " << (foo == deser_foo) << ", expected: false"
            << std::endl;
  std::cout << "foo.EqualsExcludingY(deser_foo): "
            << foo.EqualsExcludingY(deser_foo) << ", expected: true"
            << std::endl;
  // Let's see the serialized text
  TextSerializer tex_ser;
  tex_ser("foo", foo);
  tex_ser.print();

  return 0;
}
