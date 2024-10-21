# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.10)

# Define the project
project(AndroidNativeLib)

# Option to determine if we're building tests or cross-compiling for Android
option(BUILD_TESTS "Build the checkup computing tests" OFF)
option(CROSS_COMPILE_ANDROID "Cross compile for Android" OFF)


# Set the cross-compilation toolchain if building for Android
if(CROSS_COMPILE_ANDROID)
  message(STATUS "Configuring for Android cross-compilation")
  set(CMAKE_SYSTEM_NAME Android)
  set(CMAKE_ANDROID_NDK $ENV{ANDROID_NDK_ROOT})
  set(CMAKE_ANDROID_ARCH_ABI arm64-v8a)
  set(CMAKE_ANDROID_API 21)

  # Set C++ standard to C++17
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)


  # Use Ninja for the build process
  set(CMAKE_MAKE_PROGRAM ninja)
  set(CMAKE_CXX_COMPILER "${CMAKE_ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android${CMAKE_ANDROID_API}-clang++")
  set(CMAKE_C_COMPILER "${CMAKE_ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/bin/aarch64-linux-android${CMAKE_ANDROID_API}-clang")

  # Prevent the compiler from using default include paths
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -nostdinc++ -nostdinc")

  # Include directories from the NDK
  include_directories("${CMAKE_ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/include")
  include_directories("${CMAKE_ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/include/aarch64-linux-android")
  include_directories("${CMAKE_ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/include/c++/v1")
  include_directories("${CMAKE_ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/lib64/clang/14.0.7/include")

  # Creates and names a library, sets it as either STATIC
  # or SHARED, and provides the relative paths to its source code.
  add_library(native-lib SHARED src/main/cpp/native-lib.cpp)

  # Link NDK libraries if building for Android
  find_library(log-lib log PATHS "${CMAKE_ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/lib/aarch64-linux-android/${CMAKE_ANDROID_API}" NO_DEFAULT_PATH)
  if(NOT log-lib)
    message(FATAL_ERROR "log library not found, please check your Android NDK path.")
  endif()
  target_link_libraries(native-lib ${log-lib})

else()
  message(STATUS "Configuring for native Linux build")
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)

  # For non-Android builds, use default system libraries
  set(CMAKE_MAKE_PROGRAM ninja)
  set(CMAKE_C_COMPILER gcc)
  set(CMAKE_CXX_COMPILER g++)

  # Creates and names a library, sets it as either STATIC
  # or SHARED, and provides the relative paths to its source code.
  add_library(native-lib SHARED src/main/cpp/native-lib.cpp)

  # Add an executable target for running tests if BUILD_TESTS is ON
  if(BUILD_TESTS)
    add_executable(checkupcomputing_test src/main/cpp/checkupcomputing_test.cpp src/main/cpp/checkupcomputing.cpp)
  endif()
endif()
