/*
 * Stop watch project.c
 *
 * Created: 1/31/2021 1:07:18 PM
 * Author : dell
 */ 
#define F_CPU 1000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

typedef unsigned char uint8;
//Initilization global variables
uint8 seconds_1=0;
uint8 seconds_2=0;
uint8 minutes_1=0;
uint8 minutes_2=0;
uint8 hours_1=0;
uint8 hours_2=0;



//interrupt 0 function(Reset)
void INt0_Init(void)
{
	SREG  &=~(1<<7);        
	MCUCR |=(1<<ISC01);     //trigger Int0 with falling edge
	MCUCR &=~(1<<ISC00);
	GICR  |=(1<<INT0);      //Enable external interrupt pin INT0
	DDRD  &=~(1<<PD2);      //Pin PD2 is input pin
	PORTD |=(1<<PD2);       //Enable internal pull up 
	SREG  |=(1<<7);
}

//reset stop watch
ISR(INT0_vect)
{
	//Reset global variables
	seconds_1=0;
	seconds_2=0;
	minutes_1=0;
	minutes_2=0;
	hours_1=0;
	hours_2=0;
}


//interrupt 1 function(Pause)
void INT1_Init(void)
{
	SREG  &=~(1<<7);
	GICR  |=(1<<INT1);              //Enable external interrupt pin INT1
	MCUCR |=(1<<ISC11)|(1<<ISC10);  //Trigger with rising edge
	DDRD  &=~(1<<PD3);              //Pin PD3 is input pin
	SREG  |=(1<<7);
}

//Pause stop watch
ISR(INT1_vect)
{
	TCCR1B =0;           /* pause clock(timer)
	                      CS12=CS10=WGM12=0*/
}



//interrupt 2 function(Resume)
void INT2_Init(void)
{
	SREG  &=~(1<<7);
	GICR  |=(1<<INT2);   //Enable external interrupt pin INT2
	MCUCR &=~(1<<ISC2);  //trigger with falling edge
	DDRB  &=~(1<<PB2);   //Pin PB2 is input pin
	PORTB |=(1<<PB2);    //Enable internal pull up 
	SREG  |=(1<<7); 
}

//Resume stop watch
ISR(INT2_vect)
{
	TCCR1B|=(1<<CS12)|(1<<CS10)|(1<<WGM12);   // Resume clock(Timer) N=1024
}


//counting using timer 1(compare mood)
void Timer1_CTC_INIT(void)
{

	SREG   &= ~(1<<7); // Disable interrupts by clearing I-bit
	TCNT1  = 0;//Set Initial Value 
	OCR1A  = 1000;// Set Compare Value
	TIMSK  =(1<<OCIE1A); //Enable Timer1 Compare Interrupt channel A
	TCCR1A =(1<<FOC1A) ;
	TCCR1B =(1<<CS12) | (1<<CS10) | (1<<WGM12) ;
	SREG   |= (1<<7);// Enable interrupts by setting I-bit                    	             
}

ISR(TIMER1_COMPA_vect)
{

	// Second incrementaion
	if (seconds_1==9)
	{
		seconds_1=0;
		seconds_2++;
	}
	else
	{
		seconds_1++;
	} 
	
	// minute incrementation
	if(seconds_2==6)
	{
		seconds_2=0;
		minutes_1++;
	}
	
	if (minutes_1==10)
	{
		minutes_1=0;
		minutes_2++;
	}
	
	
	//hours incrementation
	if (minutes_2==6)
	{
		minutes_2=0;
		hours_1++;
	}
	
	if(hours_1==10)
	{
		hours_1=0;
		hours_2++;
	}
	

	if ((hours_2==2)&&(hours_1==5))
	{
		hours_1=0;
		hours_2=0;
	}
	
}

int main(void)
{

	DDRC =0x0F;// The first four bits in port C is output pins
	DDRA =0x3F;// The first six bits in port A is output pins
	PORTC=0x00;//set initial value for port c by zero
	PORTA=0xFF;//set initial value for port c by 1
	
	Timer1_CTC_INIT();
	INt0_Init();
	INT1_Init();
	INT2_Init();
    
    while(1)
    {
	    PORTA =(1<<0);//enable  first digit in 7 and turn off the others
	    PORTC = (PORTC & 0xF0)|(seconds_1 & 0x0F);//Displays the value or this bit
	    _delay_ms(5);

	    PORTA =(1<<1);//enable  second digit in 7 and turn off the others
	    PORTC = (PORTC & 0xF0)|(seconds_2 & 0x0F);//Displays the value or this bit
	    _delay_ms(5);

	    PORTA =(1<<2);//enable  third digit in 7 and turn off the others
	    PORTC = (PORTC & 0xF0)|(minutes_1 & 0x0F);//Displays the value or this bit
	    _delay_ms(5);

	    PORTA =(1<<3);//enable  fourth digit in 7 and turn off the others
	    PORTC = (PORTC & 0xF0)|(minutes_2 & 0x0F);//Displays the value or this bit
	    _delay_ms(5);

	    PORTA =(1<<4);//enable  fifth digit in 7 and turn off the others
	    PORTC = (PORTC & 0xF0)|(hours_1 & 0x0F);//Displays the value or this bit
	    _delay_ms(5);

	    PORTA =(1<<5);//enable sixth digit in 7 and turn off the others
	    PORTC = (PORTC & 0xF0)|(hours_2 & 0x0F);//Displays the value or this bit
	    _delay_ms(5);


		
    }
}

