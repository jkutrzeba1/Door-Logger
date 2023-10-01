#include<avr/io.h>
#include<util/delay.h>

#define F_CPU 1000000UL		/* Define frequency here its 1MHz */
#define USART_BAUDRATE 1200
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)	

void USART_Init()
{
	/* Set baud rate */
	UBRRH = (unsigned char)(BAUD_PRESCALE>>8);
	UBRRL = (unsigned char)BAUD_PRESCALE;
	/* Enable receiver and transmitter */
	UCSRB = (1<<RXEN)|(1<<TXEN);
	/* Set frame format: 8data, 2stop bit */
	UCSRC = (1<<USBS)|(3<<UCSZ0);
}

void USART_Transmit_LL(uint32_t c){
	
	while ( !( UCSRA & (1<<UDRE)) ) {};
	
	unsigned char buf1 = (c&255);
	
	UDR = buf1;
	
	while ( !( UCSRA & (1<<UDRE)) ) {};
	
	unsigned char buf2 = ((c>>8)&255);
	
	UDR = buf2;
	
	while ( !( UCSRA & (1<<UDRE)) ) {};
	
	unsigned char buf3 = ((c>>16)&255);
	
	UDR = buf3;
	
	while ( !( UCSRA & (1<<UDRE)) ) {};
	
	unsigned char buf4 = ((c>>24)&255);
	
	UDR = buf4;
}

void USART_Transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSRA & (1<<UDRE)) )
	;
	/* Put data into buffer, sends the data */
	UDR = data;
}

int USART_Receive( unsigned char *data){
	
	if((UCSRA & (1<<RXC))){
		
		*data = UDR;
		
		return 1;
	}
	
	return 0;
}

unsigned char USART_Receive_sleep(){
	
	while(!(UCSRA & (1<<RXC))){}
	
	return UDR;
	
}

/*
	Out codes:
	
	100 - door open alarm on
	101 - door open alarm off
	102 - door closed alarm off
	103 - ping
	104 - elapsed time to alarm bring
*/

int main(void){
	
	USART_Init();

	DDRD |= (1<<5); //out
	PORTD |= (1<<5);
	DDRD |= (1<<2); //out
	PORTD &= (~(1<<2));
	
	
	
	DDRD |= (1<<4); //out
	DDRD &= (~(1<<3)); //in
	
	
	// obwod otwarty -> pin stan wysoki
	
	//pull-up active
	
	int alarm_sleep = 1;
	PORTD &= (~(1<<4));
	
	int alarm_on = 0;
	
	int door_open;
	
	
	if((PIND & (1<<3)) == 0){
		
		// pull-up grounded -> pnp base not doped -> magnet circuit open
		
		door_open = 1;
		
	}
	if((PIND & (1<<3)) == (1<<3)){
		
		door_open = 0;
		
	}
	
	unsigned char buff;
	
	int counter = 0;
	
	
	int sleeper = 0;
	uint32_t tm_interval_sleeper = 0;
	uint32_t tm_interval_sleeper_counter = 0;
	
	
	
	while(1){
		
		counter++;
		
		if(counter==10){
			
			USART_Transmit(103);
			
			counter = 0;
			
			if(sleeper==1){
				
				tm_interval_sleeper_counter++;
				
				if(tm_interval_sleeper == tm_interval_sleeper_counter){
					
					sleeper = 0;
					PORTD |= (1<<4);
					
				}
				
			}
			
		}
		
		if(USART_Receive(&buff)){
			
			if(buff==25){
				
				alarm_on = 1;
				
			}
			if(buff==26){
				
				alarm_on = 0;
				
			}
			if(buff==30){
				
				sleeper = 1;
				
				uint32_t tmp = 0;
				
				unsigned char c_;

				c_ = USART_Receive_sleep();
				tmp = c_;
				tm_interval_sleeper |= tmp;
				
				c_ = USART_Receive_sleep();
				tmp = c_;
				tm_interval_sleeper |= (tmp<<8);
				
				c_ = USART_Receive_sleep();
				tmp = c_;
				tm_interval_sleeper |= (tmp<<16);
				
				c_ = USART_Receive_sleep();
				tmp = c_;
				tm_interval_sleeper |= (tmp<<24);	
				
				
			}
			
		}
		
		if(alarm_sleep == 1){
			
			if(alarm_on==0){
				
				
				if(door_open==1){
					
					if((PIND & (1<<3)) == (1<<3)){
						
						_delay_ms(80);
						
						if((PIND & (1<<3)) == (1<<3)){
							
							door_open = 0;
							USART_Transmit(102);
							
						}
						
						
					}
					
					
				}
				
				if(door_open==0){
					
					if((PIND & (1<<3)) == 0){
						
						_delay_ms(80);
						
						if((PIND & (1<<3)) == 0){
							
							door_open = 1;
							USART_Transmit(101);
						}
						
					}
					
					
					
				}
				
				
			}
			else{
				if((PIND & (1<<3)) == 0){
					
					_delay_ms(80);
					
					if((PIND & (1<<3)) == 0){
						
						if(alarm_on == 1){
							
							PORTD |= (1<<4); //beezer on
							alarm_sleep = 0;
							
							USART_Transmit(100);
							
						}
						
					}
					
				}
			}
			
		}
		
		_delay_ms(100);
	}
	
}