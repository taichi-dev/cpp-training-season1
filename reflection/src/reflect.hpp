#pragma once

// https://github.com/rttrorg/rttr
// https://preshing.com/20180116/a-primitive-reflection-system-in-cpp-part-1/

#include <any>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
namespace reflect {
namespace details {

class MemberVariable {
 public:
  MemberVariable() = default;

  template <typename C, typename T>
  MemberVariable(T C::*var) {
    getter_ = [var](std::any obj) -> std::any {
      return std::any_cast<const C *>(obj)->*var;
    };
    setter_ = [var](std::any obj, std::any val) {
      // Syntax: https://stackoverflow.com/a/670744/12003165
      // `obj.*member_var`
      auto *self = std::any_cast<C *>(obj);
      self->*var = std::any_cast<T>(val);
    };
  }

  const std::string &name() const {
    return name_;
  }

  template <typename T, typename C>
  T GetValue(const C &c) const {
    return std::any_cast<T>(getter_(&c));
  }

  template <typename C, typename T>
  void SetValue(C &c, T val) {
    setter_(&c, val);
  }

 private:
  friend class RawTypeDescriptorBuilder;

  std::string name_;
  std::function<std::any(std::any)> getter_{nullptr};
  std::function<void(std::any, std::any)> setter_{nullptr};
};

class MemberFunction {
 public:
  MemberFunction() = default;

  template <typename C, typename R, typename... Args>
  explicit MemberFunction(R (C::*func)(Args...)) {
    fn_ = [this, func](std::any obj_args) -> std::any {
      using tuple_t = std::tuple<C &, Args...>;
      // How to debug compile-time types...
      // static_assert(std::is_same<tuple_t, void>::value, "Hoi!");
      auto *tp_ptr = std::any_cast<tuple_t *>(obj_args);
      return std::apply(func, *tp_ptr);
    };
  }

  template <typename C, typename... Args>
  explicit MemberFunction(void (C::*func)(Args...)) {
    fn_ = [this, func](std::any obj_args) -> std::any {
      using tuple_t = std::tuple<C &, Args...>;
      auto *tp_ptr = std::any_cast<tuple_t *>(obj_args);
      std::apply(func, *tp_ptr);
      return std::any{};
    };
  }

  template <typename C, typename R, typename... Args>
  explicit MemberFunction(R (C::*func)(Args...) const) {
    fn_ = [this, func](std::any obj_args) -> std::any {
      using tuple_t = std::tuple<const C &, Args...>;
      // How to debug compile-time types...
      // static_assert(std::is_same<tuple_t, void>::value, "Hoi!");
      auto *tp_ptr = std::any_cast<tuple_t *>(obj_args);
      return std::apply(func, *tp_ptr);
    };
    is_const_ = true;
  }

  template <typename C, typename... Args>
  explicit MemberFunction(void (C::*func)(Args...) const) {
    fn_ = [this, func](std::any obj_args) -> std::any {
      using tuple_t = std::tuple<const C &, Args...>;
      auto *tp_ptr = std::any_cast<tuple_t *>(obj_args);
      std::apply(func, *tp_ptr);
      return std::any{};
    };
    is_const_ = true;
  }

  const std::string &name() const {
    return name_;
  }

  bool is_const() const {
    return is_const_;
  }

  template <typename C, typename... Args>
  std::any Invoke(C &c, Args &&... args) {
    if (is_const_) {
      auto tp = std::make_tuple(std::reference_wrapper<const C>(c), args...);
      return fn_(&tp);
    }
    auto tp = std::make_tuple(std::reference_wrapper<C>(c), args...);
    return fn_(&tp);
  }

 private:
  friend class RawTypeDescriptorBuilder;

  std::string name_;
  bool is_const_{false};
  std::function<std::any(std::any)> fn_{nullptr};
};

class TypeDescriptor {
 public:
  const std::string &name() const {
    return name_;
  }

  const std::vector<MemberVariable> &member_vars() const {
    return member_vars_;
  }

  const std::vector<MemberFunction> &member_funcs() const {
    return member_funcs_;
  }

  MemberVariable GetMemberVar(const std::string &name) const {
    for (const auto &mv : member_vars_) {
      if (mv.name() == name) {
        return mv;
      }
    }
    return MemberVariable{};
  }

  MemberFunction GetMemberFunc(const std::string &name) const {
    for (const auto &mf : member_funcs_) {
      if (mf.name() == name) {
        return mf;
      }
    }
    return MemberFunction{};
  }

 private:
  friend class RawTypeDescriptorBuilder;

  std::string name_;
  std::vector<MemberVariable> member_vars_;
  std::vector<MemberFunction> member_funcs_;
};

class RawTypeDescriptorBuilder {
 public:
  explicit RawTypeDescriptorBuilder(const std::string &name);

  ~RawTypeDescriptorBuilder();
  RawTypeDescriptorBuilder(const RawTypeDescriptorBuilder &) = delete;
  RawTypeDescriptorBuilder &operator=(const RawTypeDescriptorBuilder &) =
      delete;
  RawTypeDescriptorBuilder(RawTypeDescriptorBuilder &&) = default;
  RawTypeDescriptorBuilder &operator=(RawTypeDescriptorBuilder &&) = default;

  template <typename C, typename T>
  void AddMemberVar(const std::string &name, T C::*var) {
    MemberVariable mv{var};
    mv.name_ = name;
    desc_->member_vars_.push_back(std::move(mv));
  }

  template <typename FUNC>
  void AddMemberFunc(const std::string &name, FUNC func) {
    MemberFunction mf{func};
    mf.name_ = name;
    desc_->member_funcs_.push_back(std::move(mf));
  }

 private:
  std::unique_ptr<TypeDescriptor> desc_{nullptr};
};

template <typename T>
class TypeDescriptorBuilder {
 public:
  explicit TypeDescriptorBuilder(const std::string &name) : raw_builder_(name) {
  }

  template <typename V>
  TypeDescriptorBuilder &AddMemberVar(const std::string &name, V T::*var) {
    raw_builder_.AddMemberVar(name, var);
    return *this;
  }

  template <typename FUNC>
  TypeDescriptorBuilder &AddMemberFunc(const std::string &name, FUNC func) {
    raw_builder_.AddMemberFunc(name, func);
    return *this;
  }

 private:
  RawTypeDescriptorBuilder raw_builder_;
};

class Registry {
 public:
  static Registry &instance() {
    static Registry inst;
    return inst;
  }

  TypeDescriptor *Find(const std::string &name);

  void Register(std::unique_ptr<TypeDescriptor> desc);

  void Clear();

 private:
  std::unordered_map<std::string, std::unique_ptr<TypeDescriptor>> type_descs_;
};

}  // namespace details

template <typename T>
details::TypeDescriptorBuilder<T> AddClass(const std::string &name) {
  details::TypeDescriptorBuilder<T> b{name};
  return b;
}

details::TypeDescriptor &GetByName(const std::string &name);

void ClearRegistry();

}  // namespace reflect
