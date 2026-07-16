#include <Wire.h>
#include <SPI.h>
#include <RadioLib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SharpMem.h>
#include <RTClib.h>



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


void setup() {
  Serial.begin(115200);
  
  // Initialize I/O Pins
  pinMode(VIB_MOTOR_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(VIB_MOTOR_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  // Initialize I2C Bus
  Wire.begin(I2C_SDA, I2C_SCL);

  // Initialize Memory LCD
  display.begin();
  display.clearDisplay();
  display.refresh();
  Serial.println("Display Initialized.");

  drawScreen_main();

  // Initialize LoRa Module (868.0 MHz)
  Serial.print(F("[LoRa] Initializing ... "));
  // Parameters: Carrier Freq (MHz), Bandwidth (kHz), Spreading Factor, Coding Rate
  int state = lora.begin(868.0, 125.0, 9, 7); 
  lora.setOutputPower(5);
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

  // Check for incoming LoRa Packets 
  checkIncomingLoRa();
}


void sendLoRaMessage(String msg) {
  Serial.print(F("[LoRa] Sending packet"));
  int state = lora.transmit(msg);

  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("Sent successfully!"));
  } else {
    Serial.print(F("Failed, code "));
    Serial.println(state);
  }
}

void checkIncomingLoRa() {
  String receivedData;
  int state = lora.receive(receivedData);

  if (state == RADIOLIB_ERR_NONE) {
    Serial.print(F("[LoRa] Received packet! Data: "));
    Serial.println(receivedData);
    
    triggerVibe(200);
    triggerBuzzer(100);
    
    drawTextOnScreen("New Msg: " + receivedData);
  }
}

void drawScreen_main(void) {
  DateTime now = rtc.now();

  display.clearDisplay();
  
  //nTime & Battery
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

void drawTextOnScreen(String text, int x = 0, int y = 20, int size = 1) {
  display.clearDisplay();
  display.setTextSize(size);
  display.setTextColor(BLACK); 
  display.setCursor(x, y);
  display.println(text);
  display.refresh(); 
}



// --- CARDKB I2C KEYBOARD FUNCTION ---
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