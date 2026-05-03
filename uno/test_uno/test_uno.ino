#include <SoftwareSerial.h>

SoftwareSerial pcbSerial(10, 11); // arduino pin RX 10 and TX 11

void setup() {
  Serial.begin(115200); // arduino usb to computer for debugging
  pcbSerial.begin(9600); // serial for pcb to arduino

}

void loop() {
  // put test code you want to run here

}

// use to test uart from pcb to arduino
void uartTest(){
  if (pcbSerial.available()){
    String msg = pcbSerial.readStringUntil('\n');
    Serial.println(msg);
  }
}

