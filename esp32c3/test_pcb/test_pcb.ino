#include <SPI.h>
#include <Wire.h>

// SPI to TDC7201
#define PIN_TDC_ENABLE 20 // enables resets TDC7201
#define PIN_SPI_CLK 0 // GP00 SPI clock SCLK
#define PIN_SPI_CS 1 // GP01 chip select CSB
#define PIN_TDC_IN 3 // GP03 data in to TDC7201

// TDC to MCU
#define PIN_TDC_OUT1 6 // GP06 data out 1 from TDC7201
#define PIN_TDC_OUT2 7 // GP07 data out 2 from TDC7201

#define PIN_INT1 8 // interupt from TDC1
#define PIN_INT2 10 // interrupt from TDC2

// I2C / DACs
#define PIN_I2C_SCL 4 // GP04 SCL
#define PIN_I2C_SDA 5 // GP05 SDA
#define VSET_DAC_ADDR 0x60
#define RECVR_DAC_ADDR 0x61
#define DAC_REF_VOLT 3.3 // change to match whats actually powering the DAC

// UART to arduino
#define PCB_TX_PIN 21
#define PCB_RX_PIN 20

SPIClass tdcSPI(FSPI);

SPISettings tdcSettings(
  1000000, // SPI clock speed of 1 MHz
  MSBFIRST, // send most significant bit first, left to right
  SPI_MODE0 //clock polarity/phase mode 0, sent on rising edge
);

HardwareSerial unoSerial(1);

void setup() {
  Serial.begin(115200); //usb debug to pc if wanted
  unoSerial.begin(9600, SERIAL_8N1, PCB_RX_PIN, PCB_TX_PIN); // to arduino serial

  delay(1000);

  // setup for DAC, uncomment for DAC tests
  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL); // for code using the DACs

  // setup for SPI on TDC, uncomment for SPI tests
  pinMode(PIN_SPI_CS, OUTPUT);
  digitalWrite(PIN_SPI_CS, HIGH);
  tdcSPI.begin()

}

void loop() {
  // put whatever testing function you want in here

}

//---------------------------------------------------------------------
// TEST FUNCTIONS
//---------------------------------------------------------------------

// use to test usb communication from mco to computer
// need Serial line uncommented
void basicSerial(){
  Serial.println("chip alive!");
  delay(1000);
}

// use to test UART communication from PCB to arduino
// need unoSerial line uncommented
void uartTest(){
  unoSerial.println("hello world (from pcb)");
  delay(1000);
}

// checks to see if chip can see DACs
// need Wire.begin line uncommented and Serial line to see on computer
void i2cScan(){
  for (byte address = 1; address < 127; address++){
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0){
      Serial.print("i2c device at 0x");
      Serial.println(address, HEX);
    }
  }
  Serial.println("i2c scan completed");
  delay(100000);
}

// DAC output test
// need wire line uncommented and Serial. test voltage output with multimeter
void writeDAC(uint16_t volt){
  volt = constrain(volt, 0.0, DAC_REF_VOLT);
  uint16_t dac_val = (uint16_t)((volt / DAC_REF_VOLT) * 4095.0);

  uint16_t val = constrain(dac_val, 0, 4095); // 4095 bc 12 bit DAC
  Wire.beginTransmission(addr);

  Wire.write(val >> 8);
  Wire.write(val & 0xFF);

  // check for errors
  byte error = Wire.endTransmission();

  if (error != 0){
    Serial.println("DAC write error");
  }

  delay(100000);
}

// SPI pin test
// check that CS goes to low, SCLK pulses, MOSI changes, and MISO responds or stays stable
void spiPin(){
  digitalWrite(PIN_SPI_CS, LOW);
  byte response = tdcSPI.transfer(0x00);
  digitalWrite(PIN_SPI_CS, HIGH);

  Serial.println(response, HEX);

  delay(10000);
}

// use to set tdc register than read back if its correct or not
// need tdc/spi setup uncommented
void tdcRegister(uint8_t reg, uint8_t val){
  // write to register
  uint8_t cmd = 0x40 | (reg & 0x3F);

  tdcSPI.beginTransaction(tdcSettings);
  digitalWrite(PIN_SPI_CS, LOW);
  tdcSPI.transfer(cmd);
  tdcSPI.transfer(val);
  digitalWrite(PIN_SPI_CS, HIGH);
  tdcSPI.endTransaction();

  delay(1000);

  // read register
  uint8_t val;

  tdcSPI.beginTransaction(tdcSettings);
  digitalWrite(PIN_SPI_CS, LOW);
  tdcSPI.transfer(cmd);
  val = tdcSPI.transfer(0x00);
  digitalWrite(PIN_SPI_CS, HIGH);
  tdcSPI.endTransaction();

  Serial.println(val);

  delay(10000);
}






