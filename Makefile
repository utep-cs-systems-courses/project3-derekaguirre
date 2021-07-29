# makefile configuration
COMMON_OBJECTS  = 
CPU             = msp430g2553
CFLAGS          = -mmcu=${CPU} -Os -I../h
LDFLAGS		= -L../lib -L/opt/ti/msp430_gcc/include


#switch the compiler (for the internal make rules)
CC              = msp430-elf-gcc
AS              = msp430-elf-gcc -mmcu=${CPU} -c

all: project.elf

#additional rules for files
project.elf:  ${COMMON_OBJECTS} wdt_handler.o buzzer.o switches.o projectMain.o led.o state_advance.s
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ $^ -lTimer -lLcd

load: project.elf 
	  msp430loader.sh project.elf

clean:
	rm -f *.o *.elf *.*~ *.*# *~ *#
