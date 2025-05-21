TARGET = matrix

MCU = atmega328p
F_CPU = 32000000UL
PROGRAMMER = arduino
AVRDUDE_PORT = /dev/ttyUSB0

CC = avr-gcc
OBJCOPY = avr-objcopy
CFLAGS = -std=c11 -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall

SRC = $(TARGET).c
OBJ = $(TARGET).o
HEX = $(TARGET).hex

all: $(HEX)

$(HEX): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET).elf $(SRC)
	$(OBJCOPY) -O ihex -R .eeprom $(TARGET).elf $(HEX)

flash: $(HEX)
	avrdude -c $(PROGRAMMER) -p $(MCU) -P $(AVRDUDE_PORT) -U flash:w:$(HEX):i

clean:
	rm -f *.o *.elf *.hex


