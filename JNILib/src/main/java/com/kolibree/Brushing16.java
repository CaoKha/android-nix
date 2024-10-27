package com.kolibree;

import java.util.ArrayList;
import java.util.List;

public class Brushing16 {
    public List<BrushingPass16> passes;
    public int numZones;

    // Constructor requiring an int parameter
    public Brushing16(int numZones) {
        this.numZones = numZones;
        this.passes = new ArrayList<>();
    }

    public void addPass(BrushingPass16 pass) {
        this.passes.add(pass);
    }
}