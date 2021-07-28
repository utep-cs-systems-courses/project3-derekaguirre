#include <msp430.h>
#include <libTimer.h>
#include "state_advance.h"
#include "led.h"
#include "switches.h"
#include "buzzer.h"
#include "lcdutils.h"
#include "lcddraw.h"


#define LED BIT6 /*bit zero req'd for display */

short redrawScreen = 1;

int x = 50;
int y = 50;
char switch_state = 4; //see the changes
short color[5] = {COLOR_GREEN, COLOR_RED, COLOR_BLUE, COLOR_ORANGE, COLOR_YELLOW};
int current_color = 0;

/*Tones*/
#define E 6079
#define Fs 5420
#define Gs 4819
#define A 4545
#define B 4056



void wdt_c_handler()
{
  static int secCount = 0;

  secCount++;
  if (secCount == 31) {/* 8/sec */
    secCount = 0;
    current_color++;
    if(current_color > 5) //Change color of the shape
      current_color = 0;
    redrawScreen = 1;
  }
}


void main()
{

  P1DIR |= LED;/**< Green led on when CPU on */
  configureClocks();
  lcd_init();
  enableWDTInterrupts();      /**< enable periodic interrupt */
  or_sr(0x8);              /**< GIE (enable interrupts) */

  switch_init();
  clearScreen(COLOR_GOLD);
  while (1) {/* forever */
    if (redrawScreen) {
      redrawScreen = 0; /*redrawScreen = 0*/
      
      and_sr(~8);/* disable interrupts */
      if (switch_state != 4){
	P1OUT |= LED;
	switch(switch_state){
	case 0:
	  buzzer_set_period(1000);
	  switch_state = 4;
	  break;
	case 1:
	  clearScreen(COLOR_YELLOW);
	  fillRectangle(x, y, 60, 60, color[current_color]);
	  buzzer_set_period(500);
	  x = x - 4;
	  switch_state = 4;
	  break;
	case 2:
	  clearScreen(COLOR_WHITE);
	  fillRectangle(x, y, 60, 60, color[current_color]);
	  buzzer_set_period(3000);
	  x = x + 4;
	  switch_state = 4;
	  break;
	case 3:
	  clearScreen(COLOR_PURPLE);
	  buzzer_set_period(0);
	  switch_state = 4;
	  break; 
	}
      }
      else{
	P1OUT &= ~LED;/* led off */
	buzzer_set_period(0);
      }
      or_sr(8);/* enable interrupts */
    }
    or_sr(0x10);/**< CPU OFF */
  }
}


void
__interrupt_vec(PORT2_VECTOR) Port_2(){
  if (P2IFG & SWITCHES) {      /* did a button cause this interrupt? */
    P2IFG &= ~SWITCHES;      /* clear pending sw interrupts */
    switch_interrupt_handler();/* single handler for all switches */
  }
}
