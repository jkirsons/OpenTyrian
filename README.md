#OpenTyrian - ESP32 Port

This is a port of Open Tyrian by Gadget Workbench.

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

(https://www.youtube.com/watch?v=UL5eTUv7SZE)

================================================================================

##Installation

"make menuconfig"

under the section "ESP32-TYRIAN platform-specific configuration, set up the pins used for your LCD and SD Card.

"make flash"
