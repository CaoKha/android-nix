package com.kolibree;

import android.content.Context;
import androidx.test.ext.junit.runners.AndroidJUnit4;
import java.util.HashMap;
import org.junit.Assert;
import org.junit.Test;
import org.junit.runner.RunWith;

/**
 * Instrumented test, which will execute on an Android device.
 *
 * This test checks the computeCheckup8, computeCheckup12, and computeCheckup16
 * JNI functions in a similar way to the Linux-based unit tests.
 */
@RunWith(AndroidJUnit4.class)
public class InstrumentedTest {
  // static {
  //   System.loadLibrary("native-lib");
  // }

  private final CheckupComputer checkupComputer = new CheckupComputer();

  @Test
  public void testComputeCheckup8() {
    Brushing8 brushing = new Brushing8();

    brushing.addPass(new BrushingPass8(0.0f, 8.0f, MouthZones8.LoLeftExt8));
    brushing.addPass(new BrushingPass8(8.0f, 6.0f, MouthZones8.UpLeftInt8));
    brushing.addPass(new BrushingPass8(14.0f, 7.0f, MouthZones8.UpRightInt8));
    brushing.addPass(new BrushingPass8(21.0f, 3.0f, MouthZones8.UpLeftInt8));

    HashMap<MouthZones8, Float> checkup = checkupComputer.computeCheckup8(brushing);

    HashMap<MouthZones8, Float> expected = new HashMap<>();
    expected.put(MouthZones8.LoLeftExt8, 0.8f);
    expected.put(MouthZones8.UpLeftInt8, 0.9f);
    expected.put(MouthZones8.UpRightInt8, 0.7f);

    for (MouthZones8 zone : MouthZones8.values()) {
      Float actualCoverage = checkup.getOrDefault(zone, 0.0f);
      Float expectedCoverage = expected.getOrDefault(zone, 0.0f);
      Assert.assertEquals("Coverage for " + zone.name(), expectedCoverage, actualCoverage, 0.1f);
    }
  }

  @Test
  public void testComputeCheckup12() {
    Brushing12 brushing = new Brushing12();

    brushing.addPass(new BrushingPass12(0.0f, 10.0f, MouthZones12.LoIncisorExt12));
    brushing.addPass(new BrushingPass12(10.0f, 6.0f, MouthZones12.UpIncisorInt12));
    brushing.addPass(new BrushingPass12(16.0f, 5.0f, MouthZones12.UpMolarRightExt12));
    brushing.addPass(new BrushingPass12(21.0f, 5.0f, MouthZones12.LoMolarRightExt12));
    brushing.addPass(new BrushingPass12(26.0f, 5.5f, MouthZones12.UpIncisorInt12));
    brushing.addPass(new BrushingPass12(31.5f, 2.2f, MouthZones12.UpMolarRightExt12));

    HashMap<MouthZones12, Float> checkup = checkupComputer.computeCheckup12(brushing);

    HashMap<MouthZones12, Float> expected = new HashMap<>();
    expected.put(MouthZones12.LoIncisorExt12, 1.0f);
    expected.put(MouthZones12.UpIncisorInt12, 1.0f);
    expected.put(MouthZones12.UpMolarRightExt12, 0.72f);
    expected.put(MouthZones12.LoMolarRightExt12, 0.5f);

    for (MouthZones12 zone : MouthZones12.values()) {
      Float actualCoverage = checkup.getOrDefault(zone, 0.0f);
      Float expectedCoverage = expected.getOrDefault(zone, 0.0f);
      Assert.assertEquals("Coverage for " + zone.name(), expectedCoverage, actualCoverage, 0.1f);
    }
  }

  @Test
  public void testComputeCheckup16() {
    Brushing16 brushing = new Brushing16();

    brushing.addPass(new BrushingPass16(0.0f, 12.0f, MouthZones16.LoIncisorExt16));
    brushing.addPass(new BrushingPass16(12.0f, 4.0f, MouthZones16.UpIncisorInt16));
    brushing.addPass(new BrushingPass16(16.0f, 4.0f, MouthZones16.UpMolarRightInt16));
    brushing.addPass(new BrushingPass16(20.0f, 0.0f, MouthZones16.LoMolarRightOcc16));

    HashMap<MouthZones16, Float> checkup = checkupComputer.computeCheckup16(brushing);

    HashMap<MouthZones16, Float> expected = new HashMap<>();
    expected.put(MouthZones16.LoIncisorExt16, 1.0f);
    expected.put(MouthZones16.UpIncisorInt16, 0.4f);
    expected.put(MouthZones16.UpMolarRightInt16, 0.4f);
    expected.put(MouthZones16.LoMolarRightOcc16, 0.0f);

    for (MouthZones16 zone : MouthZones16.values()) {
      Float actualCoverage = checkup.getOrDefault(zone, 0.0f);
      Float expectedCoverage = expected.getOrDefault(zone, 0.0f);
      Assert.assertEquals("Coverage for " + zone.name(), expectedCoverage, actualCoverage, 0.1f);
    }
  }
}
