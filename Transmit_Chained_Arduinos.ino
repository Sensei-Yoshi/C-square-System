#include <EasyTransfer.h>

//create object
EasyTransfer ET; 
int mydata = 5;
void setup(){
  Serial.begin(9600);
  //start the library, pass in the data details and the name of the serial port. Can be Serial, Serial1, Serial2, etc.
  ET.begin(details(mydata), &Serial);
  ET.begin(details(mydata), &Serial1);
}

void loop(){
  //this is how you access the variables. [name of the group].[variable name]
  //send the data
  ET.sendData();
  delay(5000);
}
