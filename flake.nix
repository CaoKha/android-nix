{
  # Description of the project
  description = "Android NDK C++ Project";

  # Define the inputs for the flake
  inputs = {
    # Pin the Nixpkgs input to a specific revision that includes glibc 2.35 for compatibility
    nixpkgs.url = "github:NixOS/nixpkgs/1b7a6a6e57661d7d4e0775658930059b77ce94a4";
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
            platformToolsVersion = "33.0.3";  # Version of Android Platform Tools
            buildToolsVersions = [ "30.0.3" ];  # Android Build Tools version
            platformVersions = [ "30" ];  # Target Android platform version
            includeEmulator = false;  # Do not include the Android emulator
            includeSources = false;  # Do not include Android sources
            includeSystemImages = false;  # Do not include Android system images
            includeNDK = true;  # Include the Android NDK
            ndkVersions = [ "25.1.8937393" ];
          };

          # Generate a development shell for each specified Android ABI
          devShellsForABIs = lib.genAttrs (["default"] ++ androidABIs) (abi:
            let
              # Default ABI is arm64-v8a if "default" is specified
              actualAbi = if abi == "default" then "arm64-v8a" else abi;
            in
            pkgs.mkShell {
              # Add necessary build inputs to the shell environment
              buildInputs = with pkgs; [
                androidPackages.androidsdk  # Android SDK
                gradle  # Gradle for building Java and Android projects
                jdk17  # Full JDK version for JNI and logging support
                cmake  # CMake for project configuration
                ninja  # Ninja build system
                gcc  # GCC compiler
                gdb  # GNU Debugger
                git  # Git for version control
                which  # Utility to locate binaries
                clang  # Clang compiler
                bear  # Bear tool for generating compile_commands.json
                pkgs.bash  # Use Nix-provided bash to avoid conflicts
                pkgs.coreutils  # Use Nix-provided core utilities
              ];

              # Configure environment variables in the shell
              shellHook = ''
                # Set Android SDK root directory
                export ANDROID_HOME=${androidPackages.androidsdk}/libexec/android-sdk

                # Automatically determine the latest NDK version
                export ANDROID_NDK=$(ls -d $ANDROID_HOME/ndk/* | sort -V | tail -n1)

                # Set the Java home directory for JNI support
                export JAVA_HOME=${pkgs.jdk17}

                # Set the target Android ABI
                export ANDROID_ABI=${actualAbi}

                # Set the target Android platform (API level)
                export ANDROID_API_LEVEL=30

                # Get the Clang version from the Android NDK
                export CLANG_VERSION=$($ANDROID_NDK/toolchains/llvm/prebuilt/linux-x86_64/bin/clang --version | head -n 1 | awk '{print $8}')

                # Unset the CPLUS_INCLUDE_PATH to avoid conflicts with Nixpkgs settings
                unset CPLUS_INCLUDE_PATH

                # Display environment configuration
                echo "Android NDK development environment loaded"
                echo "Android SDK root: $ANDROID_HOME"
                echo "Android NDK root: $ANDROID_NDK"
                echo "ANDROID_ABI set to: $ANDROID_ABI"
                echo "ANDROID_API_LEVEL set to: $ANDROID_API_LEVEL"
                echo "JAVA_HOME set to: $JAVA_HOME"
                echo "CLANG_VERSION: $CLANG_VERSION"
              '';
            });
        in
        devShellsForABIs
      );
    };
}
