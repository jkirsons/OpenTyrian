# OpenTyrian - ESP32 Port

This is a port of Open Tyrian by Gadget Workbench.

It's based on the OpenTyrian port (https://bitbucket.org/opentyrian/opentyrian/wiki/Home)

It requires:
 - An ESP32 WROVER 
 - An ILI9341 LCD - connected by SPI
 - SD Card with the tyrian files in a "data" folder
 - Amplifier and speaker
 - Some input buttons (Up/Down/Left/Right + two buttons)

================================================================================

OpenTyrian is an open-source port of the DOS game Tyrian.

Tyrian is an arcade-style vertical scrolling shooter.  The story is set
in 20,031 where you play as Trent Hawkins, a skilled fighter-pilot employed
to fight MicroSol and save the galaxy.

Tyrian features a story mode, one- and two-player arcade modes, and networked
multiplayer.

================================================================================

[![Alt text](https://img.youtube.com/vi/UL5eTUv7SZE/0.jpg)](https://www.youtube.com/watch?v=UL5eTUv7SZE)

================================================================================

## Installation

"make menuconfig"

under the section "ESP32-TYRIAN platform-specific configuration:
![config_image](https://github.com/jkirsons/OpenTyrian/raw/master/documents/config_1.png)

### LCD / SD Card
Set up the pins used for your LCD and SD Card.  This is the default configuration:
![config_image](https://github.com/jkirsons/OpenTyrian/raw/master/documents/config_2.png)

The LCD is connected to VSPI, and the SD Card to HSPI.

### Input / Controls
The default button input is configured in keyboard.c
- GPIO36 UP
- GPIO34 DOWN
- GPIO32 LEFT
- GPIO39 RIGHT
- GPIO33 ESCAPE (quit)
- GPIO35 SPACE (fire/select)

### Sound
Sound is output using I2S connected to the DAC on GPIO25 this is configured in SDL_audio.c.  You will need an amplifier+speaker and coupling capacitor on this pin (the capacitor to bring the DAC's DC signal back to AC).

### Compiling
"make flash"
