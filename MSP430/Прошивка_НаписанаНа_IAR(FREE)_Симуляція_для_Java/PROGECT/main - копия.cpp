//   Baud rate divider with 1MHz = 1MHz/9600 = ~104.2
//   ACLK = n/a, MCLK = SMCLK = CALxxx_1MHZ = 1MHz

#include"msp430g2553.h"
#include <string.h>
#include <stdlib.h>
void Init_Watchdog_1MHz(void);
void init_UART(void);
void UART_print(char *string);
void UART_SendByte(unsigned char byte);
void itoa(unsigned int val, char *str, unsigned int limit);


int Get_measure();
void Init_Taimer(void);


void main( void ){
    
  Init_Watchdog_1MHz();
  init_UART();
  Init_Taimer();
  
_BIS_SR(GIE);
 
int i;

  while(1){
  i =  Get_measure();
      char itoad[16];
      itoa(i,itoad,3000);
       
   UART_print("Distance \t");
   UART_print(itoad);
   UART_print("\r\n");
     __delay_cycles(500000);
   //Indicator LED 
    P1DIR |= BIT6;
   }
     
}

void Init_Taimer(void){
  
 // Make P1.0 a digital output for the LED and P1.1
    // a digital output for the sensor trigger pulses
    P2DIR = BIT1; //0b00000011
    P2DIR = BIT2; 
 
    // Set up Timer_A1: SMCLK clock, input divider=1,
    // "continuous" mode. It counts from 0 to 65535,
    // incrementing once per clock cycle (i.e. every 1us).
    TA1CTL = TASSEL_2 + ID_0 + MC_2;
 }

int Get_measure() {
  
    int echo_pulse_duration;      // time in us
    int distance;                 // distance in cm
  
  
    P2OUT |= BIT1;
//    up = 1; //Next catch on Timer1A0 should be rising edge - helps with capture timer
    __delay_cycles(10);
    P2OUT &= ~BIT1;
    
     // Measure duration of echo pulse
    while ((P2IN & BIT2) == 0);   // Wait for start of echo pulse
    TA1R = 0;                     // Reset timer at start of pulse
    while ((P2IN & BIT2) > 0);    // Wait for end of echo pulse
    echo_pulse_duration = TA1R;   // Current timer value is pulse length
    distance = 0.017 * echo_pulse_duration; // Convert from us to cm
    return distance;              // Return distance in cm to calling function
    
}


void itoa(unsigned int val, char *str, unsigned int limit)
{
  int temploc = 0;
  int digit = 0;
  int strloc = 0;
  char tempstr[5]; //16-bit number can be at most 5 ASCII digits;
   if(val>limit)
    val %= limit;
   do
  {
    digit = val % 10;
    tempstr[temploc++] = digit + '0';
    val /= 10;
  } while (val > 0);
   // reverse the digits back into the output string
  while(temploc>0)
    str[strloc++] = tempstr[--temploc];
    str[strloc]=0;
}

void UART_print(char *string) {// Prints a string using the Timer_A UART
  while (*string)
  UART_SendByte(*string++);
 
 }

void UART_SendByte(unsigned char byte){
     while (!(IFG2 & UCA0TXIFG));      //Wait UART to finish before next send
        UCA0TXBUF = byte;


}

void Init_Watchdog_1MHz(void){
      //Зупинка сторожового таймера
  WDTCTL = WDTPW + WDTHOLD;                            // Stop watchdog timer
      //Налашування частоти роботи контролера 1.1MHZ(Є стандартно)
  DCOCTL = 0x00;                                       // Set DCOCLK to 1MHz
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL = CALDCO_1MHZ; 
}

void init_UART(void){
    P1SEL |= 0x06;                       // Use P1.1 and P1.2 as USCI_A0
    P1SEL2|= 0x06;                        // Use P1.1 and P1.2 as USCI_A0
    P1DIR |= 0x04;                          // Set 1.2 as output
    UCA0CTL1 |= UCSSEL_2;                   // Use SMCLK / DCO 
    UCA0BR0 = 104;                          // 1 MHz -> 9600   N=Clock/Baud
    UCA0BR1 = 0;                            // 1 MHz -> 9600
    UCA0MCTL = UCBRS1;                      // Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;                   // **Initialize USCI  

   
 }