package com.kolibree;

import java.util.HashMap;
import org.junit.Assert;
import org.junit.Test;

/**
 * CheckupComputerUnitTest demonstrates how to test the JNI functions computeCheckup8,
 * computeCheckup12, and computeCheckup16 using JUnit 4.13.2.
 */
public class CheckupComputerUnitTest {
  // Instantiate the NativeLib class to access native methods
  private final CheckupComputer checkupComputer = new CheckupComputer();

  /**
   * Test the computeCheckup8 JNI function with 8 zones.
   */
  @Test
  public void testComputeCheckup8() {
    // Create a Brushing8 object with 8 zones
    Brushing8 brushing = new Brushing8(8);

    // Add BrushingPass8 instances
    brushing.addPass(new BrushingPass8(0.0f, 8.0f, MouthZones8.LoLeftExt8));
    brushing.addPass(new BrushingPass8(8.0f, 6.0f, MouthZones8.UpLeftInt8));
    brushing.addPass(new BrushingPass8(14.0f, 7.0f, MouthZones8.UpRightInt8));
    brushing.addPass(new BrushingPass8(21.0f, 3.0f, MouthZones8.UpLeftInt8));

    // Call the native method
    HashMap<MouthZones8, Float> checkup = checkupComputer.computeCheckup8(brushing);

    // Expected results (assuming target_duration = 10.0f in C++)
    HashMap<MouthZones8, Float> expected = new HashMap<>();
    expected.put(MouthZones8.LoLeftExt8, 0.8f); // 8 / 10
    expected.put(MouthZones8.UpLeftInt8, 0.9f); // (6 + 3) / 10
    expected.put(MouthZones8.UpRightInt8, 0.7f); // 7 / 10
    // Other zones should have 0 coverage

    // Validate the results
    for (MouthZones8 zone : MouthZones8.values()) {
      Float actualCoverage = checkup.getOrDefault(zone, 0.0f);
      Float expectedCoverage = expected.getOrDefault(zone, 0.0f);
      Assert.assertEquals("Coverage for " + zone.name(), expectedCoverage, actualCoverage, 0.1f);
    }
  }

  /**
   * Test the computeCheckup12 JNI function with 12 zones.
   */
  @Test
  public void testComputeCheckup12() {
    // Create a Brushing12 object with 12 zones
    Brushing12 brushing = new Brushing12(12);

    // Add BrushingPass12 instances
    brushing.addPass(new BrushingPass12(0.0f, 10.0f, MouthZones12.LoIncisorExt12));
    brushing.addPass(new BrushingPass12(10.0f, 6.0f, MouthZones12.UpIncisorInt12));
    brushing.addPass(new BrushingPass12(16.0f, 5.0f, MouthZones12.UpMolarRightExt12));
    brushing.addPass(new BrushingPass12(21.0f, 5.0f, MouthZones12.LoMolarRightExt12));
    brushing.addPass(new BrushingPass12(26.0f, 5.5f, MouthZones12.UpIncisorInt12));
    brushing.addPass(new BrushingPass12(31.5f, 2.2f, MouthZones12.UpMolarRightExt12));

    // Call the native method
    HashMap<MouthZones12, Float> checkup = checkupComputer.computeCheckup12(brushing);

    // Expected results (assuming target_duration = 10.0f in C++)
    HashMap<MouthZones12, Float> expected = new HashMap<>();
    expected.put(MouthZones12.LoIncisorExt12, 1.0f); // 10 / 10
    expected.put(MouthZones12.UpIncisorInt12, 1.0f); // min((6 + 5.5) / 10, 1)
    expected.put(MouthZones12.UpMolarRightExt12, 0.72f); // (5 + 2.2) / 10
    expected.put(MouthZones12.LoMolarRightExt12, 0.5f); // 5 / 10
    // Other zones should have 0 coverage

    // Validate the results
    for (MouthZones12 zone : MouthZones12.values()) {
      Float actualCoverage = checkup.getOrDefault(zone, 0.0f);
      Float expectedCoverage = expected.getOrDefault(zone, 0.0f);
      Assert.assertEquals("Coverage for " + zone.name(), expectedCoverage, actualCoverage, 0.1f);
    }
  }

  /**
   * Test the computeCheckup16 JNI function with 16 zones.
   */
  @Test
  public void testComputeCheckup16() {
    // Create a Brushing16 object with 16 zones
    Brushing16 brushing = new Brushing16(16);

    // Add BrushingPass16 instances
    brushing.addPass(new BrushingPass16(0.0f, 12.0f, MouthZones16.LoIncisorExt16));
    brushing.addPass(new BrushingPass16(12.0f, 4.0f, MouthZones16.UpIncisorInt16));
    brushing.addPass(new BrushingPass16(16.0f, 4.0f, MouthZones16.UpMolarRightInt16));
    brushing.addPass(new BrushingPass16(20.0f, 0.0f, MouthZones16.LoMolarRightOcc16));

    // Call the native method
    HashMap<MouthZones16, Float> checkup = checkupComputer.computeCheckup16(brushing);

    // Expected results (assuming target_duration = 10.0f in C++)
    HashMap<MouthZones16, Float> expected = new HashMap<>();
    expected.put(MouthZones16.LoIncisorExt16, 1.0f); // min((12 / 10), 1)
    expected.put(MouthZones16.UpIncisorInt16, 0.4f); // 4 / 10
    expected.put(MouthZones16.UpMolarRightInt16, 0.4f); // 4 / 10
    expected.put(MouthZones16.LoMolarRightOcc16, 0.0f); // 0 / 10
    // Other zones should have 0 coverage

    // Validate the results
    for (MouthZones16 zone : MouthZones16.values()) {
      Float actualCoverage = checkup.getOrDefault(zone, 0.0f);
      Float expectedCoverage = expected.getOrDefault(zone, 0.0f);
      Assert.assertEquals("Coverage for " + zone.name(), expectedCoverage, actualCoverage, 0.1f);
    }
  }
}
