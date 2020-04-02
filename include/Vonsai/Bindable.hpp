#pragma once

namespace Vonsai {

class Bindable {
public:
  virtual void bind() const   = 0;
  virtual void unbind() const = 0;
  virtual ~Bindable()         = default;
};

class BindGuard {
private:
  Bindable const *m_ptr = nullptr;

public:
  explicit BindGuard(Bindable const &a_bindable) : m_ptr(&a_bindable) {
    if (m_ptr) { m_ptr->bind(); }
  }
  ~BindGuard() {
    if (m_ptr) { m_ptr->unbind(); }
    m_ptr = nullptr;
  }

  BindGuard(BindGuard &&) = delete;
  BindGuard &operator=(BindGuard &&) = delete;
  BindGuard(BindGuard const &)       = delete;
  BindGuard &operator=(BindGuard const &) = delete;
};

} // namespace Vonsai
