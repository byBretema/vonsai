#include <Vonsai/Bindable.hpp>
#include <Vonsai/Utils/Logger.hpp>

namespace Vonsai {

Bindable::~Bindable() {}

void BindGuard::inject(Bindable const *a_bindable) {
  if (m_ptr) {
    vo_err("A bound object could not be modified.");
    return;
  }
  if (a_bindable) {
    m_ptr = a_bindable;
    m_ptr->bind();
  }
}
void BindGuard::inject(Bindable const &a_bindable) { inject(&a_bindable); }

BindGuard::BindGuard() {}
BindGuard::BindGuard(Bindable const *a_bindable) { inject(a_bindable); }
BindGuard::BindGuard(Bindable const &a_bindable) : BindGuard(&a_bindable) {}

BindGuard::~BindGuard() {
  if (m_ptr) {
    m_ptr->unbind();
    m_ptr = nullptr;
  }
}
} // namespace Vonsai
