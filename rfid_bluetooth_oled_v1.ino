#include <MFRC522.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Servo.h>
#include <SoftwareSerial.h>

#define OLED_RESET -1
Adafruit_SSD1306 display(OLED_RESET);

#define SS_PIN 10
#define RST_PIN 9
#define LED_G 5 
#define LED_R 4 
#define BUZZER 2 
Servo myServo; 
int bluetoothTX = 0; 
int bluetoothRX = 1; 

SoftwareSerial Blue(bluetoothTX, bluetoothRX);

MFRC522 rfid(SS_PIN, RST_PIN);

//MFRC522::MIFARE_Key key;

int code[] = {243, 213, 106, 14};
//int codeRead = 0;
String uidString;

//bool authorized = false;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  Blue.begin(9600);
  myServo.attach(3);
  myServo.write(0);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(LED_G, LOW);  
  digitalWrite(LED_R, LOW);
  noTone(BUZZER);
  rfid.PCD_Init();


  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(10, 0);
  display.print("Apropie tagul RFID");
  display.display();
}

void loop() {


  if (rfid.PICC_IsNewCardPresent()) {
    readRFID();
  }

  if (Blue.available() > 0) {
    unsigned int servopos = Blue.read();
    if (servopos == 'A') {

      display.clearDisplay();
      display.setCursor(10, 0);
      display.print("Acces autorizat");
      display.display();

      digitalWrite(LED_G, HIGH);
      tone(BUZZER, 500);
      delay(300);
      noTone(BUZZER);
      myServo.write(180);
      delay(3000);
      myServo.write(0);
      digitalWrite(LED_G, LOW);

      
      display.clearDisplay();
      display.setCursor(10, 0);
      display.print("Apropie tagul RFID");
      display.display();
    }
  }
}

void readRFID()
{
  
  rfid.PICC_ReadCardSerial();
  Serial.print(F("\nTip PICC: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));
  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Tagul utilizat nu este unul tip MIFARE Classic."));
    return;
  }

  clearUID();

  Serial.println("Scanned PICC's UID:");
  printDec(rfid.uid.uidByte, rfid.uid.size);

  uidString = String(rfid.uid.uidByte[0]) + " " + String(rfid.uid.uidByte[1]) + " " + String(rfid.uid.uidByte[2]) + " " + String(rfid.uid.uidByte[3]);

  printUID();

  int i = 0;
  bool match = true;
  while (i < rfid.uid.size) {
    if (!(rfid.uid.uidByte[i] == code[i])) {
      match = false;
    }
    i++;
  }

  if (match) {
    Serial.println("\nTAG existent in baza de date");
    display.clearDisplay();
    display.setCursor(10, 0);
    display.print("Acces autorizat");
    display.display();
    printUID();

    digitalWrite(LED_G, HIGH);
    tone(BUZZER, 500);
    delay(300);
    noTone(BUZZER);
    myServo.write(180);
    delay(3000);
    myServo.write(0);
    digitalWrite(LED_G, LOW);

    display.clearDisplay();
    display.setCursor(10, 0);
    display.print("Apropie tagul RFID");
    display.display();
    clearUID();

  } else {
    Serial.println("\nTag necunoscut");
    display.clearDisplay();
    display.setCursor(10, 0);
    display.print("Acces respins");
    display.display();
    printUID();
    digitalWrite(LED_R, HIGH);
    tone(BUZZER, 300);
    delay(1000);
    digitalWrite(LED_R, LOW);
    noTone(BUZZER);

    display.clearDisplay();
    display.setCursor(10, 0);
    display.print("Apropie tagul RFID");
    display.display();
    clearUID();
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

void printDec(byte* buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}

void clearUID() {
  display.setTextColor(BLACK);
  display.setTextSize(1);
  display.setCursor(30, 20);
  display.print(uidString);
  display.display();
}

void printUID() {
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 20);
  display.print("UID: ");
  display.setCursor(30, 20);
  display.print(uidString);
  display.display();
}
