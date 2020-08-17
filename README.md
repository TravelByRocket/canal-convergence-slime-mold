# Canal Convergence Interactive Sculpture
A touch-responsive inflatable sculpture for the 2019 Canal Convergence event in Scottsdale, AZ, November 8-15. A collaboration between artist [Nicole Banowetz](http://www.nicolebanowetz.com) and technologist [Bryan Costanza](http://somuchscience.com/).

[Jump to Documentation](#documentation)

## Arduino IDE Setup
### Libraries to Install
- FastLED

### Boards to Install
- ESP8266 `https://arduino.esp8266.com/stable/package_esp8266com_index.json`
- ESP32 `https://dl.espressif.com/dl/package_esp32_index.json`

### Boards to Select on Upload
- LongSiteLeft, LongSiteRight, ShortSite are "Adafruit HUAZZAH32" or something like that
- Medallion is "Adafruit HUZZAH8266" or something like that
- Each capacitive touch finger sensor board is a WEMOS D1 mini (not Pro or Lite)

### Driver Installation
- I think I needed a USB driver from Adafruit for the boards but this information is escaping me 11 months after the fact

## IP Addresses
IP Addresses are reserved as
- 192.168.0.**100** for LongSiteLeft
- 192.168.0.**101** for LongSiteRight
- 192.168.0.**102** for ShortSite
- 192.168.0.**103** for Medallion

![hardware layout and details](Hardware%20Map.png)

## Test Color Temporarily
The controllers recognize basic commands sent by short UDP strings with strict formatting restrictions. 

### Start a UDP Connection
#### From a Mac
Open Terminal and type `nc -u 192.168.0.xxx 8052` where 
- `nc` is sort of a Terminal program for sending network messages
- `-u` specifies UDP protocol
- `xxx` is specific to each controller (see above)
- `8052` is the port that each controller is listening on. 

#### From an iPhone
Similar functionality as above can be found with these iOS apps
- https://apps.apple.com/us/app/tcp-udp-client/id906830843
- https://apps.apple.com/us/app/udp-terminal/id1399547636

### Change Medallion only
October 20, 2019 -- Only the colors can be changed by UDP message. The format to change the color is:
- `c-----` the `c` prefix is used to indicate a color command
- `-i----` the `i` indicates that changes will apply to the inner/center color
- `-o----` the `o` indicates that changes will apply to the outer/background color
- `--r---` changes the red value of the RGB color, accepts 0-255 (but possibly a bug limiting some values)
- `--g---` same as above but for green
- `--b---` same as above but for blue
- `cib100` specifies a value of 100 for blue component of the inner color, for example
- Note that all commands must be six characters long, so color values less than 100 must be zero-padded, like `050`, for example 
- `000000` resets the board to recover initial settings

### Change LongSiteLeft, LongSiteRight, & ShortSite
- `c-----` the `c` prefix is used to indicate a color command
- `-a----` the `a` indicates that changes will apply to the initial/resting color ("toxic" green in this case)
- `-b----` the `b` same as above but for secondary/activated color (reddish in this case)
- `-c----` the `c` same as above but for tertiary color (turquoise in this case)
- `--r---` changes the red value of the RGB color, accepts 0-255 (but possibly a bug limiting some values)
- `--g---` same as above but for green
- `--b---` same as above but for blue
- `cab100` specifies a value of 100 for blue component of the initial color, for example
- Note that all commands must be six characters long, so color values less than 100 must be zero-padded, like `050`, for example

## Other Notes

The Fingers 1-9 have IP Addresses assigned 110-118, though this is not important as the code is written at the moment except as a confirmation of uploading the correct file with over-the-air updates.

# Documentation

![inflation time lapse](Documentation/inflationtimelapse.mov)

![cap touch working 1](Documentation/cap%20touch%20working%20screenshot%201.png)
![cap touch working 2](Documentation/cap%20touch%20working%20screenshot%202.png)

![cap touch boards](Documentation/CapTouchBoardsOct19/captouchboards1.jpg)
![cap touch boards](Documentation/CapTouchBoardsOct19/captouchboards2.jpg)
![cap touch boards](Documentation/CapTouchBoardsOct19/captouchboards3.jpg)
![cap touch boards](Documentation/CapTouchBoardsOct19/captouchboards4.jpg)
![cap touch boards](Documentation/CapTouchBoardsOct19/captouchboards5%20copy.jpg)
![cap touch boards](Documentation/CapTouchBoardsOct19/captouchboards6.jpg)
![cap touch boards](Documentation/CapTouchBoardsOct19/captouchboards7.jpg)
![cap touch boards](Documentation/CapTouchBoardsOct19/captouchboards8.jpg)
![cap touch boards](Documentation/CapTouchBoardsOct19/captouchboards9.jpg)
![cap touch boards](Documentation/CapTouchBoardsOct19/captouchboards10.jpg)
![cap touch boards](Documentation/CapTouchBoardsOct19/captouchboards11.jpg)
![cap touch boards](Documentation/CapTouchBoardsOct19/captouchboards12.jpg)
![cap touch boards](Documentation/CapTouchBoardsOct19/captouchboards13.jpg)
![cap touch boards](Documentation/CapTouchBoardsOct19/captouchboards14.jpg)
![cap touch boards](Documentation/CapTouchBoardsOct19/captouchboards15.jpg)

## Lessons Learned
- Use Feather version of HUZAAH8266 (multiple button presses for update and reseton simple breakout)
- Plan interaction earlier for streamlined coding approach, including LED mapping and referencing method
- Identify SMEs at project start for checks and technical problem resolution
- Run a design review early (and possibly repeatedly)
- NeoPixel library worked well with ESP8266 but needed FastLED for ESP32
- Store wifi credentials securely in a library
- The HUZZAH8266 seems to not have enough current available on the pins for reliable two-pin capacitive touch sensing; it showed only a few bits' worth of resolution

## Future Development
- Key to LED assignment and basis for future library: offset, direction, and length
- Object-oriented control of fingers and filements
- Improved status reporting for send/recieve messages
- String in and String out for UDP communication to avoid complexities of data types in C++
- Unified code for multiple MCUs (not managing code for every similar instance); at least import a librarylike file that unifies most of the code
- Webpage-based status and controls, including streamlined color testing
- FastLED index checking (complete crash when on `.show()` after setting an invalid index)
- Utilize router broadcast address to simplifiy send/recieve of messages, run automatic setup, and/or avoid complexity of storing multiple IP addresses which forces nearly-identical sketches for multiple boards
- More verbose (and standardized through my own library) output of Wifi connection status (no error was reported when not successfully connecting to Wifi)
- Screw terminals continually came loose so find something that is still easy but more secure (also screwdrivers are so easy to lose)
- Make wiring harnesses faster (e.g., with crimp tools)

## Open Issues
- Capacitive touch interference source and mitigation
- MultiWifi stuck on network login (it worked for weeks and then consistently failed one day on both ESP8226 and ESP32)
- How to compile selectively for ESP8266 and ESP32 (briefly attempted unsuccessfully)l
- Source of compilation differences for ESP8266/32 (e.g. using `SSID.c_str()` and `PASS.c_str()` for only ESP32)
- Generating strings or char arrays on ESP8266 (experienced severe memory access issues)
- Many conversion issues around items like `.c_str()`, `uint8_t`, `char array`, pointers `*`, copies `&`