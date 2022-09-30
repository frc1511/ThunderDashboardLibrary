#pragma once

#include <ThunderDashboard/dashboard.h>

namespace frc1511 {

class Page {
public:
  virtual ~Page() = default;

  virtual void init() { }
  virtual void present(bool* running) = 0;
  constexpr bool is_focused() const { return focused; }
  
protected:
  Page() = default;

  bool focused = false;
};

} // namespace frc1511
