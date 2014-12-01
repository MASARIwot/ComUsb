#include "msp430g2231.h"
#define UART_TXD 0x02              // TXD on P1.1 (Timer0_A.OUT0)..0x02 --> BIT1 
#define A4     BIT4                // Вхід для АЦП
#define UART_TBIT (1000000 / 9600) // 9600 Baud, SMCLK = 1MHz
                                   // Globals for transmit UART communication
unsigned int txData; // UART internal variable for TX  
//-------------------------Function//-------------------------------
//Оголошення функцій
void Init_Watchdog_1MHz(void);                           
void Iniy_Pin(void);   
void Init_TaimerA_For_UART_transmit(void);
void TimerA_UART_tx(unsigned char byte);               
void TimerA_UART_print(char *string);
void ADC_init(void);
void itoa(unsigned int val, char *str, unsigned int limit);
//---------------------------Main----------------------------------- */
void main(void){
  
 int adcValue; //Змінна для збереження отриманого коду з АЦП 
              //Ініціалізація функ-й
  Init_Watchdog_1MHz(); 
  Iniy_Pin();
  Init_TaimerA_For_UART_transmit();
  ADC_init();
  
  _BIS_SR(GIE);                            // Enable CPU interrupts


  for(;;){ 
   P1DIR  |= BIT0 + BIT6;
   P1OUT ^=  BIT6+ BIT0;
    
  __delay_cycles(500000);//  затримка 1ск
  __delay_cycles(500000);
          //дозволяємо перетворення та вибірку
    ADC10CTL0 |= ENC + ADC10SC;
          //ENC -  перетворееня
          //ADC10SC - програмний запуск/зупика процесу виборки 
          // очікуємо доки закінчеться перетворення
    while ((ADC10CTL1 & ADC10BUSY) == 0x01); 
    adcValue=ADC10MEM;  // зберігаємо результат
          //забороняємо перетворення та вибірку
    ADC10CTL0 ^= ENC + ADC10SC;
 //--------------convert Int--------------------
          //ковертуємо результат  Int--char
  char itoad[16];
  itoa(adcValue,itoad,30000);
  //---------------send int--------------------- 
          //посилаємо значення по ком порту
  TimerA_UART_print(itoad); 
          //щоб числа не прийшли у вигля потяга..розділимо їх таким посилом
  TimerA_UART_print("\r\n");
          //затримка 0,25ск
 __delay_cycles(250000); 

  }
}
//---------------------------Functiuo----------------------------------- 
//Реалізація ф-ї itoa оскільки IAR не має деяких бібліотек С
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
    //Регістр управління 1-го модуля АЦП
    ADC10CTL1 = INCH_4 + ADC10DIV_3 ;//+ CONSEQ_1; 
        // INCH_4 - вибір вхідного каналу А4 --Р1.4--BIT4
        // ADC10DIV_3 - Коефіцієнт ділення тактового сигналу..010-3
    //Регістр управління 0-го могдуля АЦП    
   ADC10CTL0 = SREF_0 + ADC10SHT_3 +  ADC10ON;//REFON Ref voltage/sample&hold 
        // SREF_0 - Джерело опорної напруги Vr+=Vcc.Vr-=Vss
        // ADC10SHT_3 - Час виборки 64такта ADC10CLK
        // REFON - Вмикання генератора опорної напруги
        // ADC10ON - Вмикання модуля ADC10
   
 } // ADC_init*/

void Init_Watchdog_1MHz(void){
      //Зупинка сторожового таймера
  WDTCTL = WDTPW + WDTHOLD;                            // Stop watchdog timer
      //Налашування частоти роботи контролера 1.1MHZ(Є стандартно)
  DCOCTL = 0x00;                                       // Set DCOCLK to 1MHz
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL = CALDCO_1MHZ; 
}

void Iniy_Pin(void){
    //налаштування портів
  P1OUT = UART_TXD;                              // Initialize P1.1
  P1SEL = UART_TXD;                              // Timer function for TXD pin
  P1DIR = UART_TXD;                              // Set TXD pin to output
}

void Init_TaimerA_For_UART_transmit(void){
       //TA0CCTLх - регістр управлінн блоком захвата зрівняння
    TA0CCTL0 = OUT; // Timer_A for transmit UART operation
      // OUT - стан виходу Високий/низький рівень
      // Set TXD Idle as Mark = '1'
  TA0CCTL1 = SCS + CM1 + CAP;   // Sync, Neg Edge, Capture
      /* SCS - Синхронізація захоплення з тактовим сигналом таймера 
      Асинхронний/Синхронний  */
      // CM1 - Зежим захоплення 1- Захоплення по наростаючому фронту
      // CAP - Режим роботи блоку захвата 0/ зрівняння 1
    // TA0CTL - Регістр управління таймером А
    TA0CTL = TASSEL_2 + MC_2;            // SMCLK, start in continuous mode
        // TASSEL_2 - Джерело тактового сигналу 2- SMCLK
        // MC_2 - Режим Таймера рахує до і від)
}

void TimerA_UART_tx(unsigned char byte){//Outputs one byte using theTimer_AUART

  while (TACCTL0 & CCIE);                           // Ensure last char got TX'd
  TA0CCR0 = TAR;                                 // Current state of TA counter
      // TA0CCR0 - регістр захвата /зрівняння
  TA0CCR0 += UART_TBIT;                          // One bit time till first bit
  //Добавляємо старт і стоп біти(в байт для відправки) 
  txData = byte;                       // Load transmit data, e.g. 'A'=01000001
  txData |= 0x100;                         // Add mark stop bit, e.g. 101000001
  txData <<= 1;                          // Add space start bit, e.g. 1010000010
  TA0CCTL0 = OUTMOD0 + CCIE;             //Set TXD on, enable counter interrupt
      // OUTMOD0 - стан біту OUT
     //  CCIE - дозвіл переривань захват/зрівняння 1-дозволено 0-заборонено
}
//Функція передачі даних по програмному уарту
void TimerA_UART_print(char *string) {// Prints a string using the Timer_A UART
  while (*string)
    TimerA_UART_tx(*string++);
  
 }

//---------------------------------Interrupt----------------------------------------------
#pragma vector = TIMER0_A0_VECTOR               // Timer_A UART - Transmit ISR
   __interrupt void Timer_A0_ISR(void) {
  static unsigned char txBitCnt = 10;
  TA0CCR0 += UART_TBIT;                             // Add Offset to CCRx
  if (txBitCnt == 0) {                              // All bits TXed?
    TA0CCTL0 &= ~CCIE;                       // All bits TXed, disable interrupt
    txBitCnt = 10; //8+2 (байт+старт+стоп біт)       // Re-load bit counter
  }
  else {
    if (txData & 0x01)
      TA0CCTL0 &= ~OUTMOD2;                            // TX Mark '1'
    else
      TA0CCTL0 |= OUTMOD2;                             // TX Space '0'
  }
  txData >>= 1;                       //Shift right 1 bit (low bits TX'ed first)
  txBitCnt--;
}