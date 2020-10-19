#pragma once

#define DC_DISALLOW_COPY(ClassName)      \
public:                                  \
  ClassName(ClassName const &) = delete; \
  ClassName &operator=(ClassName const &) = delete;

#define DC_DISALLOW_MOVE(ClassName)          \
public:                                      \
  ClassName(ClassName &&) noexcept = delete; \
  ClassName &operator=(ClassName &&) noexcept = delete;

#define DC_SINGLETON(ClassName) \
public:                         \
  DC_DISALLOW_COPY(ClassName)   \
  DC_DISALLOW_MOVE(ClassName)   \
  ~ClassName() = default;       \
                                \
private:                        \
  explicit className() = default;

// #define DC_ALLOW_COPY(ClassName) \
// public:                          \
//   ClassName(ClassName const &);  \
//   ClassName &operator=(ClassName const &);

#define DC_ALLOW_MOVE_H(ClassName)                           \
public:                                                      \
  friend void swap(ClassName &lhs, ClassName &rhs) noexcept; \
  ClassName(ClassName &&rhs) noexcept { swap(*this, rhs); }  \
  ClassName &operator=(ClassName &&rhs) noexcept {           \
    swap(*this, rhs);                                        \
    return *this;                                            \
  }

#define DC_ALLOW_MOVE_C(ClassName, code)               \
  void swap(ClassName &lhs, ClassName &rhs) noexcept { \
    using std::swap;                                   \
    code;                                              \
  }


#define DC_CLASS(ClassName, ...)   \
public:                            \
  explicit ClassName(__VA_ARGS__); \
  DC_DISALLOW_COPY(ClassName)      \
  DC_ALLOW_MOVE(ClassName)         \
  ~ClassName();

#define DC_DERIVED_CLASS(ClassName, ...) \
public:                                  \
  explicit ClassName(__VA_ARGS__);       \
  DC_DISALLOW_COPY(ClassName)            \
  DC_ALLOW_MOVE(ClassName)               \
  virtual ~ClassName();
