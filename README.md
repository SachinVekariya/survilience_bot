# Survilience_Bot

We developing a survilence bot for survilience local area by live video streaming.
And as apart of this project, we are using 2 communication protocol UART and SPI.

# UART_Communication

UART communication is used if we use wireless PS2 controller.
Here GccApplication 3 containing UART code for ATmega128. 

# SPI_Protocol

SPI is used to confrigure ps2 controller with ATmega128 and then for receiving data for ps2.

Here we connect two 12v motors with PORTA using Cytron motor driver. We are using differntial drive for drive bot.
