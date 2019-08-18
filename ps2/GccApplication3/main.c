/*
 * GccApplication3.c
 *
 * Created: 26-07-2019 04:55:51 PM
 * Author : HP
 */ 

#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>

#define MISO 3
#define MOSI 2
#define SCK 1
#define SS 0

void PWM_init()
{
	TCCR1B |=(1<<WGM12)|(1<<CS10);  // fast pwm is activate with No Prescaling
	TCCR1B &= ~(1<<WGM13);
	DDRB |= (1<<PINB6)|(1<<PINB5);  // Set pwm pin as output
	ICR1 = 255;
}


int select,l3,r3,start,up,right,down,left,l2,r2,l1,r1,triangle,circle,cross,squ;
int duty=255;
int cliflag = 0;

ISR(INT1_vect)
{
	PORTC |= (1<<PINC0)|(1<<PINC1);
	_delay_ms(100);
	cliflag=1;
}

void button_state(int data0,int data1){
	
	select= ((~data0) & 0x01) ? 1 : 0 ;
	l3= ((~data0) & 0x02) ? 1 : 0 ;
	r3=((~data0) & 0x04) ? 1 : 0 ;
	start=((~data0) & 0x08) ? 1 : 0 ;
	up=((~data0) & 0x10) ? 1 : 0 ;
	right=((~data0) & 0x20) ? 1 : 0 ;
	down=((~data0) & 0x40) ? 1 : 0 ;
	left=((~data0) & 0x80) ? 1 : 0 ;
	l2=((~data1) & 0x01) ? 1 : 0 ;
	r2=((~data1) & 0x02) ? 1 : 0 ;
	l1=((~data1) & 0x04) ? 1 : 0 ;
	r1=((~data1) & 0x08) ? 1 : 0 ;
	triangle=((~data1) & 0x10) ? 1 : 0 ;
	circle=((~data1) & 0x20) ? 1 : 0 ;
	cross=((~data1) & 0x40) ? 1 : 0 ;
	squ=((~data1) & 0x80) ? 1 : 0 ;
}

long int btod(int value)
{
	int n,k=1,ans=0;
	for(int i=0;i<7;i++)
	{
		n = value%10;
		ans += (n*k);
		k*=2;
		n/=10;
	}
	return ans;
	
}

void SPI_master_init(void)
{
	
	DDRB |=(1<<MOSI)|(1<<SCK)|(1<<SS); //set the MOSI and SCK output and all other as input
	DDRB &= ~(1<<MISO); //Set MISO as input
	PORTB |= (1<<SS);       // Disable slave initially by making high on SS pin
	SPCR |= (1<<SPE)|(1<<DORD)|(1<<MSTR)|(1<<CPOL)|(1<<CPHA)|(1<<SPR1)|(1<<SPR0); //spi enable,data order,master/slave select,clock polarity,clock phase,clock rate select1(fosc/64)
	SPSR |= (1<<SPI2X);   // Disable speed doubler
	
}
unsigned char SPI_write(unsigned char data)
{
	SPDR = data;                      // load data in spi data register
	while(!(SPSR & (1<<SPIF)));       // wait till transmission is complete
	//spi flag is cleared by first reading SPSR(with SPIF set) and then accessing SPDR hence buffer used here to access SPDR after SPSR read
	_delay_ms(30);
	return SPDR;
}


unsigned char SPI_Read()
{
	
	SPDR = 0xFF;				 // writing dummy data in spdr register
	while(!(SPSR & (1<<SPIF)));  // wait until data is received
	return(SPDR);
}

void forward()
{
	PORTA=0b00000101;
	OCR1A=(int)(duty); //setting PWM to 100% duty cycle
	OCR1B=(int)(duty);
	
}
void backward()
{
	PORTA=0b00001010;
	OCR1A=(int)(duty); //setting PWM to 100% duty cycle
	OCR1B=(int)(duty);
	
}
// OCR1A FOR LEFT AND OCR1B FOR RIGHT

void sharp_left()
{
	PORTA=0b00000001;
	OCR1A=(int)(duty*0.2); //setting PWM to 50% duty cycle
	OCR1B=(int)(duty*0.8);
	
}

void sharp_right()
{
	PORTA=0b0000100;
	OCR1A=(int)(duty*0.8); //setting PWM to 50% duty cycle
	OCR1B=(int)(duty*0.2);
	
}
void smooth_left()
{
	PORTA=0b00001001;
	OCR1A=(int)(duty*0.5); //setting PWM to 50% duty cycle
	OCR1B=(int)(duty*0.8);
	
}
void smooth_right()
{
	PORTA=0b00000110;
	OCR1A=(int)(duty*0.8); //setting PWM to 50% duty cycle
	OCR1B=(int)(duty*0.5);
	
}

int main(void)
{
	int data0,data1,data2,data3,data4,data5,a,b,c,d=0;
	float rx,ry,lx,ly;
	int cir_rx,cir_ry,cir_lx,cir_ly;
	
	PWM_init();
	SPI_master_init();
	
	EIMSK |= (1<<INT1);
	EICRA |= (1<<ISC01); //falling edge generates interrupt request
	
	
	while(d!= 0x73)
	{
		PORTB|=(1<<MOSI)|(1<<SCK); //set
		PORTB&=~(1<<SS);           // slave select setting to low(
		
		
		SPI_write(0x01); //entering config mode
		SPI_write(0x43);
		SPI_write(0x00);
		SPI_write(0x01);
		SPI_write(0x00);
		
		PORTB|=(1<<MOSI);
		_delay_ms(1);
		PORTB|=(1<<SS);

		_delay_ms(10);

		
		PORTB|=(1<<MOSI)|(1<<SCK); //setting
		
		PORTB&=~(1<<SS); // clearing

		SPI_write(0x01); //seting analog mode
		SPI_write(0x44);
		SPI_write(0x00);
		SPI_write(0x01);   // 0x01 for analog and 0x00 for digital mode
		SPI_write(0x03);   // controller mode is locked
		SPI_write(0x00);
		SPI_write(0x00);
		SPI_write(0x00);
		SPI_write(0x00);

		PORTB|=(1<<MOSI);
		_delay_ms(1);
		PORTB|=(1<<SS);

		_delay_ms(10);

		
		PORTB|=(1<<MOSI)|(1<<SCK);
		PORTB&=~(1<<SS);

		SPI_write(0x01);/*was using this to poll.The code will work fine even without this*/
		SPI_write(0x43);
		SPI_write(0x00);
		SPI_write(0x00);
		SPI_write(0x5A);
		SPI_write(0x5A);
		SPI_write(0x5A);
		SPI_write(0x5A);
		SPI_write(0x5A);

		PORTB|=(1<<MOSI);
		_delay_ms(1);
		PORTB|=(1<<SS);
		_delay_ms(10);


		
		
		PORTB|=(1<<MOSI)|(1<<SCK);
		PORTB&=~(1<<SS);
		
		SPI_write(0x01);
		d= SPI_write(0x42); //making sure we're in the analog mode the value of d will be 0x73
		SPI_write(0x00);   // if d is not equal to 0x73 this loop will repeat
		SPI_write(0x00);
		SPI_write(0x00);
		SPI_write(0x00);
		SPI_write(0x00);
		SPI_write(0x00);
		SPI_write(0x00);
		
		PORTB|=(1<<MOSI);
		_delay_ms(1);
		PORTB|=(1<<SS);
		_delay_ms(10);
		
	}
	while(d==0x73)
	{
		while (1)
		{
			
			PORTB|=(1<<MOSI) | (1<<SCK);
			PORTB&=~(1<<SS);
			
			
			a=SPI_write(0x01);
			b=SPI_write(0x42);
			c=SPI_write(0x00);
			

			data0 = SPI_write(0x00); //buttons set 1 8
			data1 = SPI_write(0x00); //button set 2  8
			data2 = SPI_write(0x00); //  rx
			data3 = SPI_write(0x00); //  ry
			data4 = SPI_write(0x00); //  lx
			data5 = SPI_write(0x00); //  ly

			_delay_us(1);
			PORTB|=(1<<MOSI);
			_delay_us(1);
			PORTB|=(1<<SS);
			
			button_state(data0,data1);
			
			rx = (float)((btod(data2)-127)/127);
			_delay_ms(1);
			ry = (float)((btod(data3)-127)/127);
			_delay_ms(1);
			lx = (float)((btod(data4)-127)/127);
			_delay_ms(1);
			ly = (float)((btod(data5)-127)/127);
			_delay_ms(1);
			
			
			// Mapping from Square to Circular
			
			cir_rx = (int)(rx*sqrt(1-(ry*ry/2))*127);
			cir_ry = (int)(ry*sqrt(1-(rx*rx/2))*127);
			cir_lx = (int)(lx*sqrt(1-(ly*ly/2))*127);
			cir_ly = (int)(ly*sqrt(1-(lx*lx/2))*127);
			
			
			// Make combination with different buttons
			//what happen when different button press
			if(triangle){
				forward();
			}
			if(cross){
				backward();
			}
			if(circle){
				sharp_right();
			}
			if(squ){
				sharp_left();
			}
			
			if(triangle && circle){
				smooth_right();
			}
			if(triangle && cross){
				smooth_left();
			}
			if(cir_rx>=0 && cir_rx <=127)
			{
				PORTC |= (1 << PINC0);
				_delay_ms(1);
				PORTC &= ~(1 << PINC0);
			}
			if(cir_rx >= -127 && cir_rx < 0)
			{
				PORTC |= (1 << PINC2);
				_delay_ms(1);
				PORTC &= ~(1 << PINC2);
			}
			if(cir_ly >= 0  && cir_ly <=127)
			{
				PORTC |= (1 << PINC1);
				_delay_ms(1);
				PORTC &= ~(1 << PINC1);
			}
			if(cir_ly >= -127 && cir_ly < 0)
			{
				PORTC |= (1 << PINC0);
				_delay_ms(1);
				PORTC &= ~(1 << PINC0);
			}
		}
	}

}

