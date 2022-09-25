#pragma once

#include <ThunderDashboard/platform/platform.h>

namespace frc1511 {

class PlatformLinux: public Platform {
public:
  static PlatformLinux* get() {
    return &instance;
  }
  
  PlatformLinux(PlatformLinux const&) = delete;
  PlatformLinux& operator=(PlatformLinux const&) = delete;
  
  std::string open_file_dialog(FileType type, const char* extension = nullptr) override;
  std::string save_file_dialog(const char* extension = nullptr) override;
  
private:
  PlatformLinux();
  ~PlatformLinux();
  
  static PlatformLinux instance;
};

} // namespace frc1511
