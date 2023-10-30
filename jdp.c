#define TRIG1 PB3
#define ECHO1 PB2
#define TRIG2 PB0
#define ECHO2 PB1
#define RANGE_PER_CLOCK 1.098   
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include "i2c.h"
#include "i2c.c"
#include "SSD1306.h"
#include "SSD1306.c"
void uart_init (void);
void uart_send (unsigned char);
void send_string (char *stringAddress);
void timer0_init(void);
void send_to_monitor(unsigned char, unsigned char, float, unsigned int);
void adc_init(void);
unsigned int get_adc(void);

int main(void)
{
    DDRD = 0xFF;
    DDRB = 0xFF;
    PORTB = 0xFF; 
    DDRC = 0xFF;
    PORTC = 0xFF;
    OLED_Init();  //initialize the OLED
    
    unsigned char ledDigits[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66,
                                 0x6D, 0x7D, 0x07, 0x7F, 0x67};
    unsigned int k=0;
    unsigned char rising_edge_clocks, falling_edge_clocks, echo_width_clocks, rising_edge_clocks2, falling_edge_clocks2, echo_width_clocks2;
    unsigned char DIG1, DIG2;
    float target_range, target_range2;
    unsigned int i=0;
    DDRB = 1<<TRIG1 | 1<<TRIG2; //set trig pins as output
    PORTB &= ~(1<<TRIG1) & ~(1<<TRIG2); //set trig pins low
    char buffer[10];

    uart_init();
    timer0_init();
    while(1)
    {
        OLED_Clear();
        TCNT0 = 0; 
        PORTB |= 1<<TRIG1;
        _delay_us(10);
        PORTB &= ~(1<<TRIG1);

        while((PINB & (1<<ECHO1))==0);
        rising_edge_clocks = TCNT0; 
        

        while(!(PINB & (1<<ECHO1))==0);
        falling_edge_clocks = TCNT0;

        if (falling_edge_clocks > rising_edge_clocks)
        {
            echo_width_clocks = falling_edge_clocks - rising_edge_clocks;
            target_range = echo_width_clocks * RANGE_PER_CLOCK; 
            
        if (target_range < 10 && k >= 0) // if target range is less than 10 cm, increment k
        {
            k++;
            OLED_Clear();
            if (k > 9)
            {
                k=9;
                
            }   
        }
        DIG1 = k;
        PORTD = ledDigits[DIG1]; //display k on 7-segment display
        PORTC = ~(1<<1);
        _delay_ms(5);
        //send_to_monitor(rising_edge_clocks, falling_edge_clocks, target_range, k);
        }
        _delay_ms(100);

        

        PORTB |= 1<<TRIG2;
        _delay_us(10);
        PORTB &= ~(1<<TRIG2);


        while((PINB & (1<<ECHO2))==0);
        rising_edge_clocks2 = TCNT0;

        while(!(PINB & (1<<ECHO2))==0);
        falling_edge_clocks2 = TCNT0;

        if (falling_edge_clocks2 > rising_edge_clocks2)
        {
            echo_width_clocks2 = falling_edge_clocks2 - rising_edge_clocks2;
            target_range2 = echo_width_clocks2 * RANGE_PER_CLOCK;
        if (target_range2 < 10 && k > 0) // if target range is less than 10 cm, decrement k
        {
            k--;
            OLED_Clear();
            if (k <= 0)
            {
                k=0;
            }   
        }
        DIG1 = k;
        PORTD = ledDigits[DIG1]; //display k on 7-segment display
        PORTC = ~(1<<1);
        _delay_ms(5);
        
        _delay_ms(100);
    }
    if (k == 0)
        {
            OLED_DisplayString("Room is empty");
        }
        else if (k > 0  && k <= 3)
        {
            OLED_DisplayString("Room is at low capacity");
            
        }
        else if (k > 3 && k <= 6)
        {
            OLED_DisplayString("Room is at medium capacity");
        }
        else if (k > 6 && k < 9)
        {
            OLED_DisplayString("Room is at high capacity");
        }
        else 
        {
            OLED_DisplayString("Room is full");
        }
    }
    return 0;
}


void send_to_monitor(unsigned char rising_edge_clocks, unsigned char falling_edge_clocks, float target_range, unsigned int k)
{
    char buffer[10];
    send_string("Target range: ");
    dtostrf(target_range, 3, 0, buffer);
    send_string(buffer);
    send_string(" cm\n\r");
    send_string("k: ");
    itoa(k, buffer, 10);
}

void timer0_init(void)
{
    TCCR0A = 0;
    TCCR0B = 5;
    TCNT0 = 0;
}

void uart_init(void){
    UCSR0B = (1<<TXEN0);
    UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
    UBRR0L = 103;
}

void send_string(char *stringAddress){
    unsigned char i;
    for (i=0; i<strlen(stringAddress); i++)
        uart_send(stringAddress[i]);
}

void uart_send(unsigned char letter){
    while(!(UCSR0A & (1<<UDRE0)));
    UDR0 = letter;
}