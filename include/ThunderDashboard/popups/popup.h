#pragma once

#include <ThunderDashboard/dashboard.h>

namespace frc1511 {

class Popup {
public:
  virtual void present(bool* running) = 0;
  virtual std::string get_name() = 0;
  
protected:
  Popup() = default;
  ~Popup() = default;
};

} // namespace frc1511
