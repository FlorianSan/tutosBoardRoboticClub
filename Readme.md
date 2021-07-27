# Carte tutos club Robot

Carte électronique pour apprendre les bases de l'électronique et de la programmation au nouveaux membres du club robotique.

Cette carte devrait être suffisamment compacte et pratique pour être emportée chez soi.

Sujets:

+ LEDs
+ boutons, pullups, ...
+ potentiomètre: analogique
+ PWM
+ UART
+ I2C
+ Encoder
+ Asservissement d'un moteur


# Getting started

Le plus simple pour démarrer est d'installer VSCode, et l'extension PlateformIO.

Créer un projet pour la carte que vous utilisez (par exemple : ST Nucleo F401RE), avec le framwork Arduino.

Ajouter ceci au fichier `platformio.ini` pour activer l'UART6 qui va permettre de communiquer avec l'ATtiny1616 :

`build_flags = -D ENABLE_HWSERIAL6`


# Pinout

| Pin | Signal    |
|-----|-----------|
|PA0  |BTN1       |
|PA1  |BTN2       |
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



