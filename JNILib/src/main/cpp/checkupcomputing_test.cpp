#include "checkupcomputing.h"
#include <cassert>
#include <iostream>
#include <vector>

using namespace kolibree;

template <typename EnumType> struct TestData {
  struct BrushingPassForZone {
    EnumType zone;
    float timestamp;
    float duration;
  };
  struct CoverageForZone {
    EnumType zone;
    float coverage;
  };
  using Inputs = std::vector<BrushingPassForZone>;
  using Outputs = std::vector<CoverageForZone>;
  Inputs inputs;
  Outputs outputs;
};

// Test data for MouthZones8
const TestData<MouthZones8> testData8 = {
    {{MouthZones8::UpLeftExt8, 0.f, 1.f},
     {MouthZones8::UpRightInt8, 1.f, 4.f},
     {MouthZones8::UpLeftExt8, 5.f, 1.f},
     {MouthZones8::LoLeftExt8, 6.f, 10.f},
     {MouthZones8::LoRightExt8, 16.f, 5.f},
     {MouthZones8::LoLeftExt8, 21.f, 3.f},
     {MouthZones8::UpRightExt8, 24.f, 6.f}},
    {{MouthZones8::UpLeftExt8, 0.2f},
     {MouthZones8::UpRightInt8, 0.4f},
     {MouthZones8::LoLeftExt8, 1.f},
     {MouthZones8::LoRightExt8, 0.5f},
     {MouthZones8::UpRightExt8, 0.6f}}};

// Test data for MouthZones12
const TestData<MouthZones12> testData12 = {
    {{MouthZones12::UpIncisorExt12, 0.f, 1.5f},
     {MouthZones12::UpMolarLeftInt12, 1.5f, 4.f},
     {MouthZones12::UpIncisorExt12, 5.5f, 1.f},
     {MouthZones12::LoIncisorExt12, 6.5f, 2.3f},
     {MouthZones12::LoMolarRightExt12, 8.8f, 4.3f},
     {MouthZones12::LoIncisorExt12, 13.1f, 9.f},
     {MouthZones12::UpMolarRightExt12, 22.1f, 6.f}},
    {{MouthZones12::UpIncisorExt12, 0.25f},
     {MouthZones12::UpMolarLeftInt12, 0.4f},
     {MouthZones12::LoIncisorExt12, 1.0f},
     {MouthZones12::LoMolarRightExt12, 0.43f},
     {MouthZones12::UpMolarRightExt12, 0.6f}}};

// Test data for MouthZones16
const TestData<MouthZones16> testData16 = {
    {{MouthZones16::LoMolarLeftInt16, 0.f, 2.5f},
     {MouthZones16::UpIncisorInt16, 2.5f, 3.f},
     {MouthZones16::LoIncisorExt16, 5.5f, 2.f},
     {MouthZones16::UpMolarRightExt16, 7.5f, 4.f},
     {MouthZones16::LoMolarRightExt16, 11.5f, 5.f},
     {MouthZones16::UpIncisorExt16, 16.5f, 3.f},
     {MouthZones16::UpMolarLeftInt16, 19.5f, 2.f},
     {MouthZones16::LoIncisorExt16, 21.5f, 4.f},
     {MouthZones16::UpMolarRightExt16, 25.5f, 3.5f}},
    // Expected Outputs
    {{MouthZones16::LoMolarLeftInt16, 0.25f},
     {MouthZones16::UpIncisorInt16, 0.3f},
     {MouthZones16::LoIncisorExt16, 0.6f},
     {MouthZones16::UpMolarRightExt16, 0.75f},
     {MouthZones16::LoMolarRightExt16, 0.5f},
     {MouthZones16::UpIncisorExt16, 0.3f},
     {MouthZones16::UpMolarLeftInt16, 0.2f}}};

template <typename EnumType> void runTest(const TestData<EnumType> &testData) {
  CheckupComputer computer;
  Brushing<EnumType> brushing;

  for (const auto &input : testData.inputs) {
    brushing.passes.push_back({input.timestamp, input.duration, input.zone});
  }

  Checkup<EnumType> checkup = computer.computeCheckup(brushing);

  bool allPassed = true;
  for (const auto &expected : testData.outputs) {
    float actual = checkup.coverageForZone(expected.zone);
    if (std::abs(actual - expected.coverage) > 0.001f) {
      std::cout << "Test failed for zone " << enumToString(expected.zone)
                << ". Expected: " << expected.coverage << ", Actual: " << actual
                << std::endl;
      allPassed = false;
    }
  }

  if (allPassed) {
    std::cout << "===========================================" << std::endl;
    std::cout << "All tests passed for " << typeid(EnumType).name()
              << std::endl;
    std::cout << "Test Summary:" << std::endl;
    std::cout << "- Total Zones Tested: " << testData.outputs.size()
              << std::endl;
    std::cout << "- Inputs:" << std::endl;
    for (const auto &input : testData.inputs) {
      std::cout << "  Zone: " << enumToString(input.zone)
                << ", Timestamp: " << input.timestamp
                << ", Duration: " << input.duration << std::endl;
    }
    std::cout << "- Expected Outputs:" << std::endl;
    for (const auto &output : testData.outputs) {
      std::cout << "  Zone: " << enumToString(output.zone)
                << ", Coverage: " << output.coverage << std::endl;
    }
    std::cout << "- All zones have been correctly computed." << std::endl;
    std::cout << "- Coverage values matched expected results with precision "
                 "tolerance of 0.001."
              << std::endl;
    std::cout << "===========================================\n" << std::endl;
  }
}

int main() {
  runTest(testData8);
  runTest(testData12);
  runTest(testData16);

  return 0;
}
