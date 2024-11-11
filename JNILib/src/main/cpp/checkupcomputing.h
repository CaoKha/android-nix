#ifndef CHECKUPCOMPUTING_H
#define CHECKUPCOMPUTING_H

#include <map>
#include <string>
#include <vector>

namespace kolibree {

// =====================
// CheckupComputer Enums
// =====================
enum class MouthZones8 {
  LoLeftExt8,
  LoLeftInt8,
  LoRightExt8,
  LoRightInt8,
  UpLeftExt8,
  UpLeftInt8,
  UpRightExt8,
  UpRightInt8
};

enum class MouthZones12 {
  LoIncisorExt12,
  LoIncisorInt12,
  LoMolarLeftExt12,
  LoMolarLeftInt12,
  LoMolarRightExt12,
  LoMolarRightInt12,
  UpIncisorExt12,
  UpIncisorInt12,
  UpMolarLeftExt12,
  UpMolarLeftInt12,
  UpMolarRightExt12,
  UpMolarRightInt12
};

enum class MouthZones16 {
  LoIncisorExt16,
  LoIncisorInt16,
  LoMolarLeftExt16,
  LoMolarLeftInt16,
  LoMolarLeftOcc16,
  LoMolarRightExt16,
  LoMolarRightInt16,
  LoMolarRightOcc16,
  UpIncisorExt16,
  UpIncisorInt16,
  UpMolarLeftExt16,
  UpMolarLeftInt16,
  UpMolarLeftOcc16,
  UpMolarRightExt16,
  UpMolarRightInt16,
  UpMolarRightOcc16
};

// =====================
// CheckupComputer Utility Structs
// =====================
template <typename EnumType> struct BrushingPass {
  float timestamp;
  float duration;
  EnumType zone;
};

template <typename EnumType> struct Brushing {
  std::vector<BrushingPass<EnumType>> passes;
};

template <typename EnumType> class Checkup {
public:
  float coverageForZone(EnumType zone) const;
  void setCoverage(EnumType zone, float coverage);

private:
  std::map<EnumType, float> coverages;
};

// =====================
// CheckupComputer Main Class
// =====================

class CheckupComputer {
public:
  float target_duration = 10;

  template <typename EnumType>
  Checkup<EnumType> computeCheckup(const Brushing<EnumType> &brushing);
};

// Helper function to get the number of zones for each enum type
template <typename EnumType> constexpr int getNumberOfZones() {
  if constexpr (std::is_same_v<EnumType, MouthZones8>)
    return 8;
  else if constexpr (std::is_same_v<EnumType, MouthZones12>)
    return 12;
  else if constexpr (std::is_same_v<EnumType, MouthZones16>)
    return 16;
  else
    return 0; // or throw an error
}

// Helper function to convert enum to string
template <typename EnumType> std::string enumToString(EnumType value);

} // namespace kolibree

#endif // CHECKUPCOMPUTING_H
