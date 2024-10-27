cmake_minimum_required(VERSION 3.18)

# Define build options
option(BUILD_TESTS "Build the checkup computing tests" OFF)
option(CROSS_COMPILE_ANDROID "Cross compile for Android" OFF)

# Conditional configuration based on whether cross-compiling for Android
if(CROSS_COMPILE_ANDROID)
  message(STATUS "Configuring for Android cross-compilation")

  # Retrieve environment variables into CMake variables
  set(ANDROID_NDK $ENV{ANDROID_NDK})
  set(ANDROID_ABI $ENV{ANDROID_ABI})
  set(ANDROID_API_LEVEL $ENV{ANDROID_API_LEVEL})

  # Validate necessary environment variables
  if(NOT ANDROID_NDK)
    message(FATAL_ERROR
    "ANDROID_NDK environment variable is not set.
    Please set it to your Android NDK path.")
  endif()

  if(NOT ANDROID_ABI)
    message(FATAL_ERROR
    "ANDROID_ABI environment variable is not set.
    Please set it to your target ABI (e.g., arm64-v8a).")
  endif()

  if(NOT ANDROID_API_LEVEL)
    message(FATAL_ERROR
    "ANDROID_API_LEVEL environment variable is not set.
    Please set it to your target platform (e.g., 21).")
  endif()

  # Set the Android NDK toolchain file
  set(CMAKE_TOOLCHAIN_FILE "${ANDROID_NDK}/build/cmake/android.toolchain.cmake")

  # Set C++ standard to C++17
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

  # Determine Android architecture and triple based on ANDROID_ABI
  if(ANDROID_ABI STREQUAL "armeabi-v7a")
    set(ANDROID_ARCH "arm")
    set(ANDROID_TRIPLE "arm-linux-androideabi")
  elseif(ANDROID_ABI STREQUAL "arm64-v8a")
    set(ANDROID_ARCH "arm64")
    set(ANDROID_TRIPLE "aarch64-linux-android")
  elseif(ANDROID_ABI STREQUAL "x86")
    set(ANDROID_ARCH "x86")
    set(ANDROID_TRIPLE "i686-linux-android")
  elseif(ANDROID_ABI STREQUAL "x86_64")
    set(ANDROID_ARCH "x86_64")
    set(ANDROID_TRIPLE "x86_64-linux-android")
  else()
    message(FATAL_ERROR "Unsupported ANDROID_ABI: ${ANDROID_ABI}")
  endif()

  # Specify the Clang compilers provided by the NDK
  set(CMAKE_C_COMPILER
    "${ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/bin/${ANDROID_TRIPLE}${ANDROID_API_LEVEL}-clang")
  set(CMAKE_CXX_COMPILER
    "${ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/bin/${ANDROID_TRIPLE}${ANDROID_API_LEVEL}-clang++")

  # Set the project name
  project(NativeLib)

  # Add the native library
  add_library(native-lib SHARED
    src/main/cpp/native-lib.cpp
    src/main/cpp/checkupcomputing.cpp)

  # Find the log library using the NDK's defaults
  find_library(log-lib log
    PATHS
      ${ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/lib/${ANDROID_TRIPLE}/${ANDROID_API_LEVEL}
      NO_DEFAULT_PATH)
  if(NOT log-lib)
    message(FATAL_ERROR
    "log library not found.
    Please check your Android NDK installation. ${ANDROID_API_LEVEL}")
  else()
    message(STATUS "log library found: ${log-lib}")
  endif()

  # Link the log library to native-lib
  target_link_libraries(native-lib ${log-lib})

else()
  message(STATUS "Configuring for native Linux build")

  # Set C++ standard
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

  # Set the project name
  project(NativeLib)

  # Retrieve JAVA_HOME from the environment
  set(JAVA_HOME "$ENV{JAVA_HOME}")

  if(NOT JAVA_HOME)
    message(FATAL_ERROR "JAVA_HOME environment variable is not set.")
  endif()

  # Specify the path to libjvm.so
  set(JVM_LIBRARY_PATH "${JAVA_HOME}/lib/openjdk/lib/server/libjvm.so")

  # Verify that libjvm.so exists at the specified path
  if(NOT EXISTS "${JVM_LIBRARY_PATH}")
    message(FATAL_ERROR "libjvm.so not found at ${JVM_LIBRARY_PATH}")
  endif()

  # Add the native library
  add_library(native-lib SHARED
    src/main/cpp/native-lib.cpp
    src/main/cpp/checkupcomputing.cpp)

  # # Include JNI headers
  # # (somehow this doesn't add the -I$JAVA_HOME/include flag
  # # in compile commands so I have to comment this out)
  # target_include_directories(native-lib PRIVATE
  #     "${JAVA_HOME}/include"
  #     "${JAVA_HOME}/include/linux"
  # )

  # Explicitly add -I flags using target_compile_options
  target_compile_options(native-lib PRIVATE
      -I${JAVA_HOME}/include
      -I${JAVA_HOME}/include/linux
  )

  # Link against libjvm.so
  target_link_libraries(native-lib "${JVM_LIBRARY_PATH}")

  # Optionally, build tests
  if(BUILD_TESTS)
    # Add executable for tests
    add_executable(checkupcomputing_test
      src/main/cpp/checkupcomputing_test.cpp
      src/main/cpp/checkupcomputing.cpp)
  endif()
endif()
