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

SPIClass tdcSPI(FSPI);

SPISettings tdcSettings(
  1000000, // SPI clock speed of 1 MHz
  MSBFIRST, // send most significant bit first, left to right
  SPI_MODE0 //clock polarity/phase mode 0, sent on rising edge
);

/////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS

// -------------------------------------------- TDC register helpers
// write to register function
void writeReg(uint8_t reg, uint8_t val){
  uint8_t cmd = 0x40 | (reg & 0x3F);

  tdcSPI.beginTransaction(tdcSettings);
  digitalWrite(PIN_SPI_CS, LOW);
  tdcSPI.transfer(cmd);
  tdcSPI.transfer(val);
  digitalWrite(PIN_SPI_CS, HIGH);
  tdcSPI.endTransaction();
}

uint8_t readReg(uint8_t reg){
  uint8_t cmd = reg & 0x3F;
  uint8_t val;

  tdcSPI.beginTransaction(tdcSettings);
  digitalWrite(PIN_SPI_CS, LOW);
  tdcSPI.transfer(cmd);
  val = tdcSPI.transfer(0x00);
  digitalWrite(PIN_SPI_CS, HIGH);
  tdcSPI.endTransaction();

  return val;
}

// ------------------------------------------------------- DAC helpers
// write to DACs
void writeDAC(uint8_t addr, uint16_t val){
  val = constrain(val, 0, 4095); // 4095 bc 12 bit DAC
  Wire.beginTransmission(addr);

  Wire.write(val >> 8);
  Wire.write(val & 0xFF);

  // check for errors
  byte error = Wire.endTransmission();

  if (error != 0){
    Serial.println("DAC write error");
  }

}

void setDacVolt(uint8_t addr, float volt){
  volt = constrain(volt, 0.0, DAC_REF_VOLT);
  uint16_t dac_val = (uint16_t)((volt / DAC_REF_VOLT) * 4095.0);

  writeDAC(addr, dac_val);
}

////////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(PIN_SPI_CS, OUTPUT);
  pinMode(PIN_TDC_ENABLE, OUTPUT);
  pinMode(PIN_INT1, INPUT_PULLUP);
  pinMode(PIN_INT2, INPUT_PULLUP);

  digitalWrite(PIN_SPI_CS, HIGH);

  // setup TDC
  digitalWrite(PIN_TDC_ENABLE, LOW);
  delay(5);
  digitalWrite(PIN_TDC_ENABLE, HIGH);
  delay(2); // needs time to start up


  pinMode(PIN_SPI_CS, OUTPUT);
  digitalWrite(PIN_SPI_CS, HIGH);

  // start SPI using OUT1 as master in slave out
  tdcSPI.begin(
    PIN_SPI_CLK,
    PIN_TDC_OUT1,
    PIN_TDC_IN,
    PIN_SPI_CS
  );

  // start I2C for both DACs
  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  Wire.setClock(100000);
  
  // set initial DAC voltages
  setDacVolt(VSET_DAC_ADDR, 1.0);
  setDacVolt(RECVR_DAC_ADDR, 1.2);

  Serial.println("completed setup");
}

void loop() {
  // put your main code here, to run repeatedly:

}
