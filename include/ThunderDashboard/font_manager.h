#pragma once

#include <ThunderDashboard/dashboard.h>

namespace frc1511 {

struct FontManager {
  static FontManager* get() {
    return &instance;
  }

  ImFont* regular;
  ImFont* big;

private:
  static FontManager instance;
};

} // namespace frc1511
