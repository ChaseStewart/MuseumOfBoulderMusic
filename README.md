# MuseumOfBoulderMusic
Repository to hold work for the music installation for Museum of Boulder Oct 2021

# Author(s)
[Chase E. Stewart](https://chasestewart.co) for [Hidden Layer Design](https://hiddenlayerdesign.com)

# Repository Layout
`/Software/TestFirmware/...` - Basic Teensy firmware that can take input from a superset of all involved sensors and just print the status of these sensors over serial. It also compiles with the TeensyMIDI library and does a sort of minimal no-op involving MIDI (it reads inbound MIDI messages in the loop).

`/Software/Station1/...` - Firmware for the first MIDI station that just does some basic sensor to MIDI conversion.

## Setup Instructions
Obtain and wire up the hardware, then flash the Teensy with provided firmware and connect it via USB to a computer running Ableton Live- you will need to manually configure the teensy as a MIDI input device. More info to be provided later.
