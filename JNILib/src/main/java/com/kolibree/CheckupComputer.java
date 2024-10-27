package com.kolibree;

import java.util.HashMap;

public class CheckupComputer {
  // Load the native library
  static {
    System.loadLibrary("native-lib"); // This loads libnative-lib.so
  }

  // Declare the native methods specific to each mouth zone configuration
  public native HashMap<MouthZones8, Float> computeCheckup8(Brushing8 brushing);
  public native HashMap<MouthZones12, Float> computeCheckup12(Brushing12 brushing);
  public native HashMap<MouthZones16, Float> computeCheckup16(Brushing16 brushing);
}
