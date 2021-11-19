#include <SPI.h>
#include <RFID.h>
#define SS_PIN 10
#define SS_PIN1 7
#define RST_PIN 8
RFID rfid(SS_PIN, RST_PIN);
RFID rfid1(SS_PIN1, RST_PIN);

String rfidCard;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting the RFID Reader...");
  SPI.begin();
  rfid.init();
  rfid1.init();
}

void loop() {
  if (rfid.isCard()) {
    if (rfid.readCardSerial()) {
      rfidCard = String(rfid.serNum[0]) + " " + String(rfid.serNum[1]) + " " + String(rfid.serNum[2]) + " " + String(rfid.serNum[3]);
      Serial.println(rfidCard);
    }
    rfid.halt();
  /*}
    if (rfid1.isCard()) {
    if (rfid1.readCardSerial()) {
      rfidCard = String(rfid1.serNum[0]) + " " + String(rfid1.serNum[1]) + " " + String(rfid1.serNum[2]) + " " + String(rfid1.serNum[3]);
      Serial.println(rfidCard);
    }
    rfid1.halt();
  }*/
}
}
