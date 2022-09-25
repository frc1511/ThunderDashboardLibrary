#include <ThunderDashboard/platform/linux/linux.h>
#include <ThunderDashboard/dashboard.h>

using namespace frc1511;

PlatformLinux::PlatformLinux() { }

PlatformLinux::~PlatformLinux() { }

std::string PlatformLinux::open_file_dialog(FileType type, const char* extension) {
  // TODO: Implement.
  return "";
}

std::string PlatformLinux::save_file_dialog(const char* extension) {
  // TODO: Implement.
  return "";
}

PlatformLinux PlatformLinux::instance;

