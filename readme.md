# Tetris Game on RGB LED Matrix

This Arduino sketch implements a Tetris game on an RGB LED Matrix. The game features basic Tetris gameplay, including block falling, rotation, and line clearing, with an interactive menu for restarting or quitting the game.

There're two versions of the game: you can either play it with Arduino uno and a potentiometer, or with Arduino mega and a joystick!

## Hardware Requirements

- RGB LED Matrix Panel
- Arduino (compatible with Adafruit_GFX and RGBmatrixPanel libraries, here we use Arduino uno and mega)
- Potentiometer/Joystick (for controlling block position)
- Press Button (for rotating blocks and menu selection)
- Breadboard
- Connecting wires

## Wiring Guide

- **Matrix Pins:** Connect A, B, C, CLK, LAT, and OE to corresponding pins on Arduino
- **Potentiometer:** Connect the output to pin 5
- **Button:** Connect one side to pin 10 and the other to ground

## Software Dependencies

- `Adafruit_GFX.h`
- `RGBmatrixPanel.h`
- `gamma.h` (included in the sketch)

## Installation

1. Install the Adafruit_GFX and RGBmatrixPanel libraries via the Arduino Library Manager.
2. Load the provided sketch onto your Arduino.

## Gameplay

- Use the potentiometer/joystick to move the falling block left or right, with a joystick, pull it down to increase the falling speed of each block.
- Press the button to rotate the block.
- The game ends when blocks reach the top of the matrix.
- After a game over, use the potentiometer/joystick and button to choose between restarting or quitting the game.

## Functions Overview

### Main Functions

- `setup()`: Initializes the game and hardware.
- `loop()`: Main game loop, handles game updates.

### Game Class

- Handles game logic, block falling, collision detection, and line clearing.
- Manages game states like playing, game over, and menu selection.

### Block Class

- Represents a Tetris block with methods for drawing, moving, and rotating.

### Utility Functions

- `game_over()`: Displays the game over screen.
- `game_start()`: Displays the game start screen.
- `print_restart()`, `choose_restart()`: Manage Restart option in the menu.
- `print_quit()`, `choose_quit()`: Manage Quit option in the menu.

## Customization

- Modify block colors or add new shapes by editing the `Block` class.
- Adjust game difficulty by changing the falling speed in the `Game` class.

## Troubleshooting

- Ensure all library dependencies are correctly installed.
- Check the wiring if the matrix does not light up or behaves unexpectedly.

Enjoy playing Tetris on your custom-built RGB LED Matrix!
