#include <EasyTransfer.h>

//create object
EasyTransfer ET;

int mydata = 0;

//give a name to the group of data


void setup() {
  Serial.begin(9600);
  //start the library, pass in the data details and the name of the serial port. Can be Serial, Serial1, Serial2, etc.
  ET.begin(details(mydata), &Serial2);
  
}

void loop() {
  //check and see if a data packet has come in.
  if (ET.receiveData()) {
    Serial.println(mydata);
  }

  //you should make this delay shorter then your transmit delay or else messages could be lost
  delay(250);
}
