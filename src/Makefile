DEVICE     = atmega328p
CLOCK      = 2000000
BAUD       = 9600
# using dw-link
PROGRAMMER = -c stk500v1 -P /dev/ttyACM0
# using avrispmkII
# PROGRAMMER = -c avrispmkII -P usb

OBJECTS = uart.o spi.o flash.o i2c.o main.o

# reset enabled
# debug wire disabled (DON'T PROGRAM THE DWEN MANUALLY! avr-gdb does that for you)
# SPI programming enabled
# watchdog timer off
# reset eeprom
# maximum boot size
# boot reset vector disabled

# divide clock by 8
# don't output clock
# use maximum start-up time
# use external 16MHz Oscillator
# -> 2MHz Clock
FUSES      = -U hfuse:w:0xd9:m -U lfuse:w:0x6f:m
AVRDUDE    = avrdude $(PROGRAMMER) -p $(DEVICE)
COMPILE    = avr-gcc -Wall -g -Og -DF_CPU=$(CLOCK) -DBAUD=$(BAUD) -mmcu=$(DEVICE)

.PHONY: all install flash fuse clean

all: main.hex

install: fuse flash

flash: all
	$(AVRDUDE) -U flash:w:main.hex:i

fuse:
	$(AVRDUDE) $(FUSES)

# TODO: do extra step with object files
main.elf: $(OBJECTS)
	$(COMPILE) -o main.elf $(OBJECTS)

$(OBJECTS): %.o: %.c
	$(COMPILE) -c $@ $<

main.hex: main.elf
	rm -f main.hex
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	avr-size --format=avr --mcu=DEVICE main.elf

clean:
	rm -f *.o main.hex main.elf
