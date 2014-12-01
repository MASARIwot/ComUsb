#include "msp430g2231.h"
#define UART_TXD 0x02                                  // TXD on P1.1 (Timer0_A.OUT0)..0x02 --> BIT1 
#define A4     BIT4
#define CONVERSION_VALUE          0.003222
#define UART_TBIT (1000000 / 9600)                     // 9600 Baud, SMCLK = 1MHz
                                                       // Globals for transmit UART communication
unsigned int txData;                                   // UART internal variable for TX  
//-------------------------Function//-------------------------------
void Init_Watchdog_1MHz(void);                           
void Iniy_Pin(void);   
void Init_TaimerA_For_UART_transmit(void);
void TimerA_UART_tx(unsigned char byte);               
void TimerA_UART_print(char *string);
void ADC_init(void);
void itoa(unsigned int val, char *str, unsigned int limit);
//---------------------------Main----------------------------------- */
void main(void){
 int adcValue;

  Init_Watchdog_1MHz();
  Iniy_Pin();
  Init_TaimerA_For_UART_transmit();
  ADC_init();
  
  _BIS_SR(GIE);                                // Enable CPU interrupts


  for(;;){ 
   P1DIR  |= BIT0 + BIT6;
   P1OUT ^=  BIT6+ BIT0;
 //   __delay_cycles(500000);  
    ADC10CTL0 |= ENC + ADC10SC;
    while ((ADC10CTL1 & ADC10BUSY) == 0x01); // ждем, когда преобразование закончитс€*/
    adcValue=ADC10MEM;  // конвертируем результат в напр€жение и сохран€ем 3,2
    ADC10CTL0 ^= ENC + ADC10SC;
 //--------------convert Int--------------------
  char itoad[16];
  itoa(adcValue,itoad,3000);
  //---------------send int--------------------- 
  TimerA_UART_print(itoad); 
  TimerA_UART_print("\r\n");
 __delay_cycles(250000);  

}
}
//---------------------------Functiuo----------------------------------- 
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
//===================================================================*/
void ADC_init(void) {
    ADC10CTL1 = INCH_4 + ADC10DIV_3 ;//+ CONSEQ_1;                     
    ADC10CTL0 = SREF_0 + ADC10SHT_3 + REFON + ADC10ON;    // Ref voltage/sample & hold time/    
 } // ADC_init*/

void Init_Watchdog_1MHz(void){
  WDTCTL = WDTPW + WDTHOLD;                            // Stop watchdog timer
  DCOCTL = 0x00;                                       // Set DCOCLK to 1MHz
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL = CALDCO_1MHZ; 
}

void Iniy_Pin(void){
  
  P1OUT = UART_TXD;                                    // Initialize P1.1
  P1SEL = UART_TXD;                                    // Timer function for TXD pin
  P1DIR = UART_TXD;                                    // Set TXD pin to output
}

void Init_TaimerA_For_UART_transmit(void){
                                                       // Timer_A for transmit UART operation
  TA0CCTL0 = OUT;                                      // Set TXD Idle as Mark = '1'
  TA0CCTL1 = SCS + CM1 + CAP;                          // Sync, Neg Edge, Capture
  TA0CTL = TASSEL_2 + MC_2;                            // SMCLK, start in continuous mode
}

void TimerA_UART_tx(unsigned char byte) {              // Outputs one byte using the Timer_A UART

  while (TACCTL0 & CCIE);                              // Ensure last char got TX'd
  TA0CCR0 = TAR;                                       // Current state of TA counter
  TA0CCR0 += UART_TBIT;                                // One bit time till first bit
  txData = byte;                                       // Load transmit data, e.g. 'A'=01000001
  txData |= 0x100;                                     // Add mark stop bit, e.g. 101000001
  txData <<= 1;                                        // Add space start bit, e.g. 1010000010
  TA0CCTL0 = OUTMOD0 + CCIE;                           // Set TXD on, enable counter interrupt
}

void TimerA_UART_print(char *string) {                 // Prints a string using the Timer_A UART
  while (*string)
    TimerA_UART_tx(*string++);
  
 }

//---------------------------------Interrupt----------------------------------------------
#pragma vector = TIMER0_A0_VECTOR                      // Timer_A UART - Transmit ISR
   __interrupt void Timer_A0_ISR(void) {
  static unsigned char txBitCnt = 10;
  TA0CCR0 += UART_TBIT;                                // Add Offset to CCRx
  if (txBitCnt == 0) {                                 // All bits TXed?
    TA0CCTL0 &= ~CCIE;                                 // All bits TXed, disable interrupt
    txBitCnt = 10;                                     // Re-load bit counter
  }
  else {
    if (txData & 0x01)
      TA0CCTL0 &= ~OUTMOD2;                            // TX Mark '1'
    else
      TA0CCTL0 |= OUTMOD2;                             // TX Space '0'
  }
  txData >>= 1;                                        // Shift right 1 bit (low bits TX'ed first)
  txBitCnt--;
}