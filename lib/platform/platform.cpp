#include <ThunderDashboard/platform/platform.h>

using namespace frc1511;

#ifdef THUNDER_MACOS
# include <ThunderDashboard/platform/macos/macos.h>
#elif THUNDER_WINDOWS
# include <ThunderDashboard/platform/windows/windows.h>
#elif THUNDER_LINUX
# include <ThunderDashboard/platform/linux/linux.h>
#endif

Platform* Platform::get_current() {
#ifdef THUNDER_MACOS
  return PlatformMacOS::get();
#elif THUNDER_WINDOWS
  return PlatformWindows::get();
#elif THUNDER_LINUX
  return PlatformLinux::get();
#endif
  std::cerr << "Invalid platform" << std::endl;
  return nullptr;
}