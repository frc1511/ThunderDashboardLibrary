cmake_minimum_required(VERSION 3.1)

set(TD_VERSION_MAJOR "0")
set(TD_VERSION_MINOR "1")
set(TD_VERSION_PATCH "0")
set(TD_VERSION "${TD_VERSION_MAJOR}.${TD_VERSION_MINOR}.${TD_VERSION_PATCH}")
set(TD_VERSION_SHORT "${TD_VERSION_MAJOR}.${TD_VERSION_MINOR}")

list(APPEND TD_DEF_LIST "TD_VERSION=${TD_VERSION_SHORT}")
list(APPEND TD_DEF_LIST "TD_VERSION_MAJOR=${TD_VERSION_MAJOR}")
list(APPEND TD_DEF_LIST "TD_VERSION_MINOR=${TD_VERSION_MINOR}")
list(APPEND TD_DEF_LIST "TD_VERSION_PATCH=${TD_VERSION_PATCH}")
list(APPEND TD_DEF_LIST "TD_VERSION_STR=\"${TD_VERSION}\"")

project(ThunderDashboard LANGUAGES C CXX VERSION ${TD_VERSION})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
  #add_compile_options(/W4)
  message(STATUS "MSVC Compiler Detected")
else()
  add_compile_options(-Wall -Wextra -pedantic)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  set(TD_DEBUG ON)
  list(APPEND TD_DEF_LIST "TD_DEBUG")
endif()

if("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
  message(FATAL_ERROR "In-source builds are not allowed.\nUse a seperate directory for build files.")
endif()

option(TD_WITH_CS "Build with camera server" ON)
option(TD_DOWNLOAD_WPILIB "Download WPILib binaries instead of building them" OFF)
set(TD_DOWNLOAD_WPILIB_VERSION "" CACHE STRING "WPILib version to download")

if(NOT TD_DOWNLOAD_WPILIB_VERSION)
  set(TD_DOWNLOAD_WPILIB_VERSION "2022.4.1")
endif()

if (TD_WITH_CS)
  list(APPEND TD_DEF_LIST "TD_WITH_CS")
endif()

set(TD_LIB_DIR        "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(TD_INC_DIR        "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(TD_THIRDPARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")

set(TD_RES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/res")

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/util.cmake")

if(TD_WINDOWS)
  list(APPEND TD_DEF_LIST "THUNDER_WINDOWS")
  set(TD_DOWNLOAD_OS "windows")
  set(OpenCV_DIR "C:\\Program Files\\opencv\\build")
elseif(TD_MACOS)
  list(APPEND TD_DEF_LIST "THUNDER_MACOS")
  set(TD_DOWNLOAD_OS "osx")
elseif(TD_LINUX)
  list(APPEND TD_DEF_LIST "THUNDER_LINUX")
  set(TD_DOWNLOAD_OS "linux")
else()
  message(FATAL_ERROR "Target operating system '${CMAKE_SYSTEM_NAME}' not supported")
endif()

set(TD_DOWNLOAD_ARCH "x86-64")

if(TD_MACOS)
  enable_language(OBJC)
  set(CMAKE_OBJC_STANDARD 11)
  set(CMAKE_OBJC_STANDARD_REQUIRED YES)
  set(CMAKE_OBJC_EXTENSIONS OFF)
  
  enable_language(OBJCXX)
  set(CMAKE_OBJCXX_STANDARD 17)
  set(CMAKE_OBJCXX_STANDARD_REQUIRED YES)
  set(CMAKE_OBJCXX_EXTENSIONS OFF)
endif()

# --- Sources ---

function(add_dashboard_sources)
  set(TD_SRC ${TD_SRC} ${ARGV} PARENT_SCOPE)
endfunction()

include("${TD_LIB_DIR}/CMakeLists.txt")

# --- Resources ---

build_dashboard_resource("${TD_RES_DIR}/fonts/Roboto/Roboto-Regular.ttf"       "Roboto_Regular_ttf")
build_dashboard_resource("${TD_RES_DIR}/fonts/Roboto/Roboto-Bold.ttf"          "Roboto_Bold_ttf")
build_dashboard_resource("${TD_RES_DIR}/fonts/Ubuntu/Ubuntu-Regular.ttf"       "Ubuntu_Regular_ttf")
build_dashboard_resource("${TD_RES_DIR}/fonts/Ubuntu/Ubuntu-Bold.ttf"          "Ubuntu_Bold_ttf")
build_dashboard_resource("${TD_RES_DIR}/fonts/Font-Awesome/fa-regular-400.ttf" "FontAwesome_Regular_ttf")
build_dashboard_resource("${TD_RES_DIR}/fonts/Font-Awesome/fa-solid-900.ttf"   "FontAwesome_Solid_ttf")

build_dashboard_resource("${HD_RES_DIR}/images/no_camera.png" "no_camera_png")

# --- Thirdparty ---

option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_BUILD_DOCS OFF)
option(GLFW_INSTALL OFF)
add_subdirectory("${TD_THIRDPARTY_DIR}/glfw")
add_subdirectory("${TD_THIRDPARTY_DIR}/imgui")
add_subdirectory("${TD_THIRDPARTY_DIR}/IconFontCppHeaders")
add_subdirectory("${TD_THIRDPARTY_DIR}/glad")
add_subdirectory("${TD_THIRDPARTY_DIR}/fmt")
add_subdirectory("${TD_THIRDPARTY_DIR}/stb_image")

set(TD_WPILIB_DOWNLOAD_DIR "${CMAKE_CURRENT_BINARY_DIR}/wpilib_${TD_DOWNLOAD_WPILIB_VERSION}")
string(REPLACE "." "_" TD_WPILIB_DOWNLOAD_DIR ${TD_WPILIB_DOWNLOAD_DIR})

# WPILib ntcore and cscore.
if(TD_DOWNLOAD_WPILIB)
  function(download_lib LIB_NAME LIB_URL LIB_DEST)
    message(STATUS "Downloading ${LIB_URL}")
    file(DOWNLOAD ${LIB_URL} ${LIB_DEST})
    message(STATUS "Downloaded ${LIB_NAME}")
  endfunction()

  function(extract_lib LIB_NAME ZIP_DIR LIB_DEST)
    message(STATUS "Extracting ${ZIP_DIR}")
    file(ARCHIVE_EXTRACT INPUT "${ZIP_DIR}" DESTINATION "${LIB_DEST}")
    message(STATUS "Extracted ${LIB_NAME}")
  endfunction()

  function(download_extract_lib LIB_NAME)
    set(LIB_URL "https://frcmaven.wpi.edu/artifactory/release/edu/wpi/first/${LIB_NAME}/${LIB_NAME}-cpp/${TD_DOWNLOAD_WPILIB_VERSION}/${LIB_NAME}-cpp-${TD_DOWNLOAD_WPILIB_VERSION}-${TD_DOWNLOAD_OS}${TD_DOWNLOAD_ARCH}static.zip")
    set(LIB_DEST "${TD_WPILIB_DOWNLOAD_DIR}/${LIB_NAME}")

    download_lib(${LIB_NAME} ${LIB_URL} "${LIB_DEST}.zip")
    extract_lib(${LIB_NAME} "${LIB_DEST}.zip" ${LIB_DEST})
  endfunction()

  function(download_extract_headers LIB_NAME)
    set(INC_URL "https://frcmaven.wpi.edu/artifactory/release/edu/wpi/first/${LIB_NAME}/${LIB_NAME}-cpp/${TD_DOWNLOAD_WPILIB_VERSION}/${LIB_NAME}-cpp-${TD_DOWNLOAD_WPILIB_VERSION}-headers.zip")

    set(INC_DEST "${TD_WPILIB_DOWNLOAD_DIR}/${LIB_NAME}_include")

    download_lib(${LIB_NAME} ${INC_URL} "${INC_DEST}.zip")
    extract_lib(${LIB_NAME} "${INC_DEST}.zip" ${INC_DEST})
  endfunction()

  download_extract_lib("ntcore")
  download_extract_headers("ntcore")

  download_extract_lib("wpiutil")
  download_extract_headers("wpiutil")

  if(TD_WITH_CS)
    download_extract_lib("cscore")
    download_extract_headers("cscore")
  endif()
  
else()
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${TD_THIRDPARTY_DIR}/allwpilib/cmake/modules")
  set(WPILIB_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

  set(WITH_JAVA OFF CACHE BOOL "" FORCE)
  set(WITH_TESTS OFF CACHE BOOL "" FORCE)

  add_subdirectory("${TD_THIRDPARTY_DIR}/allwpilib/wpinet")
  add_subdirectory("${TD_THIRDPARTY_DIR}/allwpilib/wpiutil")
  add_subdirectory("${TD_THIRDPARTY_DIR}/allwpilib/ntcore")

  #WPILib CS Core.
  if(TD_WITH_CS)
    add_subdirectory("${TD_THIRDPARTY_DIR}/allwpilib/cscore")
  endif()
endif()

# --- Build ---
add_library(${PROJECT_NAME} STATIC ${TD_SRC})

target_include_directories(${PROJECT_NAME} PUBLIC ${TD_INC_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${TD_LIB_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/res")

target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
target_link_libraries(${PROJECT_NAME} PUBLIC imgui)
target_link_libraries(${PROJECT_NAME} PUBLIC icons_font_awesome)
target_link_libraries(${PROJECT_NAME} PUBLIC glad)
target_link_libraries(${PROJECT_NAME} PUBLIC fmt)
target_link_libraries(${PROJECT_NAME} PUBLIC stb_image)

if(TD_DOWNLOAD_WPILIB)
  target_link_directories(${PROJECT_NAME} PUBLIC ${TD_WPILIB_DOWNLOAD_DIR})

  function(find_lib_dir LIB_DIR LIB_NAME)
    find_library(${LIB_DIR}
      NAMES ${LIB_NAME}
      PATHS "${TD_WPILIB_DOWNLOAD_DIR}/${LIB_NAME}/${TD_DOWNLOAD_OS}/${TD_DOWNLOAD_ARCH}/static"
      PATH_SUFFIXES "lib" "lib32" "lib64"
    )
  endfunction()

  function(find_inc_dir INC_DIR LIB_NAME)
    set(${INC_DIR} "${TD_WPILIB_DOWNLOAD_DIR}/${LIB_NAME}_include" PARENT_SCOPE)
  endfunction()

  find_lib_dir(NTCORE_LIB_DIR "ntcore")
  find_inc_dir(NTCORE_INC_DIR "ntcore")
  target_link_libraries(${PROJECT_NAME} PUBLIC ${NTCORE_LIB_DIR})
  target_include_directories(${PROJECT_NAME} PUBLIC ${NTCORE_INC_DIR})

  find_lib_dir(WPIUTIL_LIB_DIR "wpiutil")
  find_inc_dir(WPIUTIL_INC_DIR "wpiutil")
  target_link_libraries(${PROJECT_NAME} PUBLIC ${WPIUTIL_LIB_DIR})
  target_include_directories(${PROJECT_NAME} PUBLIC ${WPIUTIL_INC_DIR})

  if(TD_WITH_CS)
    find_package(OpenCV REQUIRED)
    if(NOT OpenCV_FOUND)
      message(FATAL_ERROR "OpenCV not found. Install OpenCV or add flag -DTD_WITH_CS=0")
    endif()

    target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBS})
    target_include_directories(${PROJECT_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS})

    find_lib_dir(CSCORE_LIB_DIR "cscore")
    find_inc_dir(CSCORE_INC_DIR "cscore")
    target_link_libraries(${PROJECT_NAME} PUBLIC ${CSCORE_LIB_DIR})
    target_include_directories(${PROJECT_NAME} PUBLIC ${CSCORE_INC_DIR})
  endif()
else()
  target_link_libraries(${PROJECT_NAME} PUBLIC ntcore)

  if(TD_WITH_CS)
    target_link_libraries(${PROJECT_NAME} PUBLIC cscore)
  endif()
endif()

if(TD_MACOS)
  target_link_libraries(${PROJECT_NAME} PUBLIC "-framework Foundation -framework AppKit -framework UniformTypeIdentifiers")
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC ${TD_DEF_LIST})