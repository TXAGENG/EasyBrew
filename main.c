/*
 * main.c
 */
#include <msp430.h>
#include <LCD.h>


void InitLcd(void);
void sendData(unsigned int data);
void sendCommand(unsigned int command);
void Delay(long int x);
void setup(void);

unsigned int input = 0;


int main(void)
{
	int i;
	WDTCTL = WDTPW + WDTHOLD;             // Stop watchdog timer
	BCSCTL1 = RSEL2 + RSEL3;
	//P1DIR |= BIT4 + BIT7;
	//USICTL0 = USICTL0 & ~USISWRST;	//released for operation

	setup();
	InitLcd();
while(1){
	sendCommand(PASET);
	sendData(0);
	sendData(130);
	// Column address set (command 0x2A)
	sendCommand(CASET);
	sendData(0);
	sendData(130);
	// WRITE MEMORY
	sendCommand(RAMWR);
	// loop on total number of pixels / 2
	for (i = 0; i < ((((130 - 0 + 1) * (130 - 0 + 1)) / 2) + 130); i++) {
	// use the color value to output three data bytes covering two pixels
	sendData((RED >> 4) & 0xFF);
	sendData(((RED & 0xF) << 4) | ((RED >> 8) & 0xF));
	sendData(RED & 0xFF);
	}
	sendCommand(PASET);
	sendData(0);
	sendData(130);
	// Column address set (command 0x2A)
	sendCommand(CASET);
	sendData(0);
	sendData(130);
	// WRITE MEMORY
	sendCommand(RAMWR);
	// loop on total number of pixels / 2
	for (i = 0; i < ((((130 - 0 + 1) * (130 - 0 + 1)) / 2) + 130); i++) {
	// use the color value to output three data bytes covering two pixels
	sendData((BLACK >> 4) & 0xFF);
	sendData(((BLACK & 0xF) << 4) | ((BLACK >> 8) & 0xF));
	sendData(BLACK & 0xFF);
	}
}
}

void sendData(unsigned int data)
{
	P1OUT = P1OUT & ~BIT7;
	input = USISR;
	USICNT = 0x09 + USI16B;
	data |= 0x0100;
	data = data<<7;
	USISR = data;
	P1OUT |= BIT7;

}
//must shift data to the MSB direction
void sendCommand(unsigned int command)
{
	P1OUT = P1OUT & ~BIT7;
	input = USISR;
	USICNT = 0x09 + USI16B;
	command = command & ~0x0100;
	command = command<<7;
	USISR = command;
	P1OUT |= BIT7;

}
void Delay(long int x)
{
	long int i = 0;
	while(i<x)i++;

}

void InitLcd(void) {
// Hardware reset
P1OUT = P1OUT & ~BIT4;
Delay(100000);
P1OUT |= BIT4;
Delay(100000);
// Display control
sendCommand(DISCTL);
sendData(0x00); // P1: 0x00 = 2 divisions, switching period=8 (default)
sendData(0x20); // P2: 0x20 = nlines/4 - 1 = 132/4 - 1 = 32)
sendData(0x00); // P3: 0x00 = no inversely highlighted lines
// COM scan
sendCommand(COMSCN);
sendData(1); // P1: 0x01 = Scan 1->80, 160<-81
// Internal oscilator ON
sendCommand(OSCON);
// Sleep out
sendCommand(SLPOUT);
// Power control
sendCommand(PWRCTR);
sendData(0x0f); // reference voltage regulator on, circuit voltage follower on, BOOST ON
// Inverse display
sendCommand(DISINV);
// Data control
sendCommand(DATCTL);
sendData(0x01); // P1: 0x01 = page address inverted, column address normal, address scan in column direction
sendData(0x00); // P2: 0x00 = RGB sequence (default value)
sendData(0x02); // P3: 0x02 = Grayscale -> 16 (selects 12-bit color, type A)
// Voltage control (contrast setting)
sendCommand(VOLCTR);
sendData(30); // P1 = 32 volume value (experiment with this value to get the best contrast)
sendData(3); // P2 = 3 resistance ratio (only value that works)
// allow power supply to stabilize
Delay(1000000);
// turn on the display
sendCommand(DISON);
}

void setup(void)
{
P1DIR |= BIT4 + BIT5 + BIT6 + BIT7;
USICTL0 |= USISWRST;
USICTL0 = USIPE6 + USIPE5 + USIMST + USIOE + USIGE;
//USICTL1 = USICKPH;
USICKCTL = USIDIV0 + USICKPL + USISSEL1;
USICNT = 0x09 + USI16B;
USICTL0 =USICTL0 & ~USISWRST;
}


//SMCLK setup
//RSEL = 0x07;	//frequency range	@PUC RSEL = 0x07 DCO = 0x04: 1.1MHz
//DCO = 0x04;	//specific freq within range
