# komentarz //

MCU=attiny2313a

DUDECPUTYPE=t2313
PROGRAMER=usbasp

LOADCMD=avrdude
LOADARG=-p $(DUDECPUTYPE) -c $(PROGRAMER) -e -U flash:w:main.hex:i

CC=avr-gcc
OBJCOPY=avr-objcopy

CFLAGS=-g -mmcu=$(MCU) -Wall -Wstrict-prototypes -Os -mcall-prologues

hex: map
	$(OBJCOPY) -R .eeprom -O ihex main.out main.hex 

map: build
	$(CC) $(CFLAGS) -o main.out -Wl,-Map,main.map main.o 

build:
	$(CC) $(CFLAGS) -Os -c main.c
	
load: hex
	$(LOADCMD) $(LOADARG) main.hex
	
clean:
	rm  *.o