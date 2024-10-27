# Makefile for compiling C++ code, managing Java dependencies, compiling Java classes,
# running Java unit tests, and running C++ unit tests

# Variables
PROJECT_ROOT := $(shell pwd)

# Java Directories
JAVA_SRC_DIR := JNILib/src/main/java
JAVA_TEST_SRC_DIR := JNILib/src/test/java
JAVA_CLASSES_DIR := build/java/classes
JAVA_LIBS_DIR := build/java/libs

# C++ Directories
CPP_LINUX_BUILD_DIR := build/cpp/linux
CPP_ANDROID_BUILD_DIR := build/cpp/android
CPP_SOURCE_DIR := JNILib/src/main/cpp
CMAKELISTS_DIR := JNILib

# JAR Files and URLs
JUNIT_JAR := $(JAVA_LIBS_DIR)/junit-4.13.2.jar
JUNIT_URL := https://repo1.maven.org/maven2/junit/junit/4.13.2/junit-4.13.2.jar

HAMCREST_JAR := $(JAVA_LIBS_DIR)/hamcrest-core-1.3.jar
HAMCREST_URL := https://repo1.maven.org/maven2/org/hamcrest/hamcrest-core/1.3/hamcrest-core-1.3.jar

# Native Library
NATIVE_LIB_SRC := $(CPP_LINUX_BUILD_DIR)/libnative-lib.so
NATIVE_LIB_DEST := $(JAVA_LIBS_DIR)/libnative-lib.so

# C++ Test Executable
CPP_TEST_EXEC := $(CPP_LINUX_BUILD_DIR)/checkupcomputing_test

# Java Test Class
TEST_CLASS := com.kolibree.CheckupComputerUnitTest

# Phony Targets
.PHONY: all download_jars compile_cpp_linux compile_cpp_android compile_java_classes \
        compile_java_tests run_java_tests run_cpp_linux_tests test_cpp test_java test_all clean

# Default Target
all: test_all

# Unified Test Target: Compiles everything and runs all tests
test_all: compile_cpp_linux run_cpp_linux_tests download_jars compile_java_classes compile_java_tests run_java_tests 

# C++ Test Target: Compiles C++ components and runs C++ tests
test_cpp: compile_cpp_linux run_cpp_linux_tests

# Java Test Target: Compiles Java components and runs Java tests
test_java: download_jars compile_cpp_linux compile_java_classes compile_java_tests run_java_tests

# Download JARs if they are missing
download_jars: $(JUNIT_JAR) $(HAMCREST_JAR)

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

# Compile C++ Code for Android (Optional)
compile_cpp_android:
	@echo "Compiling C++ code for Android..."
	@mkdir -p $(CPP_ANDROID_BUILD_DIR)
	cd $(CPP_ANDROID_BUILD_DIR) && cmake $(PROJECT_ROOT)/$(CMAKELISTS_DIR) -G Ninja -DCROSS_COMPILE_ANDROID=ON
	cd $(CPP_ANDROID_BUILD_DIR) && ninja
	@echo "C++ compilation for Android completed."

# Compile Main Java Classes
compile_java_classes:
	@echo "Compiling main Java classes..."
	@mkdir -p $(JAVA_CLASSES_DIR)
	@javac -d $(JAVA_CLASSES_DIR) $(wildcard $(JAVA_SRC_DIR)/com/kolibree/*.java) || { echo "Failed to compile main Java classes"; exit 1; }
	@echo "Main Java classes compiled successfully."

# Compile Test Java Classes
compile_java_tests:
	@echo "Compiling test Java classes..."
	@javac -d $(JAVA_CLASSES_DIR) -cp $(JAVA_CLASSES_DIR):$(JUNIT_JAR):$(HAMCREST_JAR) $(wildcard $(JAVA_TEST_SRC_DIR)/com/kolibree/*.java) || { echo "Failed to compile test Java classes"; exit 1; }
	@echo "Test Java classes compiled successfully."

# Run Java Unit Tests
run_java_tests:
	@echo "Running Java unit tests..."
	@java -cp $(JAVA_CLASSES_DIR):$(JUNIT_JAR):$(HAMCREST_JAR) \
	     -Djava.library.path=$(JAVA_LIBS_DIR) \
	     org.junit.runner.JUnitCore $(TEST_CLASS) || { echo "Java unit tests failed"; exit 1; }
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

# Clean Build Artifacts
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(JAVA_CLASSES_DIR)
	@rm -rf $(JAVA_LIBS_DIR)
	@rm -rf $(CPP_LINUX_BUILD_DIR)
	@rm -rf $(CPP_ANDROID_BUILD_DIR)
	@echo "Build artifacts cleaned."
