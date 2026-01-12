# VIPTape

This project was created by Josh Bensadon that allows you to load and save code via the cassette port to and from the Cosmac VIP.

Version 1.1 only does VIP computers.

Version 2 and up, re-written to accomodate ELF II, HUG 1802/ETI-660/HEC-1802 and VIP computers.

## Hardware modifications for version 2:

1. Enhance the LM358 op amp circuit to utilize the first op amp as a low pass filtering amp for >2500hz frequencies and second op amp as a comparator to deliver nicely formed square wares that are 3.5 v p/p.

2. The input pin on the Mega was moved from A8 to D18 to allow correct ISR to work.

![VIPTape v2a Splash](https://github.com/cskordis/VIPTape/blob/main/Images/TitleScreen.JPG)

![VIPTape v2a](https://github.com/cskordis/VIPTape/blob/main/Images/Directory%20List.JPG)

![VIPTape v2a Keyboard Entry](https://github.com/cskordis/VIPTape/blob/main/Images/KeyboardEntryFor%20SaveFile.JPG)





