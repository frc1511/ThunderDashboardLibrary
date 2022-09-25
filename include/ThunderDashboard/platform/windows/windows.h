#pragma once

#include <ThunderDashboard/platform/platform.h>

namespace frc1511 {

class PlatformWindows : public Platform {
public:
  static PlatformWindows* get() {
    return &instance;
  }
  
  PlatformWindows(PlatformWindows const&) = delete;
  PlatformWindows& operator=(PlatformWindows const&) = delete;
  
  std::string open_file_dialog(FileType type, const char* extension = nullptr) override;
  std::string save_file_dialog(const char* extension = nullptr) override;
  
private:
  PlatformWindows();
  ~PlatformWindows();
  
  static PlatformWindows instance;
};

} // namespace frc1511
