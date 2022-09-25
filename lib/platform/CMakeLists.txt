set(TD_PLATFORM_DIR "${TD_LIB_DIR}/platform")

add_dashboard_sources(
  "${TD_PLATFORM_DIR}/platform.cpp"
)

if(TD_MACOS)
  include("${TD_PLATFORM_DIR}/macos/CMakeLists.txt")
elseif(TD_WINDOWS)
  include("${TD_PLATFORM_DIR}/windows/CMakeLists.txt")
elseif(TD_LINUX)
  include("${TD_PLATFORM_DIR}/linux/CMakeLists.txt")
endif()
