#include <msp430.h>
#include <libTimer.h>
#include "state_advance.h"
#include "led.h"
#include "switches.h"
#include "buzzer.h"
#include "lcdutils.h"
#include "lcddraw.h"


#define LED BIT6 /*bit zero req'd for display */

short redrawScreen = 1; //allows the cpu to turn on 

int x = 30; /*starting positions for shape*/
int y = 50;
int vert = 60;
int horiz = 60;
char switch_state = 4; //bounds of the switches
short color[5] = {COLOR_GREEN, COLOR_RED, COLOR_BLUE, COLOR_ORANGE, COLOR_YELLOW,};
int current_color = 0;

/*Tones*/
#define E 6079
#define Fs 5420
#define Gs 4819
#define A 4545


void wdt_c_handler()
{
  static int secCount = 0;

  secCount++;
  if (secCount == 15) {/* 16/sec */
    secCount = 0;
    current_color++;
    if(current_color > 5) //Change color of the shape
      current_color = 0;
    redrawScreen = 1;
  }
}


void main()
{

  P1DIR |= LED;/**< Red led on when CPU on */
  configureClocks();
  lcd_init();
  buzzer_init();
  switch_init();
  enableWDTInterrupts();      /**< enable periodic interrupt */
  or_sr(0x8);              /**< GIE bit 3 (enable interrupts) */
  
  
  while (1) {/* forever */
    if (redrawScreen) {
      redrawScreen = 0; /*lets RETI execute*/
      
      and_sr(~8);/* disable further interrupts */
      if (switch_state < 4){
	P1OUT |= LED;
	switch(switch_state){
	case 0:/*make rectangle smaller*/
	  clearScreen(COLOR_DARK_OLIVE_GREEN);
	  fillRectangle(x, y, vert, horiz, color[current_color]);
	  if(vert > 0){
	    vert -= 4;
	    horiz-= 4;
	  }
	  buzzer_set_period(E);
	  switch_state = 4;
	  break;
	case 1: /*make rectangle bigger*/
	  clearScreen(COLOR_DARK_OLIVE_GREEN);
	  fillRectangle(x, y, vert, horiz, color[current_color]);
	  if(vert < 80){
	    vert += 4;
	    horiz+= 4;
	  }
	  buzzer_set_period(Fs);
	  switch_state = 4;
	  break;
	case 2:/*move rectangle left*/
	  clearScreen(COLOR_DARK_OLIVE_GREEN);
	  fillRectangle(x, y, vert, horiz, color[current_color]);
	  if(x > 0){
	    x -= 4;
	  }
	  buzzer_set_period(Gs);
	  switch_state = 4;
	  break;
	case 3: /*move rectangle right*/
	  clearScreen(COLOR_DARK_OLIVE_GREEN);
	  fillRectangle(x, y, vert, horiz, color[current_color]);
	  if(x < 100){
	    x += 4;
	  }
	  buzzer_set_period(A);
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
