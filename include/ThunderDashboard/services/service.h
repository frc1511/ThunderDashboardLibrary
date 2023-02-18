#pragma once

#include <ThunderDashboard/dashboard.h>

namespace frc1511 {

class Service {
public:
  virtual ~Service() = default;

  virtual void init() { }
  virtual void process() { }

protected:
    Service() = default;
}; 

} // namespace frc1511
