#include <Wire.h>
#include <SPI.h>
#include <RadioLib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SharpMem.h>
#include <RTClib.h>
#include "TextWrap.h"
#include <LittleFS.h>

#define MSG_FILE "/messages.txt"

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

#define MAX_CHANNELS 5

float battery_volt = 2.0f;
const int ADC_PIN = 34;

bool vibeActive = false;
unsigned long vibeEndTime = 0;

bool buzzerActive = false;
unsigned long buzzerEndTime = 0;

#define VIB_MOTOR_PIN 3   
#define BUZZER_PIN 45  

// M5Stack CardKB I2C Address
#define CARDKB_I2C_ADDR 0x5F

SX1276 lora = new Module(RFM_NSS, RFM_DIO0, RFM_RST, RFM_DIO1);

// Duty Cycle variables
unsigned long windowStart = 0;
unsigned long totalTxTime = 0;
const unsigned long SIX_MINUTES = 360000; 
const unsigned long MAX_TX_ALLOWANCE = 36000;


uint8_t myPagerId = 1; // Unique ID of this pager
uint8_t currentChannel = 1; // The channel this pager is currently tuned to


struct PagerPacket {
  uint8_t targetChannel; // The Software Channel (like 1, 2, 3)
  uint8_t senderId; // ID of the pager sending the message
  char message[100]; // The text message (max 100 characters)
};

// Messages
#define MAX_MESSAGES 10

struct StoredMessage {
  uint8_t channel;
  uint8_t senderId;
  char text[100];
};

StoredMessage messages[MAX_MESSAGES];
uint8_t messageCount = 0;


void Load() {
  messages[0] = {1, 1, "Hey, testing the pager, how far can this thing actually reach with the stock antenna?"};
  messages[1] = {1, 2, "Not sure yet, still soldering the SMA connector on mine."};
  messageCount = 2;
}


// Sharp Memory LCD 
Adafruit_SharpMem display(SHARP_SCK, SHARP_MOSI, SHARP_SS, DISPLAY_WIDTH, DISPLAY_HEIGHT);
int current_screen1_btn_slctd = 0; // 0 for messages, 1 for location, 2 for settings

int current_screen = 0; // 0 for main menu, 1 for messages, 2 for location, 3 for settings


String message_notification = "";
// RTC 
RTC_PCF8563 rtc;

uint8_t lastMinute = 99;


void read_adc() {
  int rawADC = analogRead(ADC_PIN);

  float pinVoltage = (rawADC / ADC_MAX_VALUE) * V_REF;

  battery_volt = pinVoltage * 2.0;

}
void setup() {
  Serial.begin(115200);
  
  if (!LittleFS.begin(true)) { // true = format if mount fails
    Serial.println("LittleFS mount failed!");
  } else {
    Serial.println("LittleFS mounted.");
  }

  loadMessages();
  // Initialize I/O Pins
  pinMode(VIB_MOTOR_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(VIB_MOTOR_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  
  analogReadResolution(12);

  analogSetPinAttenuation(ADC_PIN, ADC_11db);

  // Initialize I2C Bus
  Wire.begin(I2C_SDA, I2C_SCL);

  // Initialize Memory LCD
  display.begin();
  display.clearDisplay();
  display.refresh();
  Serial.println("Display Initialized.");

  drawScreen_main();

  // Initialize LoRa Module (869.525 MHz for 10% duty cycle)
  Serial.print(F("[LoRa] Initializing ... "));
  
  // Carrier Freq: 869.525 MHz, Bandwidth: 125.0 kHz, Spreading Factor: 8, Coding Rate: 5 (4/5)
  int state = lora.begin(869.525, 125.0, 8, 5); 
  
  // Set output power to maximum legal limit for this frequency (it is max 27 dBm, but my rf module has maximum 20)
  lora.setOutputPower(20);
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("Success!"));
  } else {
    Serial.print(F("Failed, code "));
    Serial.println(state);
  }

  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
  } else {
    Serial.println("RTC Initialized.");
    // If RTC lost power, set the time to compilation time:
    if (rtc.lostPower()) {
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
  }
  
  // Alert that it is set up
  triggerVibe(150);
}



void loop() {
  updatePeripherals();

  read_adc();
  // Read Keyboard 
  char key = readCardKB();
  if (key != 0) {
    Serial.print("Key Pressed: ");
    Serial.println((uint8_t)key, HEX); 
    
    // Clear notifications on any key press
    message_notification = "";

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

        else if (key == 0x0D) {
          switch (current_screen1_btn_slctd) {
            case 0:
              drawScreen_messages();
              break;
            
            case 1:
              break;

            case 2:
              break;
          }
          
          triggerVibe(40);
        }
        break;
      case 2:
        if (key == 0x7B) { // right
          current_channel = (current_channel + 1) % MAX_CHANNELS;
        }

        else if (key == 0x4B) { // left
          current_channel = (current_channel - 1) % MAX_CHANNELS;
        }

        else if (key == 0x1B) { // esc
          drawScreen_Main();
        }
        break;
    }
  }

  // Update screen automatically when clock minute changes
  DateTime now = rtc.now();
  if (now.minute() != lastMinute) {
    lastMinute = now.minute();
    if (current_screen == 0) {
      drawScreen_main();
    }
  }

  // Check for incoming LoRa Packets 
  checkIncomingLoRa();
}


void sendLoRaPacket(uint8_t targetChan, const char* textMsg) {
  unsigned long currentTime = millis();

  if (currentTime - windowStart >= SIX_MINUTES) {
    windowStart = currentTime;
    totalTxTime = 0;
  }

  if (totalTxTime >= MAX_TX_ALLOWANCE) {
    Serial.println(F("[LoRa] TX Limit Reached! Wait for next window."));
    message_notification = "TX Limit reached. Wait.";
    triggerVibe(100);
    if (current_screen == 0) {
      drawScreen_main();
    }
    return;
  }

  Serial.print(F("[LoRa] Sending custom pager packet... "));
  
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
    Serial.print(F("Sent successfully! Time on air: "));
    Serial.print(txDuration);
    Serial.println(F(" ms"));
    saveMessage(targetChan, myPagerId, textMsg);
    loadMessages();
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

    message_notification =
      "Ch " + String(receivedPacket.targetChannel) +
      " From P" + String(receivedPacket.senderId) +
      ": " + msg;
    saveMessage(receivedPacket.targetChannel, receivedPacket.senderId, receivedPacket.message);
    loadMessages();

    if (current_screen == 0) {
      drawScreen_main();
    }
      
    } else {
      // Message was on a different channel. Ignore it.
      Serial.print(F("[LoRa] Ignored message for non-active channel: "));
      Serial.println(receivedPacket.targetChannel);
    }
  }
}


void saveMessage(uint8_t channel, uint8_t senderId, const char* text) {
  File f = LittleFS.open(MSG_FILE, FILE_APPEND);
  if (!f) {
    Serial.println("Failed to open file for writing");
    return;
  }
  f.printf("%d,%d,%s\n", channel, senderId, text);
  f.close();
}

void loadMessages() {
  messageCount = 0;

  File f = LittleFS.open(MSG_FILE, FILE_READ);
  if (!f) {
    Serial.println("No message file yet.");
    return;
  }

  while (f.available() && messageCount < MAX_MESSAGES) {
    String line = f.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) continue;

    int firstComma = line.indexOf(',');
    int secondComma = line.indexOf(',', firstComma + 1);
    if (firstComma == -1 || secondComma == -1) continue; // malformed line, skip

    messages[messageCount].channel  = line.substring(0, firstComma).toInt();
    messages[messageCount].senderId = line.substring(firstComma + 1, secondComma).toInt();

    String text = line.substring(secondComma + 1);
    strncpy(messages[messageCount].text, text.c_str(), sizeof(messages[messageCount].text) - 1);
    messages[messageCount].text[sizeof(messages[messageCount].text) - 1] = '\0';

    messageCount++;
  }
  f.close();

  Serial.printf("Loaded %d messages from flash.\n", messageCount);
}

void drawScreen_messages(void) {
    int current_screen = 1;
    DateTime now = rtc.now();
    display.clearDisplay();
    // string 1 copy 1
    display.setTextColor(1);
    display.setTextSize(2);
    display.setTextWrap(false);
    display.setCursor(2, 5);
    display.printf("Time: %02d:%02d", now.hour(), now.minute());
    // string 1 copy 2
    display.setCursor(281, 6);
    display.printf("Bat: %.2fV", battery_volt);
    // rect 3
    display.drawRect(0, 39, 400, 147, 1);
    // rect 5
    display.drawRect(0, 186, 400, 53, 1);
    // rect 6
    display.drawRect(139, 0, 134, 22, 1);
    // string 6
    display.setCursor(148, 4);
    display.print("CH: ");
    display.print(currentChannel);
    // string 11 copy 1
    display.setTextSize(1);
    display.setCursor(3, 27);
    display.print(message_notification);
    // string 8
    display.setCursor(3, 192);
    display.print("Message you type with your keyboard here");

    // string 9 -- wrapped messages inside rect 3 (0,39,400,147)
    int16_t textX = 5;
    int16_t textY = 45;
    int16_t textMaxY = 39 + 147 - 3; // bottom of rect 3, minus a little padding

    for (uint8_t i = 0; i < messageCount && textY < textMaxY; i++) {
      char line[110];
      snprintf(line, sizeof(line), "P%d: %s", messages[i].senderId, messages[i].text);
      textY = printWrapped(display, textX, textY, 400 - 10, textMaxY, line, 1);
      textY += 2; // gap between messages
    }

    display.refresh(); 
}



void drawScreen_main(void) {
  DateTime now = rtc.now();
  int current_screen = 0;
  display.clearDisplay();
  
  //nTime & Battery
  display.setTextColor(1);
  display.setTextSize(2);
  display.setTextWrap(false);
  display.setCursor(265, 5);
  display.printf("Time: %02d:%02d", now.hour(), now.minute());

  display.setCursor(289, 24);
  display.printf("Bat: %.2fV", battery_volt);
  
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
  }
  
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




// CARDKB I2C 
char readCardKB() {
  Wire.requestFrom(CARDKB_I2C_ADDR, 1);
  while (Wire.available()) {
    char c = Wire.read();
    if (c != 0) {
      return c; // Returns the ASCII character of the pressed key
    }
  }
  return 0; // Return 0 if no key is pressed
}

void updatePeripherals() {
  unsigned long currentTime = millis();

  // Check if it is time to turn off the vibration motor
  if (vibeActive && (currentTime >= vibeEndTime)) {
    digitalWrite(VIB_MOTOR_PIN, LOW);
    vibeActive = false;
    Serial.println("[Hardware] Vibe Off");
  }

  // Check if it is time to turn off the buzzer
  if (buzzerActive && (currentTime >= buzzerEndTime)) {
    digitalWrite(BUZZER_PIN, LOW);
    buzzerActive = false;
    Serial.println("[Hardware] Buzzer Off");
  }
}

void triggerVibe(unsigned long durationMs) {
  digitalWrite(VIB_MOTOR_PIN, HIGH);
  vibeEndTime = millis() + durationMs;
  vibeActive = true;
}

void triggerBuzzer(unsigned long durationMs) {
  digitalWrite(BUZZER_PIN, HIGH);
  buzzerEndTime = millis() + durationMs;
  buzzerActive = true;
}