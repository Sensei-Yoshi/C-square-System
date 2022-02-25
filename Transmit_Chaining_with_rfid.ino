#include <EasyTransfer.h>

EasyTransfer ET; 
int mydata = 0;
void setup(){
  Serial.begin(9600);
  //start the library, pass in the data details and the name of the serial port. Can be Serial, Serial1, Serial2, etc.
  ET.begin(details(mydata), &Serial);
}

void loop(){
 for (mydata = 0; mydata <= 1; mydata++)
  ET.sendData();
  delay(5000);
}
