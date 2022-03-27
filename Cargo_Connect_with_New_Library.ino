#include <SPI.h>
#include <MFRC522.h>
#include "TinyGPS++.h"
#include "SoftwareSerial.h"
#include <singleLEDLibrary.h>
#include <math.h>
#include <string.h>

constexpr uint8_t NR_OF_READERS = 3;

constexpr uint8_t RST_PIN  = 8;
constexpr uint8_t SS_PIN1 = 10;
constexpr uint8_t SS_PIN2 = 7;
constexpr uint8_t SS_PIN3 = 9;
#define numberOfPackages 6
byte ssPins[] = {SS_PIN1, SS_PIN2, SS_PIN3};
MFRC522 readers[3];

sllib led1(5);
sllib led2(4);
sllib led3(3);
sllib leds[3] = {led1, led2, led3};


SoftwareSerial serial_connection(11, 12); //RX=pin 10, TX=pin 11
TinyGPSPlus gps;//This is the GPS object that will pretty much do all the grunt work with the NMEA data
/*
  Set packages database

  P1: 108240179204- Avi
  P2: 92247228205-Arsh
  P3: 24211615356- Ansh
  P4: 15313713990- Viraj
  P5: 11425022327 - Avni
  P6: 1301011927- School

*/
String packagesData[numberOfPackages][3] = { {"108240179204", "40.9469309", "-74.1101247"}, {"92247228205", "40.9370687", "-74.1291859"}, {"24211615356", "40.9491214", "-74.1091650"}, {"15313713990", "40.9473777", "-74.1160913"}, {"11425022327", "40.9447131", "-74.1115394"} , {"1301011927", "40.9460423", "-74.1167200"} };
//String packagesData[2][3] = { {"24211615356", "40.946525", "-74.110275"}, {"15313713990", "40.946620", "-74.110351"} };

double currentGPSLng;
double currentGPSLat;


void setup() {

  Serial.begin(9600);
  while (!Serial);

  Serial.println("Starting the RFID Readers...");
  SPI.begin();

  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++)
  {
    readers[reader].PCD_Init(ssPins[reader], RST_PIN); // Init each MFRC522 card
  }

  pinMode(RST_PIN, OUTPUT);
  digitalWrite(RST_PIN, LOW);
  Serial.print("Started RFID Readers");

  Serial.println("Starting the GPS...");
  serial_connection.begin(9600);//This opens up communications to the GPS
  Serial.println("Started GPS");//Just show to the monitor that the sketch has started
}

void loop()
{
  getCurrentGPSLocation();
  int packageSpot = 0;

  for (packageSpot = 0; packageSpot <= 2; packageSpot++)
  {
    String packageId = readpackageID(readers[packageSpot]);
    checkpackageforDelivery(packageSpot, packageId);
  }
  resetLed();
}

void getCurrentGPSLocation() {
  while (serial_connection.available()) //While there are characters to come from the GPS
  {
    gps.encode(serial_connection.read());//This feeds the serial NMEA data into the library one char at a time
  }
  if (gps.location.isUpdated()) //This will pretty much be fired all the time anyway but will at least reduce it to only after a package of NMEA data comes in
  {
    //Get the latest info from the gps object which it derived from the data sent by the GPS unit

    currentGPSLat = gps.location.lat();
    currentGPSLng = gps.location.lng();
    Serial.println();
    Serial.println("Latitude:");
    Serial.println(currentGPSLat, 6);
    Serial.println("Longitude:");
    Serial.println(currentGPSLng, 6);

  }
}

String readpackageID(MFRC522 rfid) {

  String packageId;
  SPI.beginTransaction(SPISettings(400000, MSBFIRST, SPI_MODE0));
  digitalWrite(RST_PIN, HIGH);
  rfid.PCD_Init();
  //rfid.PCD_DumpVersionToSerial();
  //SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {

    packageId = printDec(rfid.uid.uidByte, rfid.uid.size);// + String(rfid.uid.uidByte[1]) + String(rfid.uid.uidByte[2]) + String(rfid.uid.uidByte[3]);
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  //SPI.endTransaction();
  Serial.print("Package Id: ");
  Serial.println(packageId);
  return packageId;
}

String printDec(byte *buffer, byte bufferSize) {
  String packageId;
  for (byte i = 0; i < bufferSize; i++) {
    //packageId = packageId + String(buffer[i] < 0x10 ? " 0" : " ");
    packageId = packageId + String(buffer[i]);

  }
  return packageId;
}

void checkpackageforDelivery(int packageSpot, String packageId)
{
  int packageNumber = 0;
  for (packageNumber = 0; packageNumber <= (numberOfPackages - 1) ; packageNumber++) {
    if (packageId == packagesData[packageNumber][0]) {
      Serial.print("Package In Database");
      double packageLat = packagesData[packageNumber][1].toDouble();
      double packageLng = packagesData[packageNumber][2].toDouble();
      double latDifference = abs(currentGPSLat - packageLat);
      double longDifference = abs(currentGPSLng - packageLng);
      // double gpsdist = gps.distanceBetween(currentGPSLat, currentGPSLng, packageLat, packageLng);

      Serial.print("latDifference: ");
      Serial.println(latDifference, 6);
      Serial.print("longDifference");
      Serial.println(longDifference, 6);
      if ((latDifference <= 50000) && (longDifference <= 500000)) //if distance is in radius of 2 Meters, Value needs to be 0.00005 or 0.005
      {
        Serial.println("Blinking");
        blinkLed(packageSpot);
      }

    }
  }
}

void blinkLed(int packageSpot) {
  leds[packageSpot].blinkSingle(0.1);
}

void resetLed() {
  int ledreset = 0;
  for (ledreset = 0; ledreset <= 2; ledreset++) {
    Serial.print("LED reset count");
    leds[ledreset].setOffSingle();
  }
}
