#include <SoftwareSerial.h>

SoftwareSerial chipSerial(0, 1); // arduino rx pin 0, unused arduino tx pin


// pin setup for arduino motor driver
const int dirPin = 8;
const int stepPin = 9;


// num of steps per rev
cosnt int stepsPerRev = 1600;
// keeping track of current step
long cur_step = 0;
// time since last step in microseconds
unsigned long since_last = 0;
// time for motor step
const unsigned long step_time = 100;

// UART setup
char depth_buffer[32];
int depth_index = 0;


void setup() {
  Serial.begin(9600);
  chipSerial.begin(115200);

  // configure motor driver pins
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
}

void loop() {
  moveMotor();
  readDepth();

}

void moveMotor(){
  unsigned long now = micros();

  if (now - since_last >= step_time){
    // reset time since last step
    since_last = now;

    // move step
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(2);
    digitalWrite(stepPin, LOW);

    // increase current step
    cur_step ++;
  }
}

void readDepth(){ // recieve list of incoming points
  while(chipSerial.available() > 0){ // read in all values
    char c = chipSerial.read();

    if (c == '\n'){ // if last value to be read, print out dept
      depth_buffer[depth_index] = '\0';

      float depth = atof(depth_buffer); // convert to float

      Serial.print(cur_step);
      Serial.print(",");
      Serial.print(depth, 4);

      depth_index = 0;
    }
    else { // if in the middle of reading, keep adding characters to buffer
      if (depth_index < sizeof(depth_buffer) - 1){
        depth_buffer[depth_index++] = c;
      }
    }
  }

}
