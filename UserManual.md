
# VipTape  
## Retro Cassette Interface System  
### User Manual – CMS v2a

**Hardware Design:** Josh Bensadon  
**Firmware & Software:** Costas Skordis  

---

## 1. Introduction
VipTape is a modern cassette interface designed for classic retro computers such as the COSMAC ELF, ELF II, and VIP.
It replaces unreliable cassette decks with a solid-state, touchscreen-based solution while preserving original cassette protocols.

VipTape is intended for **retro computer hobbyists and general users**. No programming knowledge is required.

---

## 2. Powering On
When powered on, VipTape performs an internal memory clear and system initialization.
During this time, user input is ignored.

When initialization completes, the main menu is shown.

---

## 3. Main Menu and Protocol Selection
The main menu allows you to:
- Select the target computer protocol
- Browse stored binary files
- Choose between PLAY (load) and RECORD (save)

Always verify the protocol shown at the top of the screen matches your computer.

---

## 4. Loading a Program (PLAY)
1. Select the correct protocol (e.g. ELF II)
2. Choose a file from the list
3. Press **Play**
4. Put your retro computer into LOAD mode
5. Confirm with **Yes**

VipTape will begin transferring data immediately.

---

## 5. Transfer in Progress
While transferring, VipTape displays a “Transferring file – Please wait” screen.
Do not touch the screen or disconnect cables during this process.

---

## 6. Saving a Program (RECORD)
1. Select **Record**
2. Enter a file name using the on-screen keyboard, or press OK to auto-generate one
3. Confirm the save
4. Put your retro computer into SAVE mode
5. Press **Yes**

Incoming audio is recorded and stored as a binary file.

---

## 7. File Naming
If no name is entered, VipTape automatically creates a unique filename using the format:

FILE0000.BIN, FILE0001.BIN, etc.

This prevents accidental overwrites.

---

## 8. Screen Saver
After approximately 5 minutes of inactivity, the screen saver appears.
Touch anywhere on the screen to resume.

---

## 9. Tips and Troubleshooting
- Always confirm the protocol before loading or saving
- If loading fails, retry with lower audio volume
- Use short, good-quality audio cables
- If unsure, press **Reset** and start again

---

## 10. Revision History
**v1.0** – Initial public release
