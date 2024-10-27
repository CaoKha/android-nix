# Progress Status

[My current progress](./docs/README.md)

# Kolibree C++ / JNI test

Your task is to write a processing API that takes a raw brushing session and computes a checkup for that session.

## How a brushing works

###### The user will brush their teeth and at the end of the brushing, the whole brushing data is sent at once to the app

- Users will brush sequentially different zones of their mouths, however they can brush again a zone they had already brushed.
- The toothbrush records the brushing as an array of “brushing passes”. A brushing pass records a duration, and the zone brushed. Every time the user changes zones, a new brushing pass is created.
- After the brushing is done, the whole sequence of brushing passes is transmitted to the Android app, which then sends this data for processing to your code.
- The mouth is split whether into 8, 12, or 16 zones depending on the settings chosen for the brushing. This setting is set before the brushing starts and doesn't change during the session.

###### Those 3 enums are already defined for you in the Java sources

`public enum MouthZones8`
`public enum MouthZones12`
`public enum MouthZones16`

## The checkup

For each brushing a user performs with our connected toothbrush, a checkup is
computed.

A checkup tells the user if they spent enough time brushing their mouth by
giving each zone in the mouth a score between 0% and 100%.
A score of 0% means the user didn't brush the zone at all while 100% means
that the user brushed for at least as long as the target duration in that zone
(all zones have the same target duration).
For instance, assuming a target duration per zone of 10s and that the mouth is
split into 5 zones, if the user brushed a zone for 4 seconds in total, they
would get a score of 40% in that zone. If they spent 12s, they would get a
score of 100%.

Brushings themselves are represented as a collection of passes.
When the user starts brushing, a new pass is created. Every time the user
starts brushing in a different zone, another pass is created. This means that
a user could be brushing zone A for 3s, then move to zone B for 4s, and
finally come back to zone A for 2s.
This would produce three passes: (A,3s), (B,4s), (A,2s).

It contains an entry for each zone for a given zone encoding but not more (an 8 zones brushing should allow querying for all 8 zones, and only those, even if some were not brushed during the session).

To compute the checkup from a collection of passes, the total time spent in
each zone is computed, then the score per zone is computed using the total
time and the target duration.

## Example

If the input and expected output data for an 8-zones brushing were to be represented as JSON, this is what they could look like:

_Input_

```
{
  "passes":
  [
    {
      "mouthzone":"LoLeftExt8",
      "duration":8
    },
    {
      "mouthzone":"UpLeftInt8",
      "duration":6
    },
    {
      "mouthzone":"UpRightInt8",
      "duration":7
    },
    {
      "mouthzone":"UpLeftInt8",
      "duration":3
    }
  ]
}
```

_Output_

```
{
  "LoLeftExt8": 80,
  "LoLeftInt8": 0,
  "LoRightExt8": 0,
  "LoRightInt8": 0,
  "UpLeftExt8": 0,
  "UpLeftInt8": 90,
  "UpRightExt8": 0,
  "UpRightInt8": 70
}
```

## Requirements

- The Android app should only have to call Java code to access your API.
- The processing of the data is done in C++, and the checkup is directly returned to the Android app as a Java object.
- We need to be able to process brushings encoded for 8, 12 or 16 mouth zones.
- The Java app encodes the raw brushing data with a different enum for each number of zones (`MouthZones8`, `MouthZones12` and `MouthZones16`).
- Each brushing is encoded with a predefined amount of zones that is not going to change during a brushing.

## Advice

- The code is given as sample, meaning that you are free to modify it to adapt with your design and assumptions. Just be sure to explain and document which assumptions you have made.
