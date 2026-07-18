# Journal #1 July 11-13: Starting the project


## What do i want to do?
I want to make my own meshtastic pager fully from scratch. It should use some radio module, have a pcb and a 3D printed case.
![JOURNAL1-IMG](screenshots/JOURNAL1-IMG4.png)

I researched about the topic a little bit and i know that i want it to have a display (propably oled), it should have a battery so maybe even a charger circuit. 
It should also have some way to type the messages: One wat is to have a like a keyboard i2c module in the pager (easier to type and do things, but takes more space)
Or i can have just few buttons and the user will select the letter he wants to type with either a rotary encoder or buttons for left or right movement of the cursor or i can have it connected to phone or laptop via bluetooth or wifi (the easiest thing if already using mcu with wifi or ble).
I have searched wich radio module i would use, and i found this one on laskakit HopeRF RFM95W 868Mhz https://www.laskakit.cz/hoperf-rfm95w-868mhz-komunikacni-modul/.

![JOURNAL1-IMG](screenshots/JOURNAL1-IMG1.png)

This one is okay for my task. It is in the correct frequency in eu and has range of hundred meters to kilometers, depending on conditions.

## The MCU
For the MCU i can choose between a esp32 s3 board, wich is already soldered and i will plug it into a female headers or solder, or i can choose just the chip and add things like resistors capacitors myself (harder to do but saves space), the problem here is that it is hard to solder and i dont have the tools to solder by hand, i could try but it is risky.
![JOURNAL1-IMG](screenshots/JOURNAL1-IMG2.png)
![JOURNAL1-IMG](screenshots/JOURNAL1-IMG3.png)

Next i will create a kicad project and try to decide wich options to go with.
### Time spent: 2 hours


# Journal #2 July 13-15: 

![JOURNAL2-IMG](screenshots/JOURNAL2-IMG3.png)

## Chosing the MCU
So after some time thinking and researching, i think i will choose the esp32 s3 wroom 1 MCU, not the devkit one, because it is smaller and cheaper and saves more space. The soldering shoudlnt be a problem, because i think i will use the jlcpcb soldering service, so i dont have to deal with all the small smd components, which i dont have the tools to solder myself.

## The RF ciruit
I have found a smd HopeRF RFM95W 868Mhz schematic and pretty file in kicad, i had to download it from github, so after some searching i imported that. Then i found the esp32 s3 wroom 1 pinut, and connected the hopeRF module pins to the MCU. Some pins like DIO2 to DIO5 can be left unconnected. Then later i also realized that i need pullup resistors to nss and reset. I also added some capacitors to the VDD pin (one with bigger capacitance and one with smaller). Then there is the antena pin. I had researched a little bit, and i have few options:
1. Connect just a wire with a specific length (like 8,2 cm?) to the antena pin directly, jusy a wire.
2. Make a short trace on pcb to a antena connector, then use swapabble antenas.
3. Make the antena on pcb (i have looked into that, but it is pretty hard rf engineering and there are a lot of things to mess up).

I have chose option 2, it is the easiest and with most range, efficency and you can also swap different antenas.

### The connector
I have chosen the coaxial connector, it has two pins:
1. ANT (antena pin).
2. GND.

I have chosen this footprint: `Connector_Coaxial:SMA_Amphenol_132134-11_Vertical`
It is easy to handsolder, and you can mount the antena directly to it. I first had a horizontal one, but you have to have that one on the side of pcb and that is limiting.


![JOURNAL2-IMG](screenshots/JOURNAL2-IMG5.png)



## Power Circuit
I need some power circuit, so i can use usb to power, and flash the code. First i needed to decide wich usb to choose. USB-C is modern, but has 24 small pins, which is hard for hand solder, that is why i chose micro-USB, which only has 5. I connected:
- VBUS to a LDO, which converts to 3.3V.
- D+ to D+ pin on esp32 s3 (it has usb in the module, no additional chips).
- D- to D- pin on esp32 s3.
- GND to GND.
- SH to GND (the shield can be connected to gnd for hobby projects).

I also have some capacitors for noise and power spike filtering.
Then later i realized i need esd protection, so i added `USBLC6-2SC6` esd to it and conneced it to it.
the 3.3V rail then powers the MCU, RF module and other things. 

![JOURNAL2-IMG](screenshots/JOURNAL2-IMG4.png)

## Display
I want a display, where you can see the messages clearly and has good colors vizually, it should also be some size, so you can read the messages. After some time, i have chosen a tft display SPI 1.9 inches. I think it is okay size for this project, maybe i will chose i bigger one, for now i dont know. It has 8 pins, so i will buy the module and connect to the pcb using dupont pins. I routed the pins to the MCU.

![JOURNAL2-IMG](screenshots/JOURNAL2-IMG6.png)

## Problem with the pins
After routing Display pins to MCU, there is problem, the SPI pins overlap with the RF module, so i chose another spi pins (gpio 37- 39). But problem is that these pins are reserved for Octal PSRAM. SO i can either:
1. Choose a module without Octal PSRAM.
2. Relocate the display SPI pins elsewhere.

I relocated them elsewhere, because the Octal PSRAM may be needed for the software, since i am doing pager with UI.


![JOURNAL2-IMG](screenshots/JOURNAL2-IMG1.png)

![JOURNAL2-IMG](screenshots/JOURNAL2-IMG2.png)

## EN and BOOT missing problem
I had forgot to add the capacitors and resistors to EN and BOOT pin on the MCU, without these, i will not be able to flash my firmware. I also added two buttons so i can activate these pins.

## What do i want to do next?
- I want to add a battery circuit to the schematic, maybe some charging like the tp modules.
- Add some things for control, like buttons, rotary encoders, maybe keyboard?
- Maybe add like a vibration motor, so the user can know, when a message arrives.
- I also am thinking about adding rtc, so the time stays correct, even when turned off.

### Time Spent: 6 Hours

# Journal #3 July 15:  Battery charging, protection circuit, buzzer, vibration motor, gps, compas, debugging, cardKB, memory lcd

![JOURNAL3-IMG](screenshots/JOURNAL3-IMG3.png)

## Display
I think i dont want a tft display, sure they are multicolor and look good, but they take a lot of power. I had researched different displays, and found out there are few optioins:
- Oled 
- TFT
- E-Paper
- Memory LCD
Oleds are cheap and good looking, but only few colors usually and take a lot of power and are usually small. So for this project i dont think an oled is good for it. TFT looks good, but takes too much power so no. E-Paper takes almost no power, but it takes time to refresh. Memory LCD is like e-paper, but it can refresh more times. It only refreshs when it needs to, so it saves power and takes little power. I chose Memory LCD. I found one on laskakit and i changed the gpio connections sp they match. 

![JOURNAL3-IMG](screenshots/JOURNAL3-IMG1.png)
![JOURNAL3-IMG](screenshots/JOURNAL3-IMG4.png)

## Vibration Motor
If the user recieves a message, how does he know he recieved it, that is a problem that the current schematic has. I need to implement some type of alarming system. One of those can be vibration motor. I found few online, and made a MOSFET controll schematic. It is powered from 3.3V, and only turns on when the gpio pin is pulled high through the resistor. I also have a flyback diode and some pull down resistors there.

![JOURNAL3-IMG](screenshots/JOURNAL3-IMG10.png)


## cardKB
I need to have some kind of control on the pager. I have seen designs with just normal buttons, rotary encoder + buttons or keyboards. I myself tried few, but found out there is a small, easy to connect keyboard by m5stack called cardKB. It has many keys, is small and uses only two data lines. That makes it best for my design, i can have the keys be used as a normal messanging keyboard and when not messanging, different keys can do different functions.

![JOURNAL3-IMG](screenshots/JOURNAL3-IMG12.png)

![JOURNAL3-IMG](screenshots/JOURNAL3-IMG5.png)


## GPS and Compas
I dont currently have any sensors. I looked at what other pagers sometimes have that i dont and that is:
- Barometer
- Compas
- GPS
- Tempature
- 
For now i have decided to add GPS and compas.
I found few cheap gps modules. One i connected, but he had to have external antena, and i didnt want to deal with it. Others were too pricey. Then i found GPS VG7669T160N0MA. It is not overpriced, has antena inside and only 4 pins to connect, the last two doesnt need to be connected. I connected it on kicad. I also had to find a jst footprint, because it uses only jst connector.

![JOURNAL3-IMG](screenshots/JOURNAL3-IMG7.png)

![JOURNAL3-IMG](screenshots/JOURNAL3-IMG11.png)
For compas i chose the QMC5883P. I had connected it to free esp32 s3 pins and adde dsome resistors and capacitors to it.
![JOURNAL3-IMG](screenshots/JOURNAL3-IMG8.png)

I will usethe compas and gps module in software propably like this:
- You can read, save or text your contacts your gps location (Maybe also have like a tool that can show you the saved path when you get lost?)
- When you send your contact the location, and they send it to you. You can start finding them, it would tell you how much you need to turn + how much meters away from you in that direction they are. It is not like maps, but it would work for when your lost somewhere.
- Also maybe like have pinpointed locations like where you parked your car, where is your home etc, then you can find them using the gps.

For now the gps data would be stored in flash. The esp32 s3 would have few mb left, and that is enough for now. Maybe later i will add a microSD card if needed.

# Power
The last time, the power only came from usb and the ldo. I want to add a battery charging and management circuit and when  not using usb, the battery would be used instead. This seemed easy, but was really hard. I spent like half a day working on this. It took me some time to find the components. I used:
- TP4056
- DW01A
- FS8205A
- APK2112K -3.3v

First i had some ics different, but then i came to these, like the ldo had too low maximum current. This took ma a lot of time to corretfully connect, still i dont know if it is 100% correct. The problem was: i needed the FS8205A, but it was not in kicad, i tried to find it online, but i didnt or it was paid. So then i realized it was just two mosfets together, so i tried to make it myself. But there were many errors and i spent a lot of time debugging. Then i found out that i can download it from some kicad tool, using kicad python terminal and i found the schematic and the footprint. I also added protection features to the circuit like diode, fuse etc. I also added the protection for battery (under and over charging, short, etc.) wich is done by DW01A. I also changed all gnds to prot_gnd, wich can be disconnected by the DW01A, when something goes wrong, so the battery works. And then i had a problem, the circuit woudlnt know when to switch the battery and usb power. So i built i circuit using pnp MOSFET, that switches it on when there is no microUSB, and off when there is. 



![JOURNAL3-IMG](screenshots/JOURNAL3-IMG2.png)

![JOURNAL3-IMG](screenshots/JOURNAL3-IMG9.png)

## Buzzer
For now i just have a vibration motor for notifications, that works when you are near it, but you woudlnt hear it if it is in another room. That is why i added a buzzer. It uses a transistor to switch on, has a flyback diode and can be turned on with any gpio pin. You can also modulate loudnalss wich pwm i think? Now it should alert you when you get notification or something happens.
![JOURNAL3-IMG](screenshots/JOURNAL3-IMG6.png)


### Time Spent: 10 Hours

# Journal #4 RTC and firmware July 16

## RTC (real time clock)


For now the pager has clock, but only inside the esp32 s3. When esp32 s3 is not active, the time is lost. That is why i need an rtc. It will track time even when other devices are disconnected. It almost zero current and is fairly small. I found a cheap small chip on lcsc, connected it and added resistors, capacitors and the oscillating crystal. It can be powered either from:
- Coin cell battery
- The main battery

It uses i2c, and i know i2c can be multidevice, so i connected it to the i2c pins i already, use. I also added pull-up resistors to it. 
![JOURNAL4-IMG](screenshots/JOURNAL4-IMG3.png)

![JOURNAL4-IMG](screenshots/JOURNAL4-IMG2.png)


## Temp 
I realized that the temp pin on tp4056 must be connected, else the chip woudlnt start. I connected it to voltage divider 10/10kohms from battery, so it reads around 1.8v.

![JOURNAL4-IMG](screenshots/JOURNAL4-IMG4.png)
![JOURNAL4-IMG](screenshots/JOURNAL4-IMG5.png)


## Software
Now that i have the main parts of the schematic design done, i need to move on to firmware. I wanted meshtastic communication, so i researched about meshtastic and i have few options:
- Write my own meshtastic protocol from scratch (gives me total control, but would take super much time, like thousands of lines of code).
- Copy the protocol parts of the meshtastic repo, and add my drivers and ui.
- Modify the meshtastic pinout for my board.

I will not do option 1 for now, because it would take so much time. I tried option 3: I downloaded the official repo, and modified the pins for my esp32 s3 board. That would work but problem is: I use different display totally then is supported, i would need to write my own firmware. And also i would need to navigate the massive meshtastic codebase. And i would like something like arduino ide library, where you can just copy the protocoling part, and make you own ui. But that does not exist yet. So with those options, i chose to not do meshtastic for now, and make just classic pager. Maybe i will add meshtastic later, but for now i need to test that the basic things even work.
![JOURNAL4-IMG](screenshots/JOURNAL4-IMG1.png)

## Arduino IDE
So i started writing the code. I started with importing libraries for my components.
```
#include <Wire.h>
#include <SPI.h>
#include <RadioLib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SharpMem.h>
#include <RTClib.h>  
```
I did not add a gps/compas library yet. I will first focus on basic functionality, then the gps/compas. Then i set the pins and variables i am using. I use i2c for cardKB and the rtc, that is why i have different addresses. I also set things like display resolution and variables for stopping buzzing/vibe (vibration mottor).
![JOURNAL4-IMG](screenshots/JOURNAL4-IMG8.png)

```

// I2C Pins 
#define I2C_SDA 41  
#define I2C_SCL 42

// LoRa (RFM95W)
#define RFM_NSS 8
#define RFM_RST 9
#define RFM_DIO0 14
#define RFM_DIO1 21
// SPI Pins used by RFM95: SCK=12, MOSI=11, MISO=13

// Memory LCD SPI Pins 
#define SHARP_SCK 36
#define SHARP_MOSI 35
#define SHARP_SS 34
#define DISPLAY_WIDTH 400 
#define DISPLAY_HEIGHT 240


bool vibeActive = false;
unsigned long vibeEndTime = 0;

bool buzzerActive = false;
unsigned long buzzerEndTime = 0;

#define VIB_MOTOR_PIN 3   
#define BUZZER_PIN 45  

// M5Stack CardKB I2C Address
#define CARDKB_I2C_ADDR 0x5F

SX1276 lora = new Module(RFM_NSS, RFM_DIO0, RFM_RST, RFM_DIO1);

// Sharp Memory LCD 
Adafruit_SharpMem display(SHARP_SCK, SHARP_MOSI, SHARP_SS, DISPLAY_WIDTH, DISPLAY_HEIGHT);
int current_screen1_btn_slctd = 0; // 0 for messages, 1 for location, 2 for settings

int current_screen = 0; // 0 for main menu, 1 for messages, 2 for location, 3 for settings


String message_notification = "";
// RTC 
RTC_PCF8563 rtc;

```
The variable like current screen means wich screen i currently am on (ui). I will have 4 screens for now with different purposes. The variable currentscreen1 button selected, is for ui button, which has rectangles around them, and when you are on that button, just the rectangle about that specific button will be displayed, not the other ones. Message notification is not done yet, but will be  for notifications on the homa page.

## UI screen 1
I found a free online sofrware called lopaka, you can put your display size in there and draw ui menus and then copy the code. I have made different designs, but stuck with the one, where there are three buttons:
- Messages (here will be main lora messages)
- Location (GPS + compas together, maybe some location functions)
- Settings (The settings)

I also display time date and battery percent (but i dont have the voltage divider circuit in schematic i need to add it)
![JOURNAL4-IMG](screenshots/JOURNAL4-IMG6.png)
![JOURNAL4-IMG](screenshots/JOURNAL4-IMG7.png)

The second version looks little bit better.
The code for the first screen looks like this:
```
void drawScreen_main(void) {
  DateTime now = rtc.now();

  display.clearDisplay();
  
  //Time & Battery
  display.setTextColor(1);
  display.setTextSize(2);
  display.setTextWrap(false);
  display.setCursor(265, 5);
  display.printf("Time: %02d:%02d", now.hour(), now.minute());

  display.setCursor(289, 24);
  display.print("Bat: 100%");
  
  display.setCursor(102, 61);
  display.printf("Date: %02d/%02d/%04d", now.day(), now.month(), now.year());

  // Title
  display.setTextSize(4);
  display.setCursor(11, 4);
  display.print("Lora Pager");

  
  
  // Messages Button
  display.setTextSize(3);
  display.setCursor(121, 102);
  display.print("Messages");
  if (current_screen1_btn_slctd == 0) {
    display.drawRect(115, 96, 154, 33, 1); 
  
  //Location Button
  display.setCursor(122, 151);
  display.print("Location");
  if (current_screen1_btn_slctd == 1) {
    display.drawRect(115, 144, 154, 35, 1); 
  }
  
  // Settings Button 
  display.setCursor(122, 201);
  display.print("Settings");
  if (current_screen1_btn_slctd == 2) { 
    display.drawRect(115, 194, 155, 35, 1); 
  }
  

    // string 11
    display.setTextSize(1);
    display.setCursor(10, 45);
    display.print(message_notification);

  display.refresh(); 
}

```
``` 
void loop() {
  updatePeripherals();

  // Read Keyboard 
  char key = readCardKB();
  if (key != 0) {
    Serial.print("Key Pressed: ");
    Serial.println((uint8_t)key, HEX); 
    
    switch (current_screen) {
      case 0: // Main Menu
        if (key == 0xB5) { // UP Key
       
          current_screen1_btn_slctd = (current_screen1_btn_slctd + 2) % 3;
          triggerVibe(40);
          drawScreen_main(); 
        } 
        else if (key == 0xB6) { // DOWN Key
          current_screen1_btn_slctd = (current_screen1_btn_slctd + 1) % 3;
          triggerVibe(40);
          drawScreen_main(); 
        }
        break;
    }
  }

  // Check for incoming LoRa  
  checkIncomingLoRa();
}
```
In the code i also have functions to activate buzzer or vibration motor for some specific time, also i have a function to read cardKB and to transmit or recieve lora. Next i want to make the screen for messaging and make like channels for the lora? Like channel 1 to 4 and each device tunes to the specific one.

### Time Spent: 6 Hours


# Journal 5: July 16-17



So i have started designing the ui in lopaka, i made a contacts window, but then i relized that i wont use contacts, because i dont use meshtastic and i would propably use channels. But i dont really know anything about the chanels in the 868 mhz spectrum. Then i found these legal bands and limits for them:
- K (863 MHz - 865 MHz): 0.1%, 25 mW ERP
- L (865 MHz - 868 MHz): 1%, 25 mW ERP
- M (868 MHz - 868.6 MHz): 1%, 25 mW ERP
- N (868.7 MHz - 869.2 MHz): 0.1%, 25 mW ERP
- P (869.4 MHz - 869.65 MHz): 10%, 500 mW ERP
- Q (869.7 MHz - 870 MHz): 1%, 25 mW ERP

I chose the 869.525mhz, because you have maximum 500mw (the biggest out of them) and you have a 10% duty cycle.

![JOURNAL5-IMG](screenshots/JOURNAL5-IMG1.png)

### What is duty cycle?
Duty cycle means how many percent of miliseconds can you transmit per some time like an hour. So 10$ duty cycle means that your device can trasnmit only for 6 minutes per hour. That is like 10 times bigger that the other duty cycles or even 100 times. Since i am using messages, that should be no problem If i send one message with spreading factor of 9, it will take like 180ms transmitting. So in 1 hour i can send 30 something messages. Of course it depends how long the messages are, but it should be okay. I will add a timer in code, that will not let me transmit anymore messages if i hit the limit. Also the power limit is 500mw (27dbm) And my rf module can only output 20 dbm that is like 100mw, i can use a stronger antena, but still be in the legal limit.

# Channels
So i want to make the channels, but i have two options:
1. Make different channels physically (like 868.2 and 868.3)
2. Make one channel, but in software change that the correct pager only recieves the message.

I choose option 2, since i dont have to change legal power limits each time switching a channel.

I also need to structure the messages, i created a sctruct in the code, first it sends the target chanell, then the id of the pager and then the message (max 100 characters).

```
struct PagerPacket {
  uint8_t targetChannel; // The Software Channel (like 1, 2, 3)
  uint8_t senderId; // ID of the pager sending the message
  char message[100]; // The text message (max 100 characters)
};


```
Then i also change the settings of the lora module, i use the frequency 869.525mhz. 125khz bandwidth and spreading factor of 8. Coding rate 5. I set the settings and also set power limit to 20, since 27 is maximum legal, but my module can only do 20.
```

  // Carrier Freq: 869.525 MHz, Bandwidth: 125.0 kHz, Spreading Factor: 8, Coding Rate: 5 (4/5)
  int state = lora.begin(869.525, 125.0, 8, 5); 
  
  // Set output power to maximum legal limit for this frequency (it is max 27 dBm, but my rf module has maximum 20)
  lora.setOutputPower(20);

```
Then i added the variables for each pager like the id and current chanell. The id will be only one for each module and the chanell can be changed in the message menu.
```
uint8_t myPagerId = 1; // Unique ID of this pager
uint8_t currentChannel = 1; // The channel this pager is currently tuned to
```

I also changed the sendloRaPacket function and the recieve function. It now sends the packet correctly in the format, and the recieve function filters it, if it is only on the correct chanell. Then it decodes it and displays it on the notification bar. I want to make that it displays only for a few time, like 3 seconds, or maybe until it automaticly refreshes idk. There are also error debugging functions over serial, so you can plug usb and find errors and also i have clamping for the message so it doesnt overflow the display.
```
void sendLoRaPacket(uint8_t targetChan, const char* textMsg) {
  Serial.print(F("[LoRa] Sending custom pager packet... "));
  

  PagerPacket packet;
  packet.targetChannel = targetChan;
  packet.senderId = myPagerId;
  
  // Safely copy string message into packet array (limit to 99 chars + null terminator)
  strncpy(packet.message, textMsg, sizeof(packet.message) - 1);
  packet.message[sizeof(packet.message) - 1] = '\0'; 

  // Transmit the raw bytes of the struct
  int state = lora.transmit((uint8_t*)&packet, sizeof(packet));

  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("Sent successfully!"));
  } else {
    Serial.print(F("Failed, code "));
    Serial.println(state);
  }
}


void checkIncomingLoRa() {
  PagerPacket receivedPacket;
  
  // Read incoming raw data directly into the struct format
  int state = lora.receive((uint8_t*)&receivedPacket, sizeof(receivedPacket));

  if (state == RADIOLIB_ERR_NONE) {
    
    // Filtering
    // Match current software channel or Channel 0 (System Broadcast)
    if (receivedPacket.targetChannel == currentChannel || receivedPacket.targetChannel == 0) {
      
      Serial.print(F("[LoRa] Msg received on matching channel: "));
      Serial.println(receivedPacket.targetChannel);
      
      triggerVibe(200);
      triggerBuzzer(100);
      
    String msg = String(receivedPacket.message);

    // Limit to first 20 characters
    if (msg.length() > 20) {
      msg = msg.substring(0, 20);
    }

    String message_notification =
      "Ch " + String(receivedPacket.targetChannel) +
      " From P" + String(receivedPacket.senderId) +
      ":\n" + msg;

    drawNotification(message_notification);
      
    } else {
      // Message was on a different channel. Ignore it.
      Serial.print(F("[LoRa] Ignored message for non-active channel: "));
      Serial.println(receivedPacket.targetChannel);
    }
  }
}
```

I added that the message stays on the screen until you press another key.


## Duty cycle safety limit
To comply with eu safety duty cycle limit, i created a safety filter, that doesnt let you send messages, if you use more duty cycle then you can legally. Here are the variables:
```
// Duty Cycle variables
unsigned long windowStart = 0;
unsigned long totalTxTime = 0;
const unsigned long SIX_MINUTES = 360000; 
const unsigned long MAX_TX_ALLOWANCE = 36000;
```
And here is the updated sendLoRaPacket function with the safety filter. It records the time and checks if you used your 6 minute window duty cycle. I chose 6 minute duty cycle, because 1 hour is too much to be waiting if you send all messages first minute, you have to wait whole hour, i made it that you reset it each 6 minutes
```
void sendLoRaPacket(uint8_t targetChan, const char* textMsg) {
  unsigned long currentTime = millis();

  if (currentTime - windowStart >= SIX_MINUTES) {
    windowStart = currentTime;
    totalTxTime = 0;
  }

  if (totalTxTime >= MAX_TX_ALLOWANCE) {
    Serial.println(F("[LoRa] TX Limit Reached! Wait for next 6 minute window."));
    message_notification = "TX Limit reached. Wait.";
    triggerVibe(100);
    if (current_screen == 0) {
      drawScreen_main();
    }
    return;
  }

  Serial.print(F("[LoRa] Sending packet... "));
  
  PagerPacket packet;
  packet.targetChannel = targetChan;
  packet.senderId = myPagerId;
  
  strncpy(packet.message, textMsg, sizeof(packet.message) - 1);
  packet.message[sizeof(packet.message) - 1] = '\0'; 

  unsigned long txStart = millis();

  int state = lora.transmit((uint8_t*)&packet, sizeof(packet));

  unsigned long txDuration = millis() - txStart;
  totalTxTime += txDuration;

  if (state == RADIOLIB_ERR_NONE) {
    Serial.print(F("Sent successfully! Time transmitting: "));
    Serial.print(txDuration);
    Serial.println(F(" ms"));
  } else {
    Serial.print(F("Failed, code "));
    Serial.println(state);
  }
}
```

## The message screen
So i designed the message screen. It has on the top info about current channel, battery and time. But i want to somehow be able to add text to the rectangles. I have one for typing the message and then one with message conversation. The problem is, that if i add the conversation into one single variable, it will go beyond the screen size. So i created a library that warps the text down if it hits the boundary. Then the text is displayed. Also you can change the channell with left or right button. I am using a cardKB datasheet, which tells me which key is named what in hex.

![JOURNAL5-IMG](screenshots/JOURNAL5-IMG4.png)
![JOURNAL5-IMG](screenshots/JOURNAL5-IMG5.png)
![JOURNAL5-IMG](screenshots/JOURNAL5-IMG2.png)
```
#ifndef TEXTWRAP_H
#define TEXTWRAP_H

#include <Adafruit_SharpMem.h>

inline int16_t printWrapped(Adafruit_SharpMem &d, int16_t x, int16_t y,
                             uint16_t maxW, int16_t maxY,
                             const String &text, uint8_t size = 1) {
  uint16_t maxChars = maxW / (6 * size);
  if (maxChars == 0) return y;
  d.setTextSize(size);
  for (uint16_t i = 0; i < text.length() && y + 8 * size <= maxY; i += maxChars, y += 8 * size + 2) {
    d.setCursor(x, y);
    d.print(text.substring(i, i + maxChars));
  }
  return y;
}

#endif
```

## LittleFS
I also need to store the messages locally, i can use microSD card, but that takes more viring and time so for now i will use LittleFS (the mcu built in memory where you can store memory even after power off). I use a library to manage it and save and load it each time i send or recieve message. I save the id of the sender the chanell and the message.

## Mechanical Switch
In the schematic design, i dont really have a way of turning of the battery, i can only unplug it, which is annoying when it is screwed in a case. That is why i added a mechanical switch, which can handle the current and just disables all other devices. The RTC 3.3v has its power before the switch (it always needs little power) so it doesnt lose track of time when switched off.
![JOURNAL5-IMG](screenshots/JOURNAL5-IMG3.png)


### Time Spent: 6 hours


# Journal 6 July 17 The PCB

## RF footprint error
So i have started making the pcb. I want to start with RF, because it needs the most specific conditions. But after importing the footprint i noticed an error. The RFM95W didnt have half of the pins there. It had the first 8 correct but the second half was mirroed the first eight, when there were supposed to be 8 different. This is not the official kicad library footprint, it is from some github repo so it can be with errors. 


![JOURNAL6-IMG](screenshots/JOURNAL6-IMG14.png)



I opened the footprint editor and there were just first 8 pins and on other side instead of 9 to 16, there were the same 8. So i fixed that and changed the other side to 9 to 16.

![JOURNAL6-IMG](screenshots/JOURNAL6-IMG15.png)

But that didnt work, the pins were same of them there but alot of them werent there. So then i figured there may be error in the schematic. So i opened the RFM9X schematic and looked at the pins. Some were correct but a lot of werent. So i found a official datasheet for the module and wrote the correct pins. Then i saved it and imported the footprint. 

![JOURNAL6-IMG](screenshots/JOURNAL6-IMG16.png)

That fixed one error but the pins were still mismatched. They were there but with differnt position. So i used a image of the module and matched it to the footprint.
![JOURNAL6-IMG](screenshots/JOURNAL6-IMG1.png)

Not it is correctly wired and i can move on to the designing part.

![JOURNAL6-IMG](screenshots/JOURNAL6-IMG13.png)

## ADC voltage divider.
So for now in software i have just text bat: 100%. It is just static value and i realized that i dont have any reading of battery capacity. So i used voltage dividers 100kohms and 100kohms. Which cuts the voltage in half and then the esp32 reads the voltage through its adc pin.
![JOURNAL6-IMG](screenshots/JOURNAL6-IMG12.png)


## PCB

## RTC
I made the rtc in the back layer, so it isnt that much disrupted by the power circuit noise which is mostly front. I connected the capacitors and the oscillator. But i didnt know how to connect the capacitor to the 3.3v power source, because it would need to go all way around, which adds more noise. So after trying i made using  `v` command a pin to the front layer, and crossed the chip on the front layer. That allowed me to connect it with less length and less noise.
![JOURNAL6-IMG](screenshots/JOURNAL6-IMG2.png)
![JOURNAL6-IMG](screenshots/JOURNAL6-IMG3.png)
![JOURNAL6-IMG](screenshots/JOURNAL6-IMG4.png)

## Vibration Motor

![JOURNAL6-IMG](screenshots/JOURNAL6-IMG5.png)

## The power circuit
So i somehow connected all the power circuit chips, conenctors capacitors, buttons fuses, etc. into one small mess. It took me pretty long, but it is pretty small. Maybe too small, because after making rectangles the size of the display and keyboard, i reliazed i have a lot of space.
![JOURNAL6-IMG](screenshots/JOURNAL6-IMG7.png)
![JOURNAL6-IMG](screenshots/JOURNAL6-IMG8.png)
So you can see that the bigger rectangle is the cardKB, the middle one is the screen and the small red one is the gps module. I also connected things like buzzer, the buttons for boot and en for esp32, and some connetors. I have a lot of space there so i think i will maybe distance the components from the power circuit a little bit from themselves. Also i still need to connect a lot of things, but this took me a lot of time. The antena is in the back layer, because i will choose the one which can bend, so it will go from back, then band upwards. I also moved the crystal to different locations to try to move it away from digital and analog lines, because it is pretty sensitive
![JOURNAL6-IMG](screenshots/JOURNAL6-IMG9.png)
![JOURNAL6-IMG](screenshots/JOURNAL6-IMG10.png)
![JOURNAL6-IMG](screenshots/JOURNAL6-IMG11.png)

## Software
Also in the software i have a read adc function, whihc reads the adc and then i call it in loop and update the display with the readings, for now it is linear (but battery discharge isnt). I will either keep the linear one or make nonlinear one.

```
void read_adc() {
  int rawADC = analogRead(ADC_PIN);

  float pinVoltage = (rawADC / ADC_MAX_VALUE) * V_REF;

  battery_volt = pinVoltage * 2.0;

}
```

### Time Spent: 7 Hours

# Journal 7 June 18

## GPS
I found a error in the shematic. The gps has rx and tx, but i connected gps tx to esp32 tx and the same with rx instead of gps tx to esp32 rx, that would make it that they would never talk with eachother. So i fixed that. Also There is a problem with gps. The pin 5 is on/off pin. I thought in the past the if left unconnected the module is active, but it needs to be high for the module to work. So i fixed that with tying the pin 5 to 3.3v from resistor.

![JOURNAL7-IMG](screenshots/JOURNAL7-IMG1.png)
![JOURNAL7-IMG](screenshots/JOURNAL7-IMG2.png)
![JOURNAL7-IMG](screenshots/JOURNAL7-IMG3.png)
## Circuit
Here is how the whole circuit looks now. I have connected things in the meantime, moved some of things and tried few combinations. Now most of the things are connected.
![JOURNAL7-IMG](screenshots/JOURNAL7-IMG4.png)
![JOURNAL7-IMG](screenshots/JOURNAL7-IMG5.png)
## I2C problem
Another problem. The i2c line (which has multiple devices connected to it), has multiple pull-up resistors, which could damage it and arent needed there, so i removed the additional ones and kept only one near the mcu.
![JOURNAL7-IMG](screenshots/JOURNAL7-IMG6.png)
## RF gnd zone
Now for the rf circuit. I need to connect the pins and also put something like a ground zone over it. I have heard of it but dont know what is it.
![JOURNAL7-IMG](screenshots/JOURNAL7-IMG7.png)
So i have researched about the gnd zone little bit. I created one in kicad with ctrl shift z command and set some parameters and created like a rectangle around the rf circuit on the back layer. I also need to do one for front. But after looking at some other circuits with rf, i realized that most of them have the gnd zone over all of the circuit. So i made one over all of the pcb on the front and the back layer.
![JOURNAL7-IMG](screenshots/JOURNAL7-IMG8.png)
But it didnt work, it didnt connect the grounds and for some reason looked weird. Then after debugging i found an error. I had connected all the gnds, the antena gnd and the gnds of the chip into the gnd pin, but i use prot gnd everywhere so it wouldnt complete a functional circuit. So i fixed it and then it worked.
![JOURNAL7-IMG](screenshots/JOURNAL7-IMG9.png)
Then i also noticed that the traces (gnd ones) i drew earlier were still there, it doesnt do anything negative, just looks better without them, because it is all gonna be filled with the zone pour.
![JOURNAL7-IMG](screenshots/JOURNAL7-IMG10.png)
I filled the zone with command b on both sides and now it looks like the other circuits.
![JOURNAL7-IMG](screenshots/JOURNAL7-IMG11.png)
## The power button problem
So i have the power button for manually turning off the whole circuit, but the problem is that it is under the keyboard, so it wouldnt be pressably. I can try to put it out of the keyboards range, but then it would go the 3.3v trace 100mm there and then 100mm back, so that is not efficent. I can maybe try to buy a vertical one, and make like a side button like on phones. Or i can try to make some mechanical tring, which will press the button from another point. But i dont know for now.
![JOURNAL7-IMG](screenshots/JOURNAL7-IMG12.png)
## Connecting the spi traces of rmf95w
I also connected the rf module traces to the esp32 s3, Since there were already traces there, i needed to sometimes go to the opposite layer and back. But now they are succesfully connected.
![JOURNAL7-IMG](screenshots/JOURNAL7-IMG13.png)
![JOURNAL7-IMG](screenshots/JOURNAL7-IMG14.png)
### This is how it looks in 3D:
![JOURNAL7-IMG](screenshots/JOURNAL7-IMG15.png)
![JOURNAL7-IMG](screenshots/JOURNAL7-IMG16.png)
## The 3D
I also exported the pcb as step file and then created a fusion 360 project, where i imported the pcb. I also found on grabcad the cardKB keyboard. And on laskakits page, the e-shop where i will be buying the display, there is a github link, where i found the 3d model of the display. So i imported everything.
![JOURNAL7-IMG](screenshots/JOURNAL7-IMG17.png)
Now i need to create the case around it. I want it to be as small as possible, but still fit all the things it needs.I created like a case around it, there are holes for the pcb mounting holes m3 bolts.
![JOURNAL7-IMG](screenshots/JOURNAL7-IMG18.png)
![JOURNAL7-IMG](screenshots/JOURNAL7-IMG19.png)
The connector i am using is for a antena, i found that the female antena connector needs 10 to 12 mm round free space, so it can be srewed in, so i made a hole around it.
![JOURNAL7-IMG](screenshots/JOURNAL7-IMG20.png)

## The antena
So for the antena i need something cheap efficent and portable, and i found few possible ones on laskakit.
1. One is 11cm one with 2.15dbm gain
2. Two is 21cm with the same gain

The difference is that the longer one would be more efficent because it is longer. I found a nonspecific antena on grabcad, they didnt have the specific and and i imported it. It was only 14cm long, So to make it the 21cm i tried extruding some sections, and after some time i created a 21cm version. Then i added joints to it and connected it to the connector. Now it looks ok. I need to choose between the:
- 21cm antena
- 11cm antena
I will do a little bit  more research on the cons and pros of each version. For now this is how the asembly looks like:


![JOURNAL7-IMG](screenshots/JOURNAL7-IMG21.png)
![JOURNAL7-IMG](screenshots/JOURNAL7-IMG22.png)

I still need to create the other half of the hatch, find a battery, check everything, finish software and ui, fix problem with the power button, but for now it looks good.

### Time Spent: 5.5 Hours