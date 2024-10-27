package com.kolibree;

import java.util.ArrayList;
import java.util.List;

public class Brushing8 {
    public List<BrushingPass8> passes;
    public int numZones;

    // Constructor requiring an int parameter
    public Brushing8() {
        this.numZones = 8;
        this.passes = new ArrayList<>();
    }

    public void addPass(BrushingPass8 pass) {
        this.passes.add(pass);
    }
}
