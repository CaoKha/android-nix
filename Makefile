# Makefile for compiling C++ code, managing Java dependencies, compiling Java classes,
# running Java unit tests, and running C++ unit tests

# Variables
PROJECT_ROOT := $(shell pwd)
GRADLE_WRAPPER := $(PROJECT_ROOT)/gradlew

# Java Directories
JAVA_SRC_DIR := JNILib/src/main/java
JAVA_TEST_SRC_DIR := JNILib/src/test/java
ANDROID_TEST_SRC_DIR := JNILib/src/androidTest/java
JAVA_CLASSES_DIR := build/java/classes
JAVA_LIBS_DIR := build/java/libs

# Define the jniLibs directory for Android native libraries
JNI_LIBS_DIR := JNILib/src/main/jniLibs

# C++ Directories
CPP_LINUX_BUILD_DIR := build/cpp/linux
CPP_ANDROID_BUILD_DIR := build/cpp/android/$(ANDROID_ABI)
CPP_SOURCE_DIR := JNILib/src/main/cpp
CMAKELISTS_DIR := JNILib

# JAR Files and URLs
JUNIT_JAR := $(JAVA_LIBS_DIR)/junit-4.13.2.jar
JUNIT_URL := https://repo1.maven.org/maven2/junit/junit/4.13.2/junit-4.13.2.jar

HAMCREST_JAR := $(JAVA_LIBS_DIR)/hamcrest-core-1.3.jar
HAMCREST_URL := https://repo1.maven.org/maven2/org/hamcrest/hamcrest-core/1.3/hamcrest-core-1.3.jar

# AndroidX Test Libraries (AAR)
ANDROIDX_TEST_RUNNER_AAR := $(JAVA_LIBS_DIR)/androidx-test-runner-1.6.2.aar
ANDROIDX_TEST_CORE_AAR := $(JAVA_LIBS_DIR)/androidx-test-core-1.6.1.aar
ANDROIDX_TEST_EXT_JUNIT_AAR := $(JAVA_LIBS_DIR)/androidx-test-ext-junit-1.2.1.aar
ANDROIDX_TEST_EXPRESSO_CORE_AAR := $(JAVA_LIBS_DIR)/androidx-test-espresso-core-3.6.1.aar

# AndroidX Test Libraries (JAR)
ANDROIDX_ANNOTATION_JAR := $(JAVA_LIBS_DIR)/androidx-annotation-1.9.1.jar

# Extracted JARs from AARs
ANDROIDX_TEST_RUNNER_JAR := $(JAVA_LIBS_DIR)/androidx-test-runner-1.6.2.jar
ANDROIDX_TEST_CORE_JAR := $(JAVA_LIBS_DIR)/androidx-test-core-1.6.1.jar
ANDROIDX_TEST_EXT_JUNIT_JAR := $(JAVA_LIBS_DIR)/androidx-test-ext-junit-1.2.1.jar
ANDROIDX_TEST_EXPRESSO_CORE_JAR := $(JAVA_LIBS_DIR)/androidx-test-espresso-core-3.6.1.jar

# AndroidX Test Libraries URLs
ANDROIDX_TEST_RUNNER_URL := https://maven.google.com/androidx/test/runner/1.6.2/runner-1.6.2.aar
ANDROIDX_TEST_CORE_URL := https://maven.google.com/androidx/test/core/1.6.1/core-1.6.1.aar
ANDROIDX_TEST_EXT_JUNIT_URL := https://maven.google.com/androidx/test/ext/junit/1.2.1/junit-1.2.1.aar
ANDROIDX_ANNOTATION_URL := https://maven.google.com/androidx/annotation/annotation/1.9.1/annotation-1.9.1.jar
ANDROIDX_TEST_EXPRESSO_CORE_URL := https://maven.google.com/androidx/test/espresso/espresso-core/3.6.1/espresso-core-3.6.1.aar

# Native Library
NATIVE_LIB_SRC := $(CPP_LINUX_BUILD_DIR)/libnative-lib.so
NATIVE_LIB_DEST := $(JAVA_LIBS_DIR)/libnative-lib.so

# C++ Test Executable
CPP_TEST_EXEC := $(CPP_LINUX_BUILD_DIR)/checkupcomputing_test

# Java Test Class
JAVA_UNITTEST_CLASS := com.kolibree.CheckupComputerUnitTest

# Android Instrumented Test Variables
ANDROID_TEST_CLASSES_DIR := $(JAVA_CLASSES_DIR)
ANDROID_DEX_OUTPUT_DIR := build/java/dex/android_test
ANDROID_APK_DIR := build/java/apk/android_test
ANDROID_UNSIGNED_APK := $(ANDROID_APK_DIR)/app-unsigned.apk
ANDROID_SIGNED_APK := $(ANDROID_APK_DIR)/app-signed.apk
ANDROID_TEST_MANIFEST := JNILib/src/androidTest/AndroidManifest.xml
ANDROID_TEST_PACKAGE := com.kolibree
ANDROID_TEST_RUNNER := androidx.test.runner.AndroidJUnitRunner

# Keystore Configuration (Using project-relative debug keystore)
DEBUG_KEYSTORE := $(PROJECT_ROOT)/build/java/debug_keystore/debug.keystore
DEBUG_KEY_ALIAS := androiddebugkey
DEBUG_KEYSTORE_PASSWORD := android
DEBUG_KEY_PASSWORD := android

# Android SDK Configuration
# Path to android.jar for the specified API level
ANDROID_PLATFORM_JAR := $(ANDROID_HOME)/platforms/android-$(ANDROID_PLATFORM)/android.jar

# Emulator setup variables
AVD_NAME := pixel_emu
ANDROID_PLATFORM := 30
ANDROID_SYSTEM_IMAGE := $(SYSTEM_IMAGE)
AVD_DEVICE := pixel
AVDMANAGER := avdmanager

# Phony Targets
.PHONY: all download_jars compile_cpp_linux compile_cpp_android compile_java_classes \
        compile_java_tests run_java_tests run_cpp_linux_tests test_cpp test_java test_android \
				test_all clean

# Default Target
all: test_all

# Unified Test Target: Compiles everything and runs all tests
test_all: compile_cpp_linux run_cpp_linux_tests download_jars compile_java_classes compile_java_tests run_java_tests start_emulator run_android_test

# C++ Test Target: Compiles C++ components and runs C++ tests
test_cpp: compile_cpp_linux run_cpp_linux_tests

# Java Test Target: Compiles Java components and runs Java tests
test_java: download_jars compile_cpp_linux compile_java_classes compile_java_tests run_java_tests

# Android Instrumented Test Target: Compiles Java components and runs Android Instrumented Tests
test_android: compile_cpp_android start_emulator run_android_test

# Download JARs if they are missing
download_jars: $(JUNIT_JAR) \
	$(HAMCREST_JAR) \
	$(ANDROIDX_TEST_RUNNER_AAR) \
	$(ANDROIDX_TEST_CORE_AAR) \
	$(ANDROIDX_TEST_EXT_JUNIT_AAR) \
	$(ANDROIDX_TEST_EXPRESSO_CORE_AAR) \
	$(ANDROIDX_ANNOTATION_JAR)

# Rule to download JUnit JAR
$(JUNIT_JAR):
	@echo "Downloading JUnit JAR..."
	@mkdir -p $(JAVA_LIBS_DIR)
	@curl -L -o $@ $(JUNIT_URL) || { echo "Failed to download JUnit JAR"; exit 1; }
	@echo "JUnit JAR downloaded successfully."

# Rule to download Hamcrest JAR
$(HAMCREST_JAR):
	@echo "Downloading Hamcrest JAR..."
	@mkdir -p $(JAVA_LIBS_DIR)
	@curl -L -o $@ $(HAMCREST_URL) || { echo "Failed to download Hamcrest JAR"; exit 1; }
	@echo "Hamcrest JAR downloaded successfully."

# Rule to download Androidx Annotation JAR
$(ANDROIDX_ANNOTATION_JAR):
	@echo "Downloading Androidx Annotation JAR..."
	@mkdir -p $(JAVA_LIBS_DIR)
	@curl -L -o $@ $(ANDROIDX_ANNOTATION_URL) || { echo "Failed to download Androidx Annotation JAR"; exit 1; }
	@echo "Androidx Annotation JAR downloaded successfully."

# Rule to download AndroidX Test Runner AAR
$(ANDROIDX_TEST_RUNNER_AAR):
	@echo "Downloading AndroidX Test Runner AAR..."
	@mkdir -p $(JAVA_LIBS_DIR)
	@curl -L -f -s -S -o $@ $(ANDROIDX_TEST_RUNNER_URL) || { echo "Failed to download AndroidX Test Runner AAR"; exit 1; }
	@echo "AndroidX Test Runner AAR downloaded successfully."
	@echo "Extracting classes.jar from AndroidX Test Runner AAR..."
	@unzip -q -o $@ classes.jar -d $(JAVA_LIBS_DIR) || { echo "Failed to extract classes.jar from AndroidX Test Runner AAR"; exit 1; }
	@mv $(JAVA_LIBS_DIR)/classes.jar $(ANDROIDX_TEST_RUNNER_JAR) || { echo "Failed to rename classes.jar for AndroidX Test Runner"; exit 1; }
	@echo "classes.jar extracted and renamed to $(ANDROIDX_TEST_RUNNER_JAR)"
	
# Rule to download AndroidX Test Core AAR
$(ANDROIDX_TEST_CORE_AAR):
	@echo "Downloading AndroidX Test Core AAR..."
	@mkdir -p $(JAVA_LIBS_DIR)
	@curl -L -f -s -S -o $@ $(ANDROIDX_TEST_CORE_URL) || { echo "Failed to download AndroidX Test Core AAR"; exit 1; }
	@echo "AndroidX Test Core AAR downloaded successfully."
	@echo "Extracting classes.jar from AndroidX Test Core AAR..."
	@unzip -q -o $@ classes.jar -d $(JAVA_LIBS_DIR) || { echo "Failed to extract classes.jar from AndroidX Test Core AAR"; exit 1; }
	@mv $(JAVA_LIBS_DIR)/classes.jar $(ANDROIDX_TEST_CORE_JAR) || { echo "Failed to rename classes.jar for AndroidX Test Core"; exit 1; }
	@echo "classes.jar extracted and renamed to $(ANDROIDX_TEST_CORE_JAR)"

# Rule to download AndroidX Test Core AAR
$(ANDROIDX_TEST_EXPRESSO_CORE_AAR):
	@echo "Downloading AndroidX Test Expresso Core AAR..."
	@mkdir -p $(JAVA_LIBS_DIR)
	@curl -L -f -s -S -o $@ $(ANDROIDX_TEST_EXPRESSO_CORE_URL) || { echo "Failed to download AndroidX Test Expresso Core AAR"; exit 1; }
	@echo "AndroidX Test Expresso Core AAR downloaded successfully."
	@echo "Extracting classes.jar from AndroidX Test Expresso Core AAR..."
	@unzip -q -o $@ classes.jar -d $(JAVA_LIBS_DIR) || { echo "Failed to extract classes.jar from AndroidX Test Expresso Core AAR"; exit 1; }
	@mv $(JAVA_LIBS_DIR)/classes.jar $(ANDROIDX_TEST_EXPRESSO_CORE_JAR) || { echo "Failed to rename classes.jar for AndroidX Test Expresso Core"; exit 1; }
	@echo "classes.jar extracted and renamed to $(ANDROIDX_TEST_EXPRESSO_CORE_JAR)"
	
# Rule to download AndroidX Test Ext JUnit AAR
$(ANDROIDX_TEST_EXT_JUNIT_AAR):
	@echo "Downloading AndroidX Test Ext JUnit AAR..."
	@mkdir -p $(JAVA_LIBS_DIR)
	@curl -L -f -s -S -o $@ $(ANDROIDX_TEST_EXT_JUNIT_URL) || { echo "Failed to download AndroidX Test Ext JUnit AAR"; exit 1; }
	@echo "AndroidX Test Ext JUnit AAR downloaded successfully."
	@echo "Extracting classes.jar from AndroidX Test Ext JUnit AAR..."
	@unzip -q -o $@ classes.jar -d $(JAVA_LIBS_DIR) || { echo "Failed to extract classes.jar from AndroidX Test Ext JUnit AAR"; exit 1; }
	@mv $(JAVA_LIBS_DIR)/classes.jar $(ANDROIDX_TEST_EXT_JUNIT_JAR) || { echo "Failed to rename classes.jar for AndroidX Test Ext JUnit"; exit 1; }
	@echo "classes.jar extracted and renamed to $(ANDROIDX_TEST_EXT_JUNIT_JAR)"

# Generate Debug Keystore if it doesn't exist
$(DEBUG_KEYSTORE):
	@echo "Generating debug keystore if it doesn't exist..."
	@mkdir -p $(dir $@)
	@if [ ! -f $@ ]; then \
		keytool -genkey -v -keystore $@ -alias $(DEBUG_KEY_ALIAS) -storepass $(DEBUG_KEYSTORE_PASSWORD) \
			-keypass $(DEBUG_KEY_PASSWORD) -keyalg RSA -keysize 2048 -validity 10000 -dname "CN=Default Debug, OU=Development, O=Company, L=City, S=State, C=US"; \
		echo "Debug keystore generated successfully."; \
	else \
		echo "Debug keystore already exists at $@"; \
	fi

# Compile C++ Code for Linux
compile_cpp_linux:
	@echo "Compiling C++ code for Linux..."
	@mkdir -p $(CPP_LINUX_BUILD_DIR)
	cd $(CPP_LINUX_BUILD_DIR) && cmake $(PROJECT_ROOT)/$(CMAKELISTS_DIR) -G Ninja -DBUILD_TESTS=ON
	cd $(CPP_LINUX_BUILD_DIR) && ninja
	@echo "C++ compilation completed."
	@echo "Copying native library to $(JAVA_LIBS_DIR)..."
	@mkdir -p $(JAVA_LIBS_DIR)
	@cp $(CPP_LINUX_BUILD_DIR)/libnative-lib.so $(JAVA_LIBS_DIR)/ || { echo "Failed to copy native library"; exit 1; }
	@echo "Native library copied successfully."

# Compile C++ Code for Android
compile_cpp_android:
	@echo "Compiling C++ code for Android..."
	@mkdir -p $(CPP_ANDROID_BUILD_DIR)
	cd $(CPP_ANDROID_BUILD_DIR) && cmake $(PROJECT_ROOT)/$(CMAKELISTS_DIR) -G Ninja -DCROSS_COMPILE_ANDROID=ON
	cd $(CPP_ANDROID_BUILD_DIR) && ninja
	@echo "C++ compilation for Android completed."
	@echo "Checking and copying native library for Android to $(JNI_LIBS_DIR)..."
	@if [ -f $(CPP_ANDROID_BUILD_DIR)/libnative-lib.so ]; then \
		mkdir -p $(JNI_LIBS_DIR)/$(ANDROID_ABI); \
		cp $(CPP_ANDROID_BUILD_DIR)/libnative-lib.so $(JNI_LIBS_DIR)/$(ANDROID_ABI)/; \
		echo "Native library copied successfully to $(JNI_LIBS_DIR)/$(ANDROID_ABI)."; \
	else \
		echo "Native library not found: $(CPP_ANDROID_BUILD_DIR)/libnative-lib.so"; \
		exit 1; \
	fi

# Compile Main Java Classes
compile_java_classes:
	@echo "Compiling main Java classes..."
	@mkdir -p $(JAVA_CLASSES_DIR)
	@javac -d $(JAVA_CLASSES_DIR) $(wildcard $(JAVA_SRC_DIR)/com/kolibree/*.java) || { echo "Failed to compile main Java classes"; exit 1; }
	@echo "Main Java classes compiled successfully."

# Compile Test Java Classes
compile_java_tests: download_jars compile_java_classes
	@echo "Compiling test Java classes..."
	@javac -d $(JAVA_CLASSES_DIR) -cp $(JAVA_CLASSES_DIR):$(JUNIT_JAR):$(HAMCREST_JAR) $(wildcard $(JAVA_TEST_SRC_DIR)/com/kolibree/*.java) || { echo "Failed to compile test Java classes"; exit 1; }
	@echo "Test Java classes compiled successfully."

# Create AVD
create_avd:
	@echo "Checking if AVD named $(AVD_NAME) exists..."
	@if ! avdmanager list avd | grep -q $(AVD_NAME); then \
		echo "AVD $(AVD_NAME) does not exist. Creating AVD..."; \
		$(AVDMANAGER) create avd \
			-n $(AVD_NAME) \
			-k "system-images;android-$(ANDROID_PLATFORM);$(ANDROID_SYSTEM_IMAGE);$(ANDROID_ABI)" \
			-d $(AVD_DEVICE) || { echo "Failed to create AVD"; exit 1; }; \
	else \
		echo "AVD $(AVD_NAME) already exists."; \
	fi
	@echo "AVD $(AVD_NAME) created successfully."

# Start the emulator if not already running
start_emulator: stop_emulator create_avd
	@if ! adb shell exit 2>/dev/null; then \
		echo "Starting emulator $(AVD_NAME)..."; \
		emulator -avd $(AVD_NAME) -no-snapshot-load -no-window & \
		sleep 30; \
	else \
		echo "Emulator $(AVD_NAME) is already running."; \
	fi

stop_emulator:
	@echo "Stopping emulator if running..."
	@emulator_device=$$(adb devices | grep emulator | awk '{print $$1}'); \
	if [ -n "$$emulator_device" ]; then \
		echo "Emulator found: $$emulator_device, stopping..."; \
		adb -s $$emulator_device emu kill || { echo "Failed to stop the emulator"; exit 1; }; \
		sleep 5; \
		if adb devices | grep -q "$$emulator_device"; then \
			echo "Emulator $$emulator_device is still running, failed to stop."; \
			exit 1; \
		else \
			echo "Emulator $$emulator_device stopped successfully."; \
		fi \
	else \
		echo "No emulator is currently running."; \
	fi

# Run Java Unit Tests
run_java_tests:
	@echo "Running Java unit tests..."
	@java -cp $(JAVA_CLASSES_DIR):$(JUNIT_JAR):$(HAMCREST_JAR) \
	     -Djava.library.path=$(JAVA_LIBS_DIR) \
	     org.junit.runner.JUnitCore $(JAVA_UNITTEST_CLASS) || { echo "Java unit tests failed"; exit 1; }
	@echo "Java unit tests executed successfully."

# Define CPP_TEST_EXEC as a target that depends on compile_cpp_linux
$(CPP_TEST_EXEC): compile_cpp_linux
	@echo "C++ test executable is ready."

# Run C++ Unit Tests
run_cpp_linux_tests: $(CPP_TEST_EXEC)
	@echo "Running C++ unit tests..."
	@if [ -x "$(CPP_TEST_EXEC)" ]; then \
		"$(CPP_TEST_EXEC)" || { echo "C++ unit tests failed"; exit 1; }; \
	else \
		echo "C++ test executable not found or not executable: $(CPP_TEST_EXEC)"; \
		exit 1; \
	fi
	@echo "C++ unit tests executed successfully."

# Generate gradle wrapper
generate_gradle_wrapper:
	gradle wrapper --gradle-version 8.7

# Android X86_64 Instrumented Test
run_android_test:
	@$(GRADLE_WRAPPER) connectedAndroidTest --info

# Clean Build Artifacts
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf build/*
	@echo "Build artifacts cleaned."
