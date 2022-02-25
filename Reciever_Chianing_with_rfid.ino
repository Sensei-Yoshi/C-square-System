#include <SPI.h>
#include <RFID.h>
//#include <EasyTransfer.h>

//EasyTransfer ET;

int mydata = 0;
#define SS_PIN1 10
#define SS_PIN2 7
#define SS_PIN3 11
#define RST_PIN 8

RFID rfid1(SS_PIN1, RST_PIN);
RFID rfid2(SS_PIN2, RST_PIN);
RFID rfid3(SS_PIN3, RST_PIN);

RFID readers[3] = {rfid1, rfid2, rfid3};



void setup() {
  Serial.begin(9600);
  ET.begin(details(mydata), &Serial);
  Serial.println("Starting the RFID Reader...");
  SPI.begin();
  rfid1.init();
  rfid2.init();
  Serial.print("Started RFID Reader");

}

void loop() {
  //check and see if a data packet has come in.
  if (ET.receiveData()) {
    Serial.println(mydata);
    String packageId = readpackageID(readers[mydata]);


  }

}
String readpackageID(RFID rfid) {

  String packageId;

  if (rfid.isCard()) {
    if (rfid.readCardSerial()) {
      packageId = String(rfid.serNum[0]) + String(rfid.serNum[1]) + String(rfid.serNum[2]) + String(rfid.serNum[3]);
    }
    rfid1.halt();
  }
  Serial.print("Package Id: ");
  Serial.println(packageId);
  return packageId;
}
