package com.kolibree;

import java.util.ArrayList;
import java.util.List;

public class Brushing12 {
    public List<BrushingPass12> passes;
    public int numZones;

    // Constructor requiring an int parameter
    public Brushing12() {
        this.numZones = 12;
        this.passes = new ArrayList<>();
    }

    public void addPass(BrushingPass12 pass) {
        this.passes.add(pass);
    }
}
