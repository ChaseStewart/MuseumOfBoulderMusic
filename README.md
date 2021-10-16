# MuseumOfBoulderMusic
Repository to hold work for the music installation for Museum of Boulder Oct 2021

# Author(s)
[Chase E. Stewart](https://chasestewart.co) for [Hidden Layer Design](https://hiddenlayerdesign.com)

# Repository Layout
`/Hardware/PercussionStation/...` - Hardware layout diagrams for the percussion station. *WARNING: These diagrams are out of date, currently NOT RELIABLE.*

`/Software/PercussionStation/...` - Firmware for the PercussionStations and MelodicStations that use a USB joystick, 6 buttons, an ultrasonic Rangefinder, and a PIR sensor to control percussive and tonal effects. *NOTE: Currently Percussion Firmware is used for BOTH Percussion AND Melodic stations*

`/Software/SweepStation/...` - Firmware for the SweepStations, which use 2 ultrasonic rangefinders, 2 buttons, and a PIR sensor to do transitional effects.

`/Software/ProvisionEEPROM/...` - Firmware to run one time on each Teensy 4.1 to provision the EEPROM in the chip. This mostly just sets the MIDI Channel for the Teensy, and then prints what it's got in the EEPROM.

`/Software/TestFirmware/...` - *WARNING: This is deprecated!* Basic Teensy firmware that can take input from a superset of all involved sensors and just print the status of these sensors over serial. It also compiles with the TeensyMIDI library and does a sort of minimal no-op involving MIDI (it reads inbound MIDI messages in the loop).

`/Software/TestStation/...` - *WARNING: This is deprecated!* An early draft of the PercussionStation- this sends MIDI control codes for a subset of sensors. 



## Setup Instructions
Obtain and wire up the hardware, then flash the Teensy with one of the provided firmware options and connect it via USB to a computer running Ableton Live- you will need to manually configure the teensy as a MIDI input device. More info to be provided later.
