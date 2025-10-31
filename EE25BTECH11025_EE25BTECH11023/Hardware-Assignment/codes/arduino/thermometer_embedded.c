#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <math.h>

// --- Configuration Definitions ---
#define F_CPU 16000000UL // Clock speed of Arduino Uno (16 MHz)
#define BAUD 9600        // Baud rate for Serial communication
#define UBRR_VAL (F_CPU / 16 / BAUD - 1) // Calculated value for UBRR register

// ====================================================================
// !!! CRITICAL: VERIFY YOUR WIRING MATCHES THIS PIN DEFINITION !!!
// ====================================================================

// --- LCD Control Pin Mapping ---
// RS (D12) -> PD6
#define LCD_RS_PORT PORTD
#define LCD_RS_DDR  DDRD
#define LCD_RS_PIN  (1 << PD6)

// EN (D11) -> PB3
#define LCD_EN_PORT PORTB
#define LCD_EN_DDR  DDRB
#define LCD_EN_PIN  (1 << PB3)

// --- LCD Data Pin Mapping (D4-D7) ---
// Note: We are using digital pins D5, D4, D3, D2 for the LCD's D4, D5, D6, D7
// D5 (LCD D4) -> PD5
// D4 (LCD D5) -> PD4
// D3 (LCD D6) -> PD3
// D2 (LCD D7) -> PD2 
#define LCD_DATA_PORT PORTD
#define LCD_DATA_DDR  DDRD

// --- Function Prototypes ---
void UART_Init(void);
void UART_Transmit(char data);
void UART_PrintString(char* str);
void ADC_Init(void);
uint16_t ADC_Read(uint8_t channel);
void LCD_Init(void);
void LCD_EnablePulse(void); // Added separate function for clarity
void LCD_Send4Bit(uint8_t data);
void LCD_Command(uint8_t command);
void LCD_Char(uint8_t data);
void LCD_String(char *str);
void LCD_SetCursor(uint8_t row, uint8_t col);


// --- 1. UART (Serial) Functions ---

// Initializes the UART module for serial communication
void UART_Init(void) {
    UBRR0H = (uint8_t)(UBRR_VAL >> 8);
    UBRR0L = (uint8_t)UBRR_VAL;
    UCSR0B = (1 << TXEN0); // Enable transmitter
    UCSR0C = (3 << UCSZ00); // 8 data bits, 1 stop bit
}

// Transmits a single byte of data
void UART_Transmit(char data) {
    while (!(UCSR0A & (1 << UDRE0))); // Wait for empty transmit buffer
    UDR0 = data;
}

// Transmits an entire string
void UART_PrintString(char* str) {
    for (uint8_t i = 0; str[i] != '\0'; i++) {
        UART_Transmit(str[i]);
    }
}


// --- 2. ADC (Analog Read) Functions ---

// Initializes the ADC module
void ADC_Init(void) {
    // Set VREF to AVCC (Arduino default)
    ADMUX |= (1 << REFS0);
    
    // Set ADC Prescaler to 128 (125kHz, required for accuracy)
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    
    // Enable ADC
    ADCSRA |= (1 << ADEN);
}

// Reads the 10-bit value from the specified channel (0 for A0)
uint16_t ADC_Read(uint8_t channel) {
    // Select the channel (ADC0-ADC5 correspond to A0-A5)
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
    
    // Start single conversion
    ADCSRA |= (1 << ADSC);
    
    // Wait for the conversion to complete
    while (ADCSRA & (1 << ADSC));
    
    // Return the 10-bit result
    return ADC;
}


// --- 3. LCD Driver Functions (Timing Improved) ---

// Pulses the Enable pin (E) to latch the data/command
void LCD_EnablePulse(void) {
    LCD_EN_PORT |= LCD_EN_PIN;  // E HIGH
    _delay_us(1);               // Hold E high briefly
    LCD_EN_PORT &= ~LCD_EN_PIN; // E LOW
    
    // *** INCREASED DELAY FOR STABILITY ***
    // This delay gives the LCD enough time to process the instruction.
    _delay_us(250);            
}

// Sends a 4-bit nibble to the LCD data pins (D4-D7)
void LCD_Send4Bit(uint8_t data) {
    // Clear the current data pins (PD2-PD5)
    LCD_DATA_PORT &= ~((1 << PD5) | (1 << PD4) | (1 << PD3) | (1 << PD2));

    // Map the 4 bits of 'data' to the physical pins D2, D3, D4, D5
    if (data & 0x01) LCD_DATA_PORT |= (1 << PD2); // D4 (LSB of nibble) -> PD2
    if (data & 0x02) LCD_DATA_PORT |= (1 << PD3); // D5 -> PD3
    if (data & 0x04) LCD_DATA_PORT |= (1 << PD4); // D6 -> PD4
    if (data & 0x08) LCD_DATA_PORT |= (1 << PD5); // D7 (MSB of nibble) -> PD5

    LCD_EnablePulse();
}

// Sends a command to the LCD (RS low)
void LCD_Command(uint8_t command) {
    LCD_RS_PORT &= ~LCD_RS_PIN; // RS = 0 (Command Mode)
    
    // Send high nibble
    LCD_Send4Bit(command >> 4);
    
    // Send low nibble
    LCD_Send4Bit(command & 0x0F);
    
    // Check for slow commands and add extra delay
    if (command == 0x01 || command == 0x02) {
        _delay_ms(5); // Long delay for Clear Display or Return Home
    }
}

// Sends a character/data to the LCD (RS high)
void LCD_Char(uint8_t data) {
    LCD_RS_PORT |= LCD_RS_PIN; // RS = 1 (Data Mode)
    
    // Send high nibble
    LCD_Send4Bit(data >> 4);
    
    // Send low nibble
    LCD_Send4Bit(data & 0x0F);
}

// Sends a string to the LCD
void LCD_String(char *str) {
    while (*str != '\0') {
        LCD_Char(*str++);
    }
}

// Sets the cursor position
void LCD_SetCursor(uint8_t row, uint8_t col) {
    uint8_t address = 0x80;
    if (row == 1) {
        address = 0xC0; // Start address for row 2 (0x40 offset)
    }
    address += col;
    LCD_Command(address);
}

// Initializes the LCD in 4-bit mode (Timing Improved)
void LCD_Init(void) {
    // 1. Set all LCD pins as outputs
    LCD_RS_DDR |= LCD_RS_PIN;
    LCD_EN_DDR |= LCD_EN_PIN;
    LCD_DATA_DDR |= ((1 << PD5) | (1 << PD4) | (1 << PD3) | (1 << PD2));

    // 2. Delay for power-up stability
    _delay_ms(50);
    
    // 3. Start 4-bit initialization sequence
    LCD_RS_PORT &= ~LCD_RS_PIN; // Command Mode
    LCD_EN_PORT &= ~LCD_EN_PIN; // E Low
    
    // Send 0x03 three times (Required for proper 4-bit mode negotiation)
    LCD_Send4Bit(0x03);
    _delay_ms(5);
    LCD_Send4Bit(0x03);
    _delay_us(150);
    LCD_Send4Bit(0x03);
    _delay_us(150);
    
    // Send 0x02 to set 4-bit mode
    LCD_Send4Bit(0x02); 
    _delay_us(150);

    // 4. Function Set (2 lines, 5x8 font)
    LCD_Command(0x28);
    // 5. Display Off
    LCD_Command(0x08);
    // 6. Clear Display
    LCD_Command(0x01); // Delay handled inside LCD_Command
    // 7. Entry Mode Set (Increment, no shift)
    LCD_Command(0x06);
    // 8. Display On, Cursor Off, Blink Off
    LCD_Command(0x0C);
}


// --- 4. Main Program (Embedded C equivalent of setup() and loop()) ---

int main(void) {
    // === SETUP (Initialization) ===
    UART_Init();
    ADC_Init();
    LCD_Init();

    // Splash Screen
    LCD_Command(0x01); 
    LCD_String("Digital");
    
    LCD_SetCursor(1, 0); 
    LCD_String("Thermometer");
    
    _delay_ms(1500);
    
    LCD_Command(0x01); 
    LCD_String("Temperature:");


    // === LOOP (Main Control Logic) ===
    while (1) {
        // Read sensor value from A0 (channel 0)
        uint16_t sensorValue = ADC_Read(0); 

        // Calculate voltage (5.0 * sensorValue / 1023.0)
        double voltage = (5.0 * (double)sensorValue) / 1023.0;

        // Perform complex temperature calculation (Preserved original formula)
        double temp = -876.974483 * pow(voltage, 3) +
                       6107.116386 * pow(voltage, 2) - 
                      13897.546295 * voltage + 
                      10390.951523;

        // --- Serial Output (Voltage) ---
        char uart_buf[10];
        dtostrf(voltage, 1, 4, uart_buf); 
        UART_PrintString("Voltage: ");
        UART_PrintString(uart_buf);
        UART_PrintString("V\r\n"); 

        // --- LCD Output (Temperature) ---
        char lcd_buf[6];
        dtostrf(temp, 1, 2, lcd_buf); 

        LCD_SetCursor(1, 0); 
        LCD_String(lcd_buf);
        
        // Print the degree symbol (ASCII 0xDF) and 'C'
        LCD_Char(0xDF);  
        LCD_Char('C');
        
        _delay_ms(1000); // Wait 1 second
        
        // Clear the temperature line for the next reading
        LCD_SetCursor(1, 0);  
        LCD_String("                "); // 16 spaces to clear line
    }
}