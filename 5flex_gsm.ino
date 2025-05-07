#include <LiquidCrystal.h>
#include <SoftwareSerial.h>


LiquidCrystal lcd(2, 3, 4, 5, 6, 7);


SoftwareSerial gsmSerial(8, 9); 


const int flexPins[] = {A0, A1, A2, A3, A4};
const int flexThreshold = 120; 


const int switchPin = 10;
bool lastMode = true;


const String messages[] = {
  "Food Needed!",
  "Help Needed!",
  "Water Needed!",
  "Emergency!",
  "Milk Needed!"
};


const char* phoneNumber = "+917010575332"; 

void setup() {
  Serial.begin(9600);
  gsmSerial.begin(9600);
  
 
  lcd.begin(16, 2);
  lcd.clear();
  
 
  pinMode(switchPin, INPUT_PULLUP);

 
  sendCommand("AT", 1000);
  sendCommand("AT+CMGF=1", 1000); 

 
  updateLCDMode();
}

void loop() {
  bool lcdMode = digitalRead(switchPin) == HIGH; 


  if (lcdMode != lastMode) {
    lastMode = lcdMode;
    updateLCDMode();
  }

  for (int i = 0; i < 5; i++) {
    int sensorValue = analogRead(flexPins[i]);

    if (sensorValue < flexThreshold) {
      String message = messages[i];

      if (lcdMode) {
       
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Alert:");
        lcd.setCursor(0, 1);
        lcd.print(message);

        
        sendSMS(message);

        delay(5000); 
        updateLCDMode(); 
      } else {
       
        Serial.print("Triggered Sensor: ");
        Serial.println(i + 1);
        delay(1000);
      }
    }
  }
}


void updateLCDMode() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mode: ");
  lcd.setCursor(0, 1);
  if (digitalRead(switchPin) == HIGH) {
    lcd.print("sms Mode");
  } else {
    lcd.print("device Mode");
  }
}

void sendCommand(String command, const int timeout) {
  gsmSerial.println(command);
  long int time = millis();
  while (millis() - time < timeout) {
    while (gsmSerial.available()) {
      char c = gsmSerial.read();
      Serial.print(c);
    }
  }
}

void sendSMS(String message) {
  gsmSerial.print("AT+CMGS=\""); 
  gsmSerial.print(phoneNumber);
  gsmSerial.println("\"");
  delay(1000);
  gsmSerial.println(message);
  delay(1000);
  gsmSerial.write(26); 
  delay(5000);
}
