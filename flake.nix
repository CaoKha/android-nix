{
  description = "Android NDK C++ Project";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.05";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachSystem ([
      "x86_64-linux"
      "aarch64-linux-android"
    ]) (system:
      let
        pkgs = import nixpkgs {
          inherit system;
          config = {
            allowUnfree = true;
            android_sdk = {
              accept_license = true;
            };
          };
        };

        androidPackages = pkgs.androidenv.composeAndroidPackages {
          toolsVersion = "26.1.1";
          platformToolsVersion = "33.0.3";
          buildToolsVersions = [ "30.0.3" ];
          platformVersions = [ "30" ];
          includeEmulator = false;
          includeSources = false;
          includeSystemImages = false;
          includeNDK = true;
          ndkVersions = [ "25.2.9519653" ];
        };
      in
      {
        devShell = pkgs.mkShell {
          buildInputs = with pkgs; [
            androidPackages.androidsdk
            gradle
            jdk11  # Use a full JDK version with JNI and logging support
            cmake
            ninja
            gcc
            gdb
            git
            which
            clang
          ];

          shellHook = ''
            # Set Android SDK root
            export ANDROID_SDK_ROOT=${androidPackages.androidsdk}/libexec/android-sdk
            export ANDROID_NDK_ROOT=$(ls -d $ANDROID_SDK_ROOT/ndk/* | sort -V | tail -n1)

            # Set JNI include paths for Clang to find jni.h
            export JAVA_HOME=${pkgs.jdk11}

            echo "Android NDK development environment loaded"
            echo "Android SDK root: $ANDROID_SDK_ROOT"
            echo "Android NDK root: $ANDROID_NDK_ROOT"
            echo "JAVA_HOME set to: $JAVA_HOME"
          '';
        };
      });
}
