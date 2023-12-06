# Battleship
Battleship is a two-player game where participants strategically place five different ship types onto a 10x10 board. The objective is to sink the opponent's ships before they can do the same to you.

## Getting Started

To play Battleship, you'll need the following:

- 2 working STM32f3Discovery boards
- 4 wires for the required UART communication between boards
- A host PC for each player, from which they will provide input via the command line
- Each player will receive a `chat.py` file for communication with their respective boards

## Setup

1. **Board Connection**: Connect the two STM32f3Discovery boards using the 4 wires for UART communication.

2. **User Input**: Both boards require a host PC for receiving user input via the command line. Utilize the provided `chat.py` file for communication with the boards.

3. **Game Start**: Press the blue button on both boards simultaneously to initiate the game. This action allows the boards to establish communication.

## How to Play

1. Each player strategically places their five ship types on their 10x10 board.
2. The game begins with both players trying to sink each other's ships.
3. Communicate with your board using the provided `chat.py` file to make strategic moves.
4. The first player to sink all of their opponent's ships wins the game.

Enjoy playing Battleship with this interactive setup!

