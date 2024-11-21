{
  # Description of the project
  description = "Android NDK C++ Project";

  # Define the inputs for the flake
  inputs = {
    # Pin the Nixpkgs input to a specific revision that includes glibc 2.35 for compatibility
    # nixpkgs.url = "github:NixOS/nixpkgs/7ad7b570e96a3fd877e5fb08b843d66a30428f12";
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.05";
  };

  # Define the outputs
  outputs = { self, nixpkgs }:
    let
      # Import commonly used Nix libraries
      lib = nixpkgs.lib;

      # Target systems for the development environment
      systems = [ "x86_64-linux" ];

      # Supported Android ABIs (Application Binary Interfaces)
      androidABIs = [ "armeabi-v7a" "arm64-v8a" "x86" "x86_64" ];
    in
    {
      # Generate a devShell for each system
      devShells = lib.genAttrs systems (system:
        let
          # Import the Nixpkgs configuration for the specified system
          pkgs = import nixpkgs {
            inherit system;
            config = {
              allowUnfree = true;  # Allow unfree packages
              android_sdk = {
                accept_license = true;  # Accept Android SDK license automatically
              };
            };
          };

          # Configure Android packages, including SDK and NDK
          androidPackages = pkgs.androidenv.composeAndroidPackages {
            toolsVersion = "26.1.1";  # Version of Android SDK tools
            platformToolsVersion = "34.0.5";  # Version of Android Platform Tools
            buildToolsVersions = [ "34.0.0" ];  # Android Build Tools version
            platformVersions = [ "30" ];  # Target Android platform version
            includeEmulator = true;  # Do not include the Android emulator
            includeSources = false;  # Do not include Android sources
            includeSystemImages = true;  # Do not include Android system images
            systemImageTypes = ["google_apis_playstore"];
            abiVersions = ["arm64-v8a" "armeabi-v7a" "x86" "x86_64"];
            includeNDK = true;  # Include the Android NDK
            ndkVersions = [ "26.3.11579264" ];
          };

          # Specify the Build Tools, Cmake version to use
          buildToolsVersion = "34.0.0";
          systemImageType = "google_apis_playstore";

          # Generate a development shell for each specified Android ABI
          devShellsForABIs = lib.genAttrs (["default"] ++ androidABIs) (abi:
            let
              # Default ABI is arm64-v8a if "default" is specified
              actualAbi = if abi == "default" then "x86_64" else abi;
            in
            pkgs.mkShell {
              # Add necessary build inputs to the shell environment
              buildInputs = with pkgs; [
                androidPackages.androidsdk  # Android SDK
                gradle # Gradle for building Java and Android projects
                jdk17  # Full JDK version for JNI and logging support
                cmake  # CMake for project configuration
                ninja  # Ninja build system
                gcc  # GCC compiler
                gdb  # GNU Debugger
                git  # Git for version control
                which  # Utility to locate binaries
                zip
                bear  # Bear tool for generating compile_commands.json
                bash  # Use Nix-provided bash to avoid conflicts
                coreutils  # Use Nix-provided core utilities
              ];

              # Configure environment variables in the shell
              shellHook = ''
                # Set Android SDK root directory
                export ANDROID_HOME=${androidPackages.androidsdk}/libexec/android-sdk
                # export ANDROID_SDK_ROOT=$ANDROID_HOME # ANDROID_SDK_ROOT var is used by emulator

                # Automatically determine the latest NDK version
                # export ANDROID_NDK=$(ls -d $ANDROID_HOME/ndk/* | sort -V | tail -n1)
                export ANDROID_NDK=$ANDROID_HOME/ndk-bundle

                # Set the Java home directory for JNI support
                export JAVA_HOME=${pkgs.jdk17}

                # Set the target Android ABI
                export ANDROID_ABI=${actualAbi}

                # Set the target Android platform (API level)
                export ANDROID_PLATFORM=30

                # Get the Clang version from the Android NDK
                export CLANG_VERSION=$($ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/clang --version | head -n 1 | awk '{print $12}')

                # Unset the CPLUS_INCLUDE_PATH to avoid conflicts with Nixpkgs settings
                unset CPLUS_INCLUDE_PATH

                # Set SYSTEM_IMAGE
                export SYSTEM_IMAGE=${systemImageType}

                # Add build-tools to PATH (for d8)
                export PATH="$ANDROID_HOME/build-tools/${buildToolsVersion}:$PATH"

                # Display environment configuration
                echo "Android NDK development environment loaded"
                echo "Android SDK root: $ANDROID_HOME"
                echo "Android NDK root: $ANDROID_NDK"
                echo "ANDROID_ABI set to: $ANDROID_ABI"
                echo "ANDROID_PLATFORM set to: $ANDROID_PLATFORM"
                echo "SYSTEM_IMAGE set to: $SYSTEM_IMAGE"
                echo "JAVA_HOME set to: $JAVA_HOME"
                echo "CLANG_VERSION: $CLANG_VERSION"
              '';
            });
        in
        devShellsForABIs
      );
    };
}
