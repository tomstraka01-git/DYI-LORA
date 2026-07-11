# Journal #1 June 11: Starting the project


## What do i want to do?
I want to make my own meshtastic pager fully from scratch. It should use some radio module, have a pcb and a 3D printed case.
![JOURNAL1-IMG](screenshots\JOURNAL1-IMG4.png)

I researched about the topic a little bit and i know that i want it to have a display (propably oled), it should have a battery so maybe even a charger circuit. 
It should also have some way to type the messages: One wat is to have a like a keyboard i2c module in the pager (easier to type and do things, but takes more space)
Or i can have just few buttons and the user will select the letter he wants to type with either a rotary encoder or buttons for left or right movement of the cursor or i can have it connected to phone or laptop via bluetooth or wifi (the easiest thing if already using mcu with wifi or ble).
I have searched wich radio module i would use, and i found this one on laskakit HopeRF RFM95W 868Mhz https://www.laskakit.cz/hoperf-rfm95w-868mhz-komunikacni-modul/.

![JOURNAL1-IMG](screenshots\JOURNAL1-IMG1.png)

This one is okay for my task. It is in the correct frequency in eu and has range of hundred meters to kilometers, depending on conditions.

## The MCU
For the MCU i can choose between a esp32 s3 board, wich is already soldered and i will plug it into a female headers or solder, or i can choose just the chip and add things like resistors capacitors myself (harder to do but saves space), the problem here is that it is hard to solder and i dont have the tools to solder by hand, i could try but it is risky.
![JOURNAL1-IMG](screenshots\JOURNAL1-IMG2.png)
![JOURNAL1-IMG](screenshots\JOURNAL1-IMG3.png)
Next i will create a kicad project and try to decide wich options to go with.
### Time spent: 2 hours
