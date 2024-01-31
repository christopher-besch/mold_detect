DEVICE     = atmega328p
CLOCK      = 2000000
BAUD       = 9600
PROGRAMMER = -c stk500v1 -P /dev/ttyACM0
# FUSES      = -U hfuse:w:0xd9:m -U lfuse:w:0x62:m
FUSES      = -U hfuse:w:0xd9:m -U lfuse:w:0x6f:m
AVRDUDE    = avrdude $(PROGRAMMER) -p $(DEVICE)
COMPILE    = bear -- avr-gcc -Wall -g -Og -DF_CPU=$(CLOCK) -DBAUD=$(BAUD) -mmcu=$(DEVICE)

all: main.hex

install: fuse flash

flash: all
	$(AVRDUDE) -U flash:w:main.hex:i

fuse:
	$(AVRDUDE) $(FUSES)

# TODO: do extra step with object files
main.elf: main.c
	$(COMPILE) -o main.elf main.c

main.hex: main.elf
	rm -f main.hex
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex

clean:
	rm -f *.o main.hex main.elf

