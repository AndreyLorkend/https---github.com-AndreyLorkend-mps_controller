#include <mega8535.h>

#include "lcd.h"

#include "delay.h"

#include "stdlib.h"

#asm
  .equ __lcd_port = 0x1b;
#endasm

volatile unsigned char usartRxBuf = 0;

#define RXCIE 7
#define RXEN 4
#define TXEN 3
#define URSEL 7
#define UCSZ1 2
#define UCSZ0 1
unsigned char MAXnum, buff = 0;
unsigned char i, j, n = 0;
unsigned char mas1[16];
char * str1;

void USART_Init(void) {
  UBRRH = 0;
  UBRRL = 51;
  UCSRB = (1 << RXCIE) | (1 << RXEN) | (1 << TXEN);
  UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
}
#define UDRE 5

void USART_SendChar(unsigned char sym) {
  while (!(UCSRA & (1 << UDRE)));
  UDR = sym;
}

unsigned char USART_GetChar(void) {
  unsigned char tmp;
  unsigned char saveState = SREG;
  #asm("cli");
  tmp = usartRxBuf;
  usartRxBuf = 0;
  SREG = saveState;
  return tmp;
}

interrupt[USART_RXC] void usart_rxc_my(void) {
  usartRxBuf = UDR;
  buff = usartRxBuf;
  if (buff) {
    if (i < 16) {
      mas1[i] = buff;
      i++;
    }
  }
}

void main(void) {
  lcd_init(20);
  USART_Init();
  lcd_clear();
  lcd_gotoxy(0, 0);
  #asm("sei");

  while (1) {
    if (i == 16) {
      for (i = 0; i < 16; i++) {
        if (mas1[i] % 2 == 0) {
          if (MAXnum <= mas1[i]) {
            MAXnum = mas1[i];
          }
        }
      }

      for (i = 0; i < 16; i++) {
        itoa(mas1[i], str1);
        lcd_puts(str1);
        j++;
        if (j % 4 != 0) {
          lcd_gotoxy(j * 5, n);
        } else {
          j = 0;
          n++;
          lcd_gotoxy(j * 5, n);
        }
      }
      delay_ms(1000);
      lcd_clear();
      lcd_gotoxy(0, 0);
      itoa(MAXnum, str1);
      lcd_puts(str1);
      i = 0;
      n = 0;
      j = 0;
      MAXnum = 0;
    }
  }
}