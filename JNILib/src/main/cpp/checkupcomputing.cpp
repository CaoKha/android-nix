#include "checkupcomputing.h"
#include <algorithm>

namespace kolibree {

// =====================
// Function definitions for Checkup Class
// =====================
template <typename EnumType>
float Checkup<EnumType>::coverageForZone(EnumType zone) const {
  auto it = coverages.find(zone);
  return (it != coverages.end()) ? it->second : 0.0f;
}

template <typename EnumType>
void Checkup<EnumType>::setCoverage(EnumType zone, float coverage) {
  coverages[zone] = std::min(coverage, 1.0f);
}

// =====================
// Function definitions for CheckupComputer Class
// =====================
template <typename EnumType>
Checkup<EnumType>
CheckupComputer::computeCheckup(const Brushing<EnumType> &brushing) {
  Checkup<EnumType> checkup;
  std::map<EnumType, float> zoneDurations;

  for (const auto &pass : brushing.passes) {
    zoneDurations[pass.zone] += pass.duration;
  }

  for (int i = 0; i < getNumberOfZones<EnumType>(); ++i) {
    EnumType zone = static_cast<EnumType>(i);
    float duration = zoneDurations[zone];
    float coverage = duration / target_duration;
    checkup.setCoverage(zone, coverage);
  }

  return checkup;
}

// =====================
// Template instantiations
// =====================
template class Checkup<MouthZones8>;
template class Checkup<MouthZones12>;
template class Checkup<MouthZones16>;

template Checkup<MouthZones8>
CheckupComputer::computeCheckup(const Brushing<MouthZones8> &);
template Checkup<MouthZones12>
CheckupComputer::computeCheckup(const Brushing<MouthZones12> &);
template Checkup<MouthZones16>
CheckupComputer::computeCheckup(const Brushing<MouthZones16> &);

// Implement enumToString for each enum type
template <> std::string enumToString<MouthZones8>(MouthZones8 value) {
  static const char *names[] = {"LoLeftExt8",  "LoLeftInt8", "LoRightExt8",
                                "LoRightInt8", "UpLeftExt8", "UpLeftInt8",
                                "UpRightExt8", "UpRightInt8"};
  return names[static_cast<int>(value)];
}

template <> std::string enumToString<MouthZones12>(MouthZones12 value) {
  static const char *names[] = {
      "LoIncisorExt12",   "LoIncisorInt12",    "LoMolarLeftExt12",
      "LoMolarLeftInt12", "LoMolarRightExt12", "LoMolarRightInt12",
      "UpIncisorExt12",   "UpIncisorInt12",    "UpMolarLeftExt12",
      "UpMolarLeftInt12", "UpMolarRightExt12", "UpMolarRightInt12"};
  return names[static_cast<int>(value)];
}

template <> std::string enumToString<MouthZones16>(MouthZones16 value) {
  static const char *names[] = {
      "LoIncisorExt16",    "LoIncisorInt16",    "LoMolarLeftExt16",
      "LoMolarLeftInt16",  "LoMolarLeftOcc16",  "LoMolarRightExt16",
      "LoMolarRightInt16", "LoMolarRightOcc16", "UpIncisorExt16",
      "UpIncisorInt16",    "UpMolarLeftExt16",  "UpMolarLeftInt16",
      "UpMolarLeftOcc16",  "UpMolarRightExt16", "UpMolarRightInt16",
      "UpMolarRightOcc16"};
  return names[static_cast<int>(value)];
}

} // namespace kolibree
