#include <msp430x14x.h>
#include "lcd.h"
#include "portyLcd.h"
#include "lib.h"

/** \brief Main fuction of the program
 *
 * \param void
 * \return void
 *
 */
void main( void ){
  WDTCTL = WDTPW + WDTHOLD;

  unsigned int i = 0;
  
  InitPortsLcd();
  InitLCD();
  clearDisplay();

  BCSCTL1 |= XTS;

  do{
      IFG1 &= ~OFIFG;
      for (i = 0xFF; i > 0; i--);
  }while ((IFG1 & OFIFG) == OFIFG);

  BCSCTL1 |= DIVA_3;
  BCSCTL2 |= SELM0 | SELM1;

  TACTL = TASSEL_1 + MC_1 + ID_1;
  CCTL0 = CCIE;
  CCR0 = 5000;
  _EINT();

  initChars();
  initLogic(1);

  printMenu();

  for(;;){
    menu();
  }
}