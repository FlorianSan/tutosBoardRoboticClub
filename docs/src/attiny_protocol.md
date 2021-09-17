# Attiny protocol

The Attiny1616 featured in the tuto board controls 9 leds, and has 2 inputs : a potentiometer and a phototransistor, acting as light sensor.

The protocol documented here allows you to interact with the Attiny.

Packets sent by the nucleo to the Attiny will be called **Tx packets**. Those sent by the Attiny to the nucleo will be called **Rx packets**.


## Message format

All messages have the same format :


<table>
<tr>
<td>STX</td>
<td>LEN</td>
<td>PAYLOAD[...]</td>
<td>CHK</td>
</tr>
</table>



**STX**  
Start of the packet. STX=0xFF.

**LEN**  
Byte size of the PAYLOAD and CHK field.

**PAYLOAD**  
See Tx packets and Rx Packets.

**CHK**  
Checksum for the packet.
CHK = ~(LEN + PAYLOAD[0] + … + PAYLOAD[N])


## Tx packets

Payload of Tx packets is made of a 1 byte instruction optionally followed by parameters.

<table>
<tr>
<td>STX</td>
<td>LEN</td>
<td>INSTRCTION</td>
<td>PARAM 1</td>
<td>...</td>
<td>PARAM N</td>
<td>CHK</td>
</tr>
</table>

The instructions are:

+ **PING**  
  Ping the Attiny.  
  + Value: **0x01**
  + Parameters: no parameters
  + Answer: PONG message.

+ **READ**  
  Request the values of *count* registers, starting at *address*.  
  + Value: **0x02**
  + Parameters: Address (1byte), count (1byte)
  + Answer: REG message.

+ **WRITE**  
  Write the *values* at registers starting at *address*. The register address is auto-incremented.  
  + Value: **0x03**
  + Parameters: address, value1, value2, …
  + Answer: no answer

## Rx packets
Payload of Rx packets is made of a 1 byte identifier optionally followed by data.

+ **PONG**
  + Value: 0x11
  + Data: no data

+ **REG**
  + Value: 0x12
  + Data: values

+ **CONTINUOUS**
  + Value: 0x13
  + Data: 2 bytes defined as follow:

<table>
<tr>
<td>data_id[1:0]</td>
<td bgcolor="gray">RESERVED</td>
<td>data[9:8]</td>
</tr>
<tr>
<td colspan="3" style="text-align:center">data[7:0]</td>
</tr>
</table>


Where data_id identify the nature of the data:

+ 0x0: potentiometer value
+ 0x1: light value
+ 0x2: leds status

The data itself spans over 10 bits for potentiometer and light values, and 9 bits for leds status.

## Registers description

|Name   |Address|Access|
|-------|-------|------|
|POT_L  |0X00   |R     |
|POT_H  |0X01   |R     |
|LIGHT_L|0X02   |R     |
|LIGHT_H|0X03   |R     |
|LEDS_L |0X04   |RW    |
|LEDS_H |0X05   |RW    |
|MODE   |0X06   |RW    |



### POT

10 bits potentiometer value.

<div>
    <div style="width: 60px; float: left;">
        POT_H<br/>
        POT_L
    </div>
    <div>
        <table>
        <tr>
        <td bgcolor="gray">RESERVED</td>
        <td>POT[9:8]</td>
        </tr>
        <tr>
        <td colspan="2" style="text-align:center">POT[7:0]</td>
        </tr>
        </table>
     </div>
</div>


### LIGHT

10 bits light value.

<div>
    <div style="width: 60px; float: left;">
        LIGHT_H<br/>
        LIGHT_L
    </div>
    <div>
        <table>
        <tr>
        <td bgcolor="gray">RESERVED</td>
        <td>LIGHT[9:8]</td>
        </tr>
        <tr>
        <td colspan="2" style="text-align:center">LIGHT[7:0]</td>
        </tr>
        </table>
     </div>
</div>


### LEDS

State of the leds. 0 is OFF, 1 is ON.  
Default value: 0

<div>
    <div style="width: 60px; float: left;">
        LEDS_H<br/>
        LEDS_L
    </div>
    <div>
        <table>
        <tr>
        <td bgcolor="gray">RESERVED</td>
        <td>LEDS[8]</td>
        </tr>
        <tr>
        <td colspan="2" style="text-align:center">LEDS[7:0]</td>
        </tr>
        </table>
     </div>
</div>


### MODE

+ **POTC**: if set, the Attiny will send CONTINUOUS potentiometer data. **Default 0**.
+ **LIGHTC**: if set, the Attiny will send CONTINUOUS light data. **Default 0**.
+ **LEDSC**: if set, the Attiny will send CONTINUOUS leds data. **Default 0**.
+ **CFREQ**: frequency at which CONTINUOUS messages will be sent.
+ **LEDSMODE**: Leds command mode.

<table>
<tr>
<td>POTC</td>
<td>LIGHTC</td>
<td>LEDSC</td>
<td>CFREQ[1:0]</td>
<td>LEDSMODE[2:0]</td>
</table>

**bitfields values:**

<div style="display:flex;">
    <div style="flex:1;">
        CFREQ[1:0]   
        <ul>
        <li>0x0: 50ms (20Hz)</li>
        <li>0x1: 100ms (10Hz) <strong>default</strong></li>
        <li>0x2: 200ms (5Hz)</li>
        <li>0x3: 500ms (2Hz)</li>
        </ul>
    </div>
    <div style="flex:1">
        LEDSMODE[2:0]  
        <ul>
        <li>0x0: MANUAL: write to the LEDS register to command the leds.</li>
        <li>0x1: Pot value: leds reflect potentiometer value. <strong>default</strong></li>
        <li>0x2: Light value: leds reflect light value.</li>
        <li>0x3: Pot speed: not implemented</li>
        <li>0x4: Light speed: not implemented</li>
        </ul>
     </div>
</div>
