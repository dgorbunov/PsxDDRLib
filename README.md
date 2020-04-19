# Playstation DDR Arduino Library
- Support for remapping pad functions
- Support for Holds
- Uses Arduino to act as a generic HID device - no drivers required.
- Any Arduino can be configured as HID device.
- Tested on Stepmania

# Setup
Configure your Arduino as a generic HID device as described here: http://mitchtech.net/arduino-usb-hid-keyboard/
Follow the wiring diagram here: https://store.curiousinventor.com/guides/PS2and match up with the pins defined in the program, or choose your own. Bitbang protocol is used here so SPI pins are not required. 

# Reference
HID Table for remapping: https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf
