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

