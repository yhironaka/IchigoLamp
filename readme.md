IchigoLamp
==========
yhironaka@gmail.com 廣中靖久

<img src="https://raw.githubusercontent.com/yhironaka/yhironaka.github.io/master/images/IchigoLamp.jpg" width="480px">  
https://www.youtube.com/watch?v=XWhrrqIixSc

IchigoJamにuartで接続し、マイコン入りシリアルLEDのWS2812/WS2812Bを任意の色で光らせます。

### PinList
PinNo. | PinName | Signal | Module
----|----|----|----
 1 | RESET/PIO0_5          |  RESET     |  SYSTEM
 2 | PIO0_4                |  U0_TXD    |  USART0
 3 | SWCLK/PIO0_3          |  SPI0_MOSI |  SPI0
 4 | SWDIO/PIO0_2          |  SWDIO     |  SYSTEM
 5 | PIO0_1/ACMP_I2/CLKIN  |  PIO0_1    |  GPIO0
 6 | VDD                   |            |  
 7 | VSS                   |            |  
 8 | PIO0_0/ACMP_I1        |  U0_RXD    |  USART0

3番ピンが出力です。WS2812のDINに接続して下さい。
LPC810は電源が3.3Vです。WS2812は5Vです。ダイオード等の順方向電圧で調整するのが簡単です。

### コマンド

uartから以下のコマンドを入力すると、LEDが点灯します。
「LEDLAMP」、「LL」に続けて16進数でRGBの順に値を記述します。
複数個のLEDをつなげている場合、LPC810に近い方から順に色を指定します。

ex) LEDLAMP FF0000 00FF00 0000FF FFFFFF

改行のタイミングで点灯します。

### 参考

#### LPC810のコードベース
microbuilder/LPC810_CodeBase
https://github.com/microbuilder/LPC810_CodeBase

#### LPC810からWS2812を制御するモジュール
LEDs racing around a DIP
http://jeelabs.org/book/1450d/
https://github.com/jeelabs/embello/tree/master/explore/1450-dips/leds
