# Playstation DDR Arduino Library
- Perfect for Stepmania with no key mapping required
- Support for remapping pad functions
- Support for up to 6 key holds at a time with two pads
- Uses any model of Arduino to act as a generic HID device - no drivers required
- Low Latency


# Setup
Configure your Arduino as a generic HID device as described here: http://mitchtech.net/arduino-usb-hid-keyboard/
Follow the wiring diagram here: https://store.curiousinventor.com/guides/PS2 and match up with the pins defined in the program, or choose your own. Bitbang protocol is used here so SPI pins are not required. 
## My Converter Box
![My All in One Converter Box](https://github.com/dgorbunov/PsxDDRLib/blob/master/IMG_2372%20(1).JPG)

# Reference
HID Table for remapping: https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf

# TODO:
- Add general analog support + PS Controller Support
- Test on Nano
- Change Keymappings to not override keyboard?
- Improve pad release latency
- Pull request to Psx Lib with HID support.
