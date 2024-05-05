DEVICE                 = atmega328p
CLOCK                  = 2000000
BAUD                   = 9600
MOLD_DETECT_VERSION    = \"0.1.0\"
# using dw-link
PROGRAMMER = -c stk500v1 -P /dev/ttyACM0
# using avrispmkII
# PROGRAMMER = -c avrispmkII -P usb

OBJECTS = uart.o spi.o flash.o i2c.o error.o led.o terminal.o interrupts.o measure.o main.o

# FUSES #
# reset enabled
# debug wire disabled (DON'T PROGRAM THE DWEN MANUALLY! avr-gdb does that for you)
# SPI programming enabled
# watchdog timer off
# don't reset eeprom on programming
# maximum boot size
# boot reset vector disabled

# divide clock by 8
# don't output clock
# use maximum start-up time
# use external 16MHz Oscillator
# -> 2MHz Clock
FUSES      = -U hfuse:w:0xd1:m -U lfuse:w:0x6f:m
AVRDUDE    = avrdude $(PROGRAMMER) -p $(DEVICE)
COMPILE    = avr-gcc -Wall -g -Og -DF_CPU=$(CLOCK) -DBAUD=$(BAUD) -DMOLD_DETECT_VERSION=$(MOLD_DETECT_VERSION) -mmcu=$(DEVICE)

.PHONY: all install flash fuse clean

all: main.hex

install: fuse flash

flash: all
	$(AVRDUDE) -U flash:w:main.hex:i

fuse:
	$(AVRDUDE) $(FUSES)

main.elf: $(OBJECTS)
	$(COMPILE) -o main.elf $(OBJECTS)

$(OBJECTS): %.o: ../%.c
	$(COMPILE) -c -o $@ $<

main.hex: main.elf
	rm -f main.hex
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	avr-size --format=avr --mcu=DEVICE main.elf

clean:
	rm -f *.o main.hex main.elf
