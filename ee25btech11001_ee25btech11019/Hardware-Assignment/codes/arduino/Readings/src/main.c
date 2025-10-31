#define F_CPU 16000000UL

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include <math.h>

#define BAUD 9600L //Clock Speed

double read_voltage(void) {
    ADCSRA |= (1 << ADSC);// Start conversion
    while (ADCSRA & (1 << ADSC));        

    int adc_value = ADCL | (ADCH << 8);//10-bit 
    double voltage = 5.0 * adc_value / 1023.0; 

    return voltage;
}

void initialize_ADC() {
    ADMUX = (1 << REFS0);//MUX is already set to A0.
    ADCSRA = (1 << ADPS0)|(1 << ADPS1)|(1 << ADPS2)|(1 << ADEN);//Prescaling factor set to 128 and enabled conversion
} 

void initialize_UART() {
    //setting up the clock speed
    uint16_t ubbr = (uint16_t)(F_CPU/(16L*BAUD))-1;
    UBRR0L = (uint8_t)(ubbr);
    UBRR0H = (uint8_t)(ubbr >> 8);
    UCSR0B = (1 << TXEN0); //Enabled Transmitter
    UCSR0C = (1 << UCSZ00) | (1 << UCSZ01); 
}

void send_byte(uint8_t byte) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = byte;
}

void print_to_serial(double num) {
    int int_part = (int)num;
    int frac_part = abs((int)((num - int_part) * 10000)); // 4 decimal places
    // Send integer part
    if (int_part == 0)
        send_byte('0');
    else {
        char buffer[10];
        int i = 0;
        while (int_part > 0) {
            buffer[i++] = (int_part % 10) + '0';
            int_part /= 10;
        }
        for (int j = i - 1; j >= 0; j--)
            send_byte(buffer[j]);
    }
    send_byte('.');
    // Send fractional part
    for (int i = 0; i < 4; i++) {
        send_byte(((frac_part / (int)pow(10, 3 - i)) % 10) + '0');
    }

    send_byte('\r');
    send_byte('\n');

    _delay_ms(1000);
}


int main() {
    initialize_ADC();
    initialize_UART();
    while(1){
        double ans = read_voltage();
        print_to_serial(ans);
    }
    return 0;
}