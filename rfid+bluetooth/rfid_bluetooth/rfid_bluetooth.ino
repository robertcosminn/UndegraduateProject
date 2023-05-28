#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <SoftwareSerial.h>

#define SS_PIN 10
#define RST_PIN 9
#define LED_G 5 //define green LED pin
#define LED_R 4 //define red LED pin
#define BUZZER 2 //buzzer pin
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Creeaza instantierea modulului MFRC522
Servo myServo; //define servo name
int bluetoothTX = 0; //bluetooth TX conectat la pinul 0 (RX)
int bluetoothRX = 1; //bluetooth RX conectat la pinul 1 (TX)

SoftwareSerial Blue(bluetoothTX, bluetoothRX); //am creat obiectul "Blue" cu parametrii mentionati pentru comunicarea seriala cu bluetoothul

void setup() 
{
  Serial.begin(9600);// Initializeaza comunicarea seriala cu computerul
  Blue.begin(9600);   // Initializeaza comunicarea seriala bluetooth cu dispozitivul android
  SPI.begin();      // Initializeaza bus-ul SPI
  mfrc522.PCD_Init();   // Initializeaza MFRC522
  myServo.attach(3); //servo pin
  myServo.write(0); //servo pozitie de start
  pinMode(LED_G, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  noTone(BUZZER);
  Serial.println("Apropie cardul de cititorul RFID :");
  Serial.println();

}
void loop() 
{
//citim de pe bluetooth si scriem pe USB serial
  if(Blue.available()>0) //verifica conexiunea bluetooth si primeste un numar de la acesta
  {
    int servopos = Blue.read(); //salveaza numarul receptionat in servopos
    Serial.println(servopos);//printeaza numarul curent servopos
    myServo.write(servopos);//roteste servo-motorul la unghiul receptionat din aplicatia android
  }
  // Verifica prezenta unui card
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Citeste seria cardului
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Afiseaza tagul UID pe serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Mesaj : ");
  content.toUpperCase();
  if (content.substring(1) == "F3 D5 6A 0E") //punem UID tagul cardului/cardurilor la care putem avea acces
  {
    Serial.println("Acces autorizat !");
    Serial.println();
    delay(500);
    digitalWrite(LED_G, HIGH);
    tone(BUZZER, 500);
    delay(300);
    noTone(BUZZER);
    myServo.write(180);
    delay(5000);
    myServo.write(0);
    digitalWrite(LED_G, LOW);
  }
 
 else   {
    Serial.println("Acces respins !");
    digitalWrite(LED_R, HIGH);
    tone(BUZZER, 300);
    delay(1000);
    digitalWrite(LED_R, LOW);
    noTone(BUZZER);
  }

}
