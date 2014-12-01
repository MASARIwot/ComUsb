//   Baud rate divider with 1MHz = 1MHz/9600 = ~104.2
//   ACLK = n/a, MCLK = SMCLK = CALxxx_1MHZ = 1MHz

#include"msp430g2553.h"
#include <string.h>
#include <stdlib.h>

//------------Оголошення ф-й-------------------------

void Init_Watchdog_1MHz(void);
void init_UART(void);
void UART_print(char *string);
void UART_SendByte(unsigned char byte);
void itoa(unsigned int val, char *str, unsigned int limit);
int Get_measure();
void Init_Taimer(void);

//--------------Основна програма-------------------------
void main( void ){
 // налаштування   
  Init_Watchdog_1MHz();
  init_UART();
  Init_Taimer();
  //заборона усіх переривань процесора
_BIS_SR(GIE);

int i;
  while(1){
//отримуємо відстань    
      i =  Get_measure();
//конвертуємо результат      
      char itoad[16];
      itoa(i,itoad,65535);
//відправляємо значення по УАРТ на ПС  
   UART_print(itoad);
   UART_print("\r\n");
//затримка   
    __delay_cycles(500);
   //Indicator LED 
    P1DIR ^= BIT6;
   }
}
//----------------Реалізація Функцій-------------------------
void Init_Taimer(void){
//налаштування портів для відправки імпульсу
    P2DIR = BIT1; 
//для мигання лампочки   
    P1DIR  = BIT4;
//  TA1CCTL0 = CCIE; //дозвіл переривань
    TA1CTL = TASSEL_2 + ID_0 + MC_2;
    // TASSEL_2 вибір джерела тактового сигналу SMCLK
    // ID_0 Коефіцієнт ділення тактового сигналу 0
    // MC_2 Режим таймера.Безперервна лічба до 0ffffh(0 to 65535)(i.e. every 1us).
}
///
int Get_measure(){              //вимірьвання відстані
 
    int echo_pulse_duration;      // time in us
    int distance;                 // distance in cm
  
  //ставимо високий рівень на вихід Р2.1 =1
    P2OUT |= BIT1;
   //робимо затримку 10мс
    __delay_cycles(10);
   //Р2.1 = 0
    P2OUT &= ~BIT1;
   // Measure duration of echo pulse
    while ((P2IN & BIT2) == 0);   // очікуємо echo pulse
    TA1R = 0;                     // обнуляємо таймер
    while ((P2IN & BIT2) > 0);    // очікуємо закінчення echo pulse
    echo_pulse_duration = TA1R;   // записуємо поточне значення таймера
    distance = 0.017*echo_pulse_duration; // конвертуємо в см 
    //якщо відстань менша 30см то запалюємо світодіод на ніжкі Р1.4
     if( distance <= 30 ){
        P1OUT  |= BIT4;
     }else {P1OUT &=~ BIT4;}
    return distance;              // повертаємо результат
}
//------ф-я для переведення Int в Char
void itoa(unsigned int val, char *str, unsigned int limit){
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

void UART_print(char *string) { 
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
//стандартне/базове налаштування
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

/*#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
  
  Buf +=65535;
  
}*/

