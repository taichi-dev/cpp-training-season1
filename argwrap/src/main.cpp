#include <iostream>
#include <string>

#include "argwrap.hpp"

void PassByValue(std::string val) {
  std::cout << "PassByValue(`" << val << "`)" << std::endl;
}

void PassByConstRef(const std::string &val) {
  std::cout << "PassByConstRef(const `" << val << "` &)" << std::endl;
}

void Test(std::string &s1, const std::string &s2, std::string s3) {
  std::cout << "s1=" << s1 << " s2=" << s2 << " s3=" << s3 << std::endl;
  s1.push_back('d');
}

int main() {
  std::string a{"abc"};
  {
    td::FuncWrap fw{PassByValue};
    fw.Invoke(a);
    fw.Invoke(std::string{"taichi"});
  }
  std::cout << std::endl;
  {
    td::FuncWrap fw{PassByConstRef};
    fw.Invoke(a);
    fw.Invoke(std::string{"taichi"});
  }
  std::cout << std::endl;
  {
    td::FuncWrap fw{Test};
    fw.Invoke(a, std::string{"def"}, a);
    std::cout << "a=" << a << std::endl;
  }
  std::cout << std::endl;

  return 0;
}
