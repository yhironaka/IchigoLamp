/**************************************************************************/
/*
** IchigoLamp 
** License: CC BY yhironaka@gmail.com
** 2015/11/02
*/
/**************************************************************************/

#include <stdio.h>
#include "main.h"
#include "LPC8xx.h"
#include "mrt.h"
#include "uart.h"

#define TXDATCTL_EOT        (1<<20)
#define TXDATCTL_RX_IGNORE  (1<<22)
#define TXDATCTL_FSIZE(s)   ((s) << 24)
#define CFG_ENABLE          (1<<0)
#define CFG_MASTER          (1<<2)
#define STAT_RXRDY          (1<<0)
#define STAT_TXRDY          (1<<1)

#define MAX_LED_NUM  128

static char g_LED_Buf[MAX_LED_NUM * 3];

void spiInit () {
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<11);
    LPC_SYSCON->PRESETCTRL &= ~(1<<0);
    LPC_SYSCON->PRESETCTRL |= (1<<0);

    // 2.4 MHz, i.e. 12 MHz / 5
    LPC_SPI0->DIV = 4;
    LPC_SPI0->DLY = 0;

    LPC_SPI0->TXCTRL = TXDATCTL_FSIZE(12-1) | TXDATCTL_RX_IGNORE;

    LPC_SPI0->CFG = CFG_MASTER;
    LPC_SPI0->CFG |= CFG_ENABLE;
}

static void spiSend (uint16_t cmd) {
    while ((LPC_SPI0->STAT & STAT_TXRDY) == 0);
    LPC_SPI0->TXDAT = cmd;
}

static const uint16_t bits[] = {
    0b100100100100,
    0b100100100110,
    0b100100110100,
    0b100100110110,
    0b100110100100,
    0b100110100110,
    0b100110110100,
    0b100110110110,
    0b110100100100,
    0b110100100110,
    0b110100110100,
    0b110100110110,
    0b110110100100,
    0b110110100110,
    0b110110110100,
    0b110110110110,
};

static void sendByte (int value) {
    spiSend(bits[value >> 4]);
    spiSend(bits[value & 0xF]);
}

static void sendRGB (int r, int g, int b) {
    sendByte(g);
    sendByte(r);
    sendByte(b);
}

int startsWith(const char* s, const char* key) {
  int i;
  for (i = 0;; i++) {
    char c1 = s[i];
    char c2 = key[i];
    if (c2 == '\0')
      return 1;
    if (c1 == '\0')
      return 0;
    if (c1 != c2)
      return 0;
  }
}

int main(void)
{
  int i;

  LPC_FLASHCTRL->FLASHCFG = 0;            // 1 wait state instead of 1
  SwitchMatrix_Init();
  spiInit();
  uart0Init(115200);
  mrtInit(__SYSTEM_CLOCK/1000);

  uart0puts("IchigoLamp Ver.");
  uart0puts(VERSION_NUM);
  uart0puts("\n\r");

    for(i=64;i>=0;i--) {
      spiSend(0);
      spiSend(0);

      sendRGB(i,i,i);
      sendRGB(i,i,i);
      sendRGB(i,i,i);
      sendRGB(i,i,i);
      sendRGB(i,i,i);
      sendRGB(i,i,i);
      sendRGB(i,i,i);
      sendRGB(i,i,i);
      mrtDelay(5);
    }

  char readbuf[10];
  char c;
  int bufpos;
  int out_bufpos;
  while(1)
  {
    // LEDLAMP待ち
    bufpos=0;
    for(i = 0 ; i < 10 ; i++) readbuf[i]=0;
    while(1)
    {
      if(!uart0test()) continue;

      c = uart0read();
      readbuf[bufpos]=c;
      if (startsWith("LEDLAMP ",readbuf)){
        if(bufpos >= 7) break;
        bufpos++;
      }
      else if (startsWith("LL ",readbuf)){
        if(bufpos >= 2) break;
        bufpos++;
      }
      else {
        uart0puts(readbuf);
        for(i = 0 ; i < 10 ; i++) readbuf[i] = 0;
        bufpos = 0; 
      }
    }
  
    // デコード
    bufpos = 0;
    out_bufpos=0;
    for(i = 0 ; i < 10 ; i++) readbuf[i]=0;
    while(1)
    {
      if(!uart0test()) continue;

      c = uart0read();
      if(c == 0x0d || c == 0x0a) break;
      if(c >='0' && c <='9') {
        readbuf[bufpos] = c - '0';
        bufpos++;
      }
      if(c >='A' && c <='F') {
        readbuf[bufpos] = c - 'A' + 10;
        bufpos++;
      }
      if(c >='a' && c <='f') {
        readbuf[bufpos] = c - 'a' + 10;
        bufpos++;
      }
      if(bufpos == 6) {
        for(i = 0 ; i < 3 ; i++){
          g_LED_Buf[out_bufpos + i] =  readbuf[i * 2] * 16;
          g_LED_Buf[out_bufpos + i] += readbuf[i * 2 + 1];
        }
        out_bufpos += 3;
        bufpos=0;
        if (out_bufpos >= MAX_LED_NUM * 3) break;
      }
    }
    spiSend(0);
    spiSend(0);
    for (i = 0 ; i < out_bufpos ; i += 3) {
      sendRGB(g_LED_Buf[i] , g_LED_Buf[i + 1] , g_LED_Buf[i + 2]);
    }
  }
}
