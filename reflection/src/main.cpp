#include <iostream>

#include "reflect.hpp"

class Foo {
 public:
  void PassByValue(std::string s) const {
    std::cout << "Foo::PassByValue(`" << s << "`)" << std::endl;
  }

  void PassByConstRef(const std::string &s) const {
    std::cout << "Foo::PassByConstRef(const `" << s << "` &)" << std::endl;
  }

  std::string Concat(const std::string &head, const std::string &tail) {
    auto res = head + tail;
    return res;
  }

  // std::unique_ptr will result in compile-time error
  std::shared_ptr<float> MakeFloatPtr(float i) {
    return std::make_shared<float>(i);
  }

  static void MakeReflectable() {
    reflect::AddClass<Foo>("Foo")
        .AddMemberVar("name", &Foo::name)
        .AddMemberVar("x_", &Foo::x_)
        .AddMemberFunc("PassByValue", &Foo::PassByValue)
        .AddMemberFunc("PassByConstRef", &Foo::PassByConstRef)
        .AddMemberFunc("Concat", &Foo::Concat)
        .AddMemberFunc("MakeFloatPtr", &Foo::MakeFloatPtr);
  }

  int x() const { return x_; }

  std::string name;

 private:
  int x_{0};
};

void TestMemberFunction() {
  std::cout << ">>> TestMemberFunction" << std::endl;
  using namespace reflect::details;
  Foo f;
  std::string hello_s{"hello"};
  std::string world_s{" world"};

  MemberFunction foo_pass_by_val{&Foo::PassByValue};
  foo_pass_by_val.Invoke(f, hello_s);

  MemberFunction foo_pass_by_cref{&Foo::PassByConstRef};
  // foo_pass_by_cref.Invoke(f, hello_s);  // Crash, value
  // foo_pass_by_cref.Invoke(f, std::ref(hello_s));  // Crash, non-const ref
  foo_pass_by_cref.Invoke(f, std::cref(hello_s));  // OK: const ref

  MemberFunction foo_concat{&Foo::Concat};
  // foo_concat.Invoke(f, std::cref(hello_s), world_s);
  auto res = foo_concat.Invoke(f, std::cref(hello_s), std::cref(world_s));
  std::cout << "Concat got: " << std::any_cast<std::string>(res) << std::endl;

  std::cout << "<<< TestMemberFunction OK\n" << std::endl;
}

void TestMemberVariable() {
  std::cout << ">>> TestMemberVariable" << std::endl;
  using namespace reflect::details;

  struct S {
    int a{0};
    float b{0.0f};
  };

  MemberVariable s_a{&S::a};
  MemberVariable s_b{&S::b};
  S s{};
  s_a.SetValue(s, 42);
  s_b.SetValue(s, 123.4f);
  std::cout << "s.a=" << s_a.GetValue<int>(s) << " expected=" << s.a
            << std::endl;
  std::cout << "s.b=" << s_b.GetValue<float>(s) << " expected=" << s.b
            << std::endl;
  std::cout << "<<< TestMemberVariable OK\n" << std::endl;
}

void TestFoo() {
  std::cout << ">>> TestFoo\n" << std::endl;

  Foo::MakeReflectable();
  auto foo_t = reflect::GetByName("Foo");
  for (const auto &mv : foo_t.member_vars()) {
    std::cout << "member var: " << mv.name() << std::endl;
  }
  std::cout << std::endl;
  for (const auto &mf : foo_t.member_funcs()) {
    std::cout << "member func: " << mf.name() << ", is_const=" << mf.is_const()
              << std::endl;
  }
  std::cout << std::endl;

  Foo f;
  // Test member variables
  auto name_var = foo_t.GetMemberVar("name");
  name_var.SetValue(f, std::string{"taichi"});
  std::cout << "f.name=" << f.name << std::endl;
  auto x_var = foo_t.GetMemberVar("x_");
  x_var.SetValue(f, 42);
  std::cout << "f.x=" << f.x() << std::endl;
  std::cout << std::endl;

  // Test member functions
  auto foo_make_float_ptr = foo_t.GetMemberFunc("MakeFloatPtr");
  auto res = foo_make_float_ptr.Invoke(f, 123.4f);
  auto float_sptr = std::any_cast<std::shared_ptr<float>>(res);
  std::cout << "MakeFloatPtr res: " << *float_sptr << std::endl;

  std::string hello_s{"hello"};
  std::string world_s{" world"};

  auto foo_pass_by_val = foo_t.GetMemberFunc("PassByValue");
  foo_pass_by_val.Invoke(f, hello_s);

  auto foo_pass_by_cref = foo_t.GetMemberFunc("PassByConstRef");
  // foo_pass_by_cref.Invoke(f, hello_s);  // Crash, value
  // foo_pass_by_cref.Invoke(f, std::ref(hello_s));  // Crash, non-const ref
  foo_pass_by_cref.Invoke(f, std::cref(hello_s));  // OK: const ref

  auto foo_concat = foo_t.GetMemberFunc("Concat");
  // foo_concat.Invoke(f, hello_s, world_s);
  res = foo_concat.Invoke(f, std::cref(hello_s), std::cref(world_s));
  std::cout << "Concat got: " << std::any_cast<std::string>(res) << std::endl;
  std::cout << std::endl;

  std::cout << "<<< TestFoo OK\n" << std::endl;
}

int main() { TestFoo(); }
