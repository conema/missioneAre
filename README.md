# missioneAre
_A simple university project with Ardunio_

## How it works
<p align="center"><img src="https://user-images.githubusercontent.com/12801153/27876700-4602fc6c-61b8-11e7-989f-6816b7a55475.png" width="250"></p>

This project is created with the idea of demonstrating the importance of software, with relatively few components it was possible to create simple game that resemble to olds 80's games. With a logic similar to that used in this project you can recreate any game that can be represented on an LCD screen, from Pac-Man to Donkey Kong. The real limitation is only the size of the screen and therefore the game world.

The operation principle of the game is conceptually simple, our shuttle, in the central position, will have to try to defend itself from attacking alien ships, which will come in a variable amount and with incremental speed based on the level reached. This level is increased to 15, 25 and 35 kills. The total number of dropped ships is displayed and updated in the right corner of the screen. Our shuttle has two commands, one to turn and shoot to the left and one to do the opposite action. The player can re-start the game when his ship is destroyed or when he end the game. In these cases, the final score is shown, which is calculated based on kills, distance traveled, and missiles fired. So the user who uses less possible missiles is rewarded.
 
## How it is made
<p align="center"><img src="https://user-images.githubusercontent.com/12801153/27876139-3f7ab12a-61b6-11e7-8f26-75949fa93961.jpg" width="250"></p>
The assembly is very simple, the LCD screen has been linked like it is standard to do, the steps are explained on the <a href="https://www.arduino.cc/en/Tutorial/HelloWorld">Arduino site</a>. A 220ohm resistor was inserted between the third LCD pin and ground to maintain constant the contrast. The pushbuttons are instead connected to the digital inputs to control when they are pressed. You can recreate the project by copying the upper photo.

## Main functions in the code

- drawLCD: in this function the sprites of our game are declared. The representation is in binary, if the position contains a '1', the led identified by the row and column, will turn on, vice versa will be off. subsequently Sprites are moved to the LCD's RAM and the screen is cleared. See [this project](https://omerk.github.io/lcdchargen/) for some examples.

- drawShuttle: this fuction updates the sprite to use, according to the direction. It does very important control, that is the one on the collision between an object and our shuttle, a key element to understand if you have been knocked down. It also takes care of printing the number of numbers and the number of kills.

- shuttleFire: It manages the missiles of our shuttle, based on the direction of the ship, the missile is shot in the right direction.

