#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

//PORTB
#define RS 4 
#define EN 3
//PORTD
#define D4 5
#define D5 4
#define D6 3
#define D7 2
// ADC
//ADCSRA
/*
ADEN 7 Enables the conversion
ADSC 6 starts
ADATE 5 
ADIF 4
ADIE 3
ADPS2 2 To set clock speeds
ADPS1 1 111 is 128 prescalar. So clockspeed becomes 1/128th
ADPS0 0
*/
//ADMUX
/*
REFS1 7   When REFS1 REFS0 = 0 1, Voltage reference is 5V (Thats what I used.)
REFS0 6
ADLAR 5
MUX4  4  MUX is to select from A0 to A5, I have connected to A0, thus 00000
MUX3  3
MUX2  2
MUX1  1
MUX0  0
*/
//UART
//UCSR0A
/*
RXC0 7  Recieve
TXC0 6  Transmit
UDREO 5 When this is 1, we can transmit the next character and when it is 0, the transmission is going on.
FE0 4
DORO 3
UPEO 2
U2XO 1
MPCMO 0
*/
//UCSR0B
/*
RXCIEO 7
TXCIEO 6
UDRIEO 5
RXENO 4 Enables reciever
TXENO 3 Enables transmitter
UCSZO2 2
RXB8O 1
TXB8O 0
*/
//UCSR0C
/*
UMSELO1 7
UMESLOO 6
UPMO1 5
UPMOO 4
USBSO 3
UCSZ01 2 Character size bit1
UCSZ00 1 Character size bit0
UCPOL0 0
*/
//UBRR0H/UBRR0L
/*
UBRR = F/16xBaud -1
for 9600 baud at  16 MHz, UBRR = 103
*/
//UDR0 ='A' transmits A.

//Functions
double read_voltage(void) {
    ADCSRA |= (1 << ADSC);// Start conversion
    while (ADCSRA & (1 << ADSC));        

    int adc_value = ADCL | (ADCH << 8);//10-bit 
    double voltage = 5.0 * adc_value / 1023.0; 

    return voltage;
}


void initialize_ADC() {
    ADMUX |= (1 << REFS0);//MUX is already set to A0.
    ADCSRA |= (1 << ADPS0)|(1 << ADPS1)|(1 << ADPS2)|(1 << ADEN);//Prescaling factor set to 128 and enabled conversion
} 


void send_nibble(uint8_t nibble) {
  PORTD &= ~((1 << D4) | (1 << D5) | (1 << D6) | (1 << D7)); // Wherever I have bits other than Ds 1, by taking &, they stay 1.

  if(nibble & 1){
    PORTD |= 1 << D4;
  }
  if(nibble & 2){
    PORTD |= 1 << D5;
  }
  if(nibble & 4){
    PORTD |= 1 << D6;
  }
  if(nibble & 8){
    PORTD |= 1 << D7;
  }
  PORTB |= 1 << EN;
  _delay_us(100);
  PORTB &= ~(1 << EN);
  _delay_us(100);
  
}

void send_cmd(uint8_t cmd) {
    PORTB &=  ~(1 << RS);
    send_nibble(cmd >> 4);
    send_nibble(cmd);
    _delay_ms(2);
}

void send_data(uint8_t data) {
    PORTB |= 1 << RS; //RS = 1 means data and RS = 0 means command
    send_nibble(data >> 4);
    send_nibble(data);
    _delay_ms(2);
}

void initialize_LCD() {
    DDRB |= (1 << RS) | (1 << EN);
    DDRD |= (1 << D4) | (1 << D5) | (1 << D6) | (1 << D7);
    //Handshake to Convert 8-bit mode to 4-bit mode
    _delay_ms(20);
    send_nibble(0x03);
    _delay_ms(5);
    send_nibble(0x03);
    _delay_us(200);
    send_nibble(0x03);
    _delay_us(200);
    send_nibble(0x02);

    send_cmd(0x28);
    send_cmd(0x0C);
    send_cmd(0x01);
    send_cmd(0x06);
}

void set_cursor_to(int row, int col) {
    uint8_t addr = (uint8_t)(64*row + col);
    send_cmd(addr | 128);//DDRAM address is set by making 7th bit 1 and the rest 6 bits as address
}

void print_to_LCD(char *str) {
    int l = strlen(str);
    for (int i=0; i<l; i++) {
        send_data(str[i]);
    }
}


char* num_to_str(double num, int k) {
    long temp = lround(num * pow(10, k));
    int n1 = temp;
    int l = 0;
    while (n1 > 0) {
        n1 /= 10;
        l++;
    }
    if (l <= k) l = k + 1;

    char* str = malloc((l + 2) * sizeof(char));
    str[l + 1] = '\0';

    for (int i = 0; i < k; i++) {
        str[l - i] = (temp % 10) + '0';
        temp /= 10;
    }

    str[l - k] = '.';

    for (int i = l - k - 1; i >= 0; i--) {
        if (temp > 0) str[i] = (temp % 10) + '0';
        else str[i] = '0';
        temp /= 10;
    }

    return str;
}


float find_temperature(float a, float b, float v) {
    float temp = (v-a)/b;
    temp = round(temp*100)/100;
    return temp;
}

int main(){
    float a = 2.050;
    float b = 0.0042214;
    initialize_ADC();
    initialize_LCD();
    set_cursor_to(0, 0);
    print_to_LCD("V : ");
    set_cursor_to(1, 0);
    print_to_LCD("T : ");
    while (1) {
        _delay_ms(1000);
        double volt = read_voltage();
        char* Vstr = num_to_str(volt, 2);
        double temp = find_temperature(a,b,volt);
        char* Tempstr = num_to_str(temp, 2);
        set_cursor_to(0, 6);
        print_to_LCD("    ");
        set_cursor_to(0, 6);
        print_to_LCD(Vstr);
        print_to_LCD("V");
        free(Vstr);
        set_cursor_to(1, 6);
        print_to_LCD("    ");
        set_cursor_to(1, 6);
        print_to_LCD(Tempstr);
        send_data(223);
        print_to_LCD("C");
        free(Tempstr);
    }
    return 0;
}