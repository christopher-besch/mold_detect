#include <avr/io.h>
#include <util/delay.h>

#include "spi.h"

void spi_controller_init()
{
    // on rising edge of CLK
    // CLK low on rising/falling edge of /CS -> SPI mode 0 (0, 0)
    // MSB

    // set COPI, SCK and CS output
    DDRB |= (1 << DD3) | (1 << DD5) | (1 << DD2);
    // set CIPO input
    DDRB &= ~(1 << DD4);
    // set CS high (pulled low when transceiving data)
    PORTB |= (1 << PORT2);

    // no interrupts, enable SPI, MSB, enable controller operation (formerly master)
    // SPI mode 0 (CPOL=0, CPHA=0), set speed to 1/16th the clock speed
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
    // no double speed
    SPSR = 0;
}

void spi_start()
{
    PORTB &= ~(1 << PORT2);
}

void spi_end()
{
    PORTB |= (1 << PORT2);
}

char spi_transceive_char(char data)
{
    // start transmission
    SPDR = data;
    // wait for completed transmission
    while(!(SPSR & (1 << SPIF)))
        ;
    // receive byte
    return SPDR;
}

char spi_receive_char()
{
    // one byte needs to be clocked
    return spi_transceive_char(0);
}
