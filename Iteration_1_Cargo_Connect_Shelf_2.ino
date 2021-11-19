#include <SPI.h>
#include <RFID.h>
#include "TinyGPS++.h"
#include "SoftwareSerial.h"
#include <singleLEDLibrary.h>
#include <math.h>

#define SS_PIN1 10
#define SS_PIN2 7
#define RST_PIN 8

sllib led1(5);
sllib led2(4);
sllib leds[2] = {led1, led2};


SoftwareSerial serial_connection(11, 12); //RX=pin 10, TX=pin 11
TinyGPSPlus gps;//This is the GPS object that will pretty much do all the grunt work with the NMEA data

//Set packages database

String packagesData[2][3] = { {"24211615356", "40.946525", "-74.110275"}, {"15313713990", "40.946620", "-74.110351"} };

double currentGPSLng;
double currentGPSLat;

RFID rfid1(SS_PIN1, RST_PIN);
RFID rfid2(SS_PIN2, RST_PIN);

RFID readers[2] = {rfid1, rfid2};

void setup() {

  Serial.begin(9600);
  Serial.println("Starting the RFID Reader...");
  //SPI.beginTransaction(SPISettings(400000, MSBFIRST, SPI_MODE0));
  SPI.begin();
  rfid1.init();
  rfid2.init();
  Serial.print("Started RFID Reader");

  Serial.println("Starting the GPS...");
  serial_connection.begin(9600);//This opens up communications to the GPS
  Serial.println("Started GPS");//Just show to the monitor that the sketch has started
}

void loop() {
  resetLed();
  getCurrentGPSLocation();
  int packageSpot = 0;

  for (packageSpot = 0; packageSpot <= 1; packageSpot++)
  {
    String packageId = readpackageID(readers[packageSpot]);
    checkpackageforDelivery(packageSpot, packageId);
  }
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

void checkpackageforDelivery(int packageSpot, String packageId)
{
  int packageNumber = 0;
  for (packageNumber = 0; packageNumber <= 1; packageNumber++) {
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
      if ((latDifference <= 5000) && (longDifference <= 50000)) //if distance is in radius of 2 Meters
      {
        Serial.println("Blinking");
        blinkLed(packageSpot);
      }

    }
  }
}

void blinkLed(int packageSpot) {
  leds[packageSpot].blinkSingle(1);
}

void resetLed() {
  int ledreset = 0;
  for (ledreset = 0; ledreset <= 1; ledreset++) {
    Serial.print("LED reset count");
    leds[ledreset].setOffSingle();
  }
}
