# Journal #1 June 11: Starting the project


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


# Journal #2 June 13: 

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

# Journal #3 June 15:  Battery charging, protection circuit, buzzer, vibration motor, gps, compas, debugging, cardKB, memory lcd

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