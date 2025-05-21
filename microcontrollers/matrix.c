#include <avr/io.h>
#include <util/delay.h>

#define CS_PIN     PB2 // Arduino Digital Pin 10
#define CS_PORT    PORTB
#define CS_DDR     DDRB

#define NUM_DEVICES 8 // Number of MAX7219 chips in chain
#define INTENSITY 2 // LED broghtness from 0 to 15

#define BAUD 9600

uint8_t display[8][8];

void usart_init(uint16_t baud) {
    uint16_t ubrr = F_CPU / 16 / baud - 1;

    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)(ubrr);

    // Enable receiver and transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);

    // Set frame format: 8 data bits, 1 stop bit
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void spi_init() {
    // Set MOSI (PB3), SCK (PB5), and CS (PB2) as output
    DDRB |= (1 << PB3) | (1 << PB5) | (1 << CS_PIN);

    // Enable SPI, Master mode, fosc/16
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);

    // Set CS high (inactive)
    CS_PORT |= (1 << CS_PIN);
}

void usart_transmit(uint8_t data) {
    while (!(UCSR0A & (1 << UDRE0)));  // Wait for empty transmit buffer
    UDR0 = data;
}

uint8_t usart_receive(void) {
    while (!(UCSR0A & (1 << RXC0)));   // Wait for data
    return UDR0;
}

void usart_transmit_string(const char* str) {
    while (*str) {
        usart_transmit(*str++);
    }
}

void spi_send(uint8_t data) {
    SPDR = data;
    while (!(SPSR & (1 << SPIF)));
}

void max7219_send_all(uint8_t address, uint8_t data) {
    CS_PORT &= ~(1 << CS_PIN);

    for (uint8_t i = 0; i < NUM_DEVICES; i++) {
        spi_send(address);
        spi_send(data);
    }

    CS_PORT |= (1 << CS_PIN);
}

void max7219_init() {
    max7219_send_all(0x0F, 0x00); // Display test: off
    max7219_send_all(0x0C, 0x01); // Shutdown: normal operation
    max7219_send_all(0x0B, 0x07); // Scan limit: all 8 rows
    max7219_send_all(0x0A, INTENSITY); // Intensity: 7
    max7219_send_all(0x09, 0x00); // Decode mode: no decode
}


void max7219_set_pixel(uint8_t x, uint8_t y, uint8_t on) {
    if (x >= 32 || y >= 16) {
        return;
    }

    // Determine which matrix
    uint8_t row_block = y / 8;
    uint8_t col_block = x / 8;

    uint8_t matrix_index;

    if (row_block == 0) {
        matrix_index = col_block; // Top row: left to right
    }
    else {
        matrix_index = 7 - col_block; // Bottom row: right to left
    }

    uint8_t local_x, local_y;
    if (row_block == 0) { // Top row: invert local coords
        local_x = 7 - (x % 8);
        local_y = 7 - (y % 8);
    }
    else { // Bottom row
        local_x = x % 8;
        local_y = y % 8;
    }


    if (on)
        display[matrix_index][local_y] |= (1 << (7 - local_x));
    else
        display[matrix_index][local_y] &= ~(1 << (7 - local_x));
}

void max7219_clear() {
    for (uint8_t x = 0; x < 31; ++x) {
        for (uint8_t y = 0; y < 15; ++y) {
            max7219_set_pixel(x, y, 0);
        }
    }
}

void max7219_refresh() {
    for (uint8_t row = 0; row < 8; row++) {
        CS_PORT &= ~(1 << CS_PIN);

        for (uint8_t i = 0; i < NUM_DEVICES; i++) {
            spi_send(row + 1);
            spi_send(display[NUM_DEVICES - 1 - i][row]);
        }

        CS_PORT |= (1 << CS_PIN);
    }
}

void draw_border(void) {
    for (int x = 1; x < 31; ++x) {
        max7219_set_pixel(x, 1, 1);
    }

    for (int y = 1; y < 15; ++y) {
        max7219_set_pixel(30, y, 1);
    }

    for (int x = 1; x < 31; ++x) {
        max7219_set_pixel(x, 14, 1);
    }

    for (int y = 1; y < 15; ++y) {
        max7219_set_pixel(1, y, 1);
    }

    max7219_set_pixel(0, 0, 1);
    max7219_set_pixel(31, 0, 1);
    max7219_set_pixel(0, 15, 1);
    max7219_set_pixel(31, 15, 1);
}

void draw_ball(uint8_t x, uint8_t y) {
    max7219_set_pixel(x, y, 1);
    max7219_set_pixel(x + 1, y - 1, 1);
    max7219_set_pixel(x + 1, y, 1);
    max7219_set_pixel(x + 1, y + 1, 1);
    max7219_set_pixel(x - 1, y + 1, 1);
    max7219_set_pixel(x - 1, y, 1);
    max7219_set_pixel(x - 1, y - 1, 1);
    max7219_set_pixel(x, y + 1, 1);
    max7219_set_pixel(x, y - 1, 1);
}

int main(void) {
    usart_init(BAUD);
    spi_init();
    max7219_init();

    uint8_t ball1_x = 4;
    uint8_t ball1_y = 4;

    uint8_t ball2_x = 27;
    uint8_t ball2_y = 11;

    max7219_clear();
    draw_border();
    draw_ball(ball1_x, ball1_y);
    draw_ball(ball2_x, ball2_y);
    max7219_refresh();

    while (1) {
        uint8_t cmd = usart_receive();
        if (cmd == 'L' && ball1_x > 3) {
            ball1_x--;
        }
        if (cmd == 'R' && ball1_x < 28) {
            ball1_x++;
        }
        if (cmd == 'F' && ball1_y > 3) {
            ball1_y--;
        }
        if (cmd == 'B' && ball1_y < 12) {
            ball1_y++;
        }

        if (cmd == 'S' && ball2_x > 3) {
            ball2_x--;
        }
        if (cmd == 'C' && ball2_x < 28) {
            ball2_x++;
        }
        if (cmd == 'T' && ball2_y > 3) {
            ball2_y--;
        }
        if (cmd == 'X' && ball2_y < 12) {
            ball2_y++;
        }
        max7219_clear();
        draw_border();
        draw_ball(ball1_x, ball1_y);
        draw_ball(ball2_x, ball2_y);
        max7219_refresh();
    }
}
