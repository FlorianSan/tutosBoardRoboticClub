
# Pinout

## Nucleo

| Pin | Signal    |
|-----|-----------|
|PA0  |BTN1 (B1)  |
|PA1  |BTN2 (B2)  |
|PA4  |BUZZ       |
|PA8  |Enc1A      |
|PA9  |Enc1B      |
|PA10 |Display_CLK|
|PA11 |UART6_TX   |
|PA12 |UART6_RX   |
|PA15 |MotB       |
|PB0  |POT        |
|PB1  |LED1       |
|PB4  |SERVO1     |
|PB5  |SERVO2     |
|PB6  |Enc2A      |
|PB7  |Enc2B      |
|PB8  |SCL1       |
|PB9  |SDA1       |
|PB10 |MotA       |
|PB12 |Attiny_link|
|PC4  |Display_DIO|
|PC8  |LED2       |
|PC9  |INT_TCA    |



Les jumpers JP1 et JP2 connectent respectivement une résistance de pullup et de pulldown aux signaux BTN1 et BTN2.


Afin d'utiliser les servomoteurs ou le moteur, il faut alimenter la carte par le connecteur jack DC en 12V (12V sur le connecteur central, GND sur l'extérieur), et régler le jumper JP5 de la nucleo (à côté du bouton RESET) sur "E5V".
La led D19 devrait s'allumer.

## TCA9554

| Pin | Signal  |
|-----|---------|
|P0   |D4       |
|P1   |D5       |
|P2   |D6       |
|P3   |D7       |
|P4   |D17      |
|P5   |D18      |
|P6   |SW6 (B3) |
|P7   |SW7 (B4) |


SW5 permet de régler un bit d'adresse (A0).

SW4 permet de connecter ou pas les pullups sur SDA et SCL.

La broche INT est connectée sur la broche PC9 de la nucleo.

Les broches d'adresses A1 et A2 sont connectés à GND.

datasheet : [http://www.ti.com/lit/ds/symlink/tca9554.pdf](http://www.ti.com/lit/ds/symlink/tca9554.pdf)

## Display

L'afficheur 7segments 4 digits est commandé par un TM1637.

datasheet : [https://www.mcielectronics.cl/website_MCI/static/documents/Datasheet_TM1637.pdf](https://www.mcielectronics.cl/website_MCI/static/documents/Datasheet_TM1637.pdf)

## Driver moteur

Le driver moteur est un BD62120AEFJ.

datasheet : [https://docs.rs-online.com/a300/0900766b816747fb.pdf](https://docs.rs-online.com/a300/0900766b816747fb.pdf)

## ATtiny 1616

**Warning** Si vous regardez ça, vous savez ce que vous faites !

| Pin | Signal    |
|-----|-----------|
|PA0  |UPDI       |
|PA2  |Attiny_link|
|PA4  |D10        |
|PA5  |D9         |
|PA6  |D8         |
|PA7  |D16        |
|PB0  |D11        |
|PB1  |D13        |
|PB2  |UART6_RX   |
|PB3  |UART6_TX   |
|PB4  |D14        |
|PB5  |D15        |
|PC0  |D12        |
|PC1  |AtPot      |
|PC2  |AtLight    |

**Warning** UART6_RX et UART6_TX sont les signaux de la nucleo, qui correspondent donc respectivement à TX et RX de l'ATtiny.

