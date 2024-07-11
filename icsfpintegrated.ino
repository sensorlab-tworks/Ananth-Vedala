#include <Adafruit_ADS1X15.h>
const int inputMin = 0;
const int inputMax = 100;
const int outputMin = 0;
const int outputMax = 1023;

// Function to map the input range to the output range

Adafruit_ADS1115 ads;  // Use this for the 16-bit version
const int pwmPin = 9;
int TSL230_Pin = 4;    // TSL230 output
int TSL230_s0 = 3;     // TSL230 sensitivity setting 1
int TSL230_s1 = 2;     // TSL230 sensitivity setting 2
int TSL230_samples = 6;
int speed = 0;
int intr = 0;
bool enable = false;
bool receive = false;
String command;

void setup() {
  Serial.begin(9600);
  pinMode(3, OUTPUT);
  ads.setGain(GAIN_TWOTHIRDS);
  setupTSL230();
   if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }
}

void loop() {
  int16_t adc0;
  float volts0;
  adc0 = ads.readADC_SingleEnded(0);
  volts0 = ads.computeVolts(adc0);
  float lightLevel = readTSL230(TSL230_samples);
  float temp = volts0*100;
  Func();
}

void serialEvent() {
  while (Serial.available()) {
    char ch = (char)Serial.read();
    if (ch == '$') {
      receive = true;
      command = "";
      continue; // so that the command is not reset
    }

    if (ch == '@') {
      receive = false;
      enable = true;
      intr = 1;
    }

    if (receive) {
      command += ch;
    }
  }
}

int mapToPWM(int input) {
  return map(input, inputMin, inputMax, outputMin, outputMax);
}

void Func() {
  if (intr == 1) {
    String p1 = command.substring(4, 5);
    String p2 = command.substring(6, 7);
    String p3 = command.substring(7, 10);
    /*int input = p3.toInt();
    //Serial.print(speed);
    speed = mapToPWM(input);*/
   
    speed = mapToPWM(p3.toInt());

    analogWrite(pwmPin,speed);
    Serial.print(ads.computeVolts(ads.readADC_SingleEnded(0))*100);
    Serial.print(",");
    Serial.print(readTSL230(TSL230_samples));
    Serial.print(",");
    Serial.println(speed);
    delay(10);
    intr = 0;

    //Serial.print(p1.toInt());
    //Serial.print(",");
    //Serial.print(p2.toInt());
    //Serial.print(",");
    //Serial.println(p3.toInt());
  }
}

void setupTSL230() {
  pinMode(TSL230_s0, OUTPUT);
  pinMode(TSL230_s1, OUTPUT);
  // configure sensitivity - Can set to
  // S1 LOW  | S0 HIGH: low
  // S1 HIGH | S0 LOW:  med
  // S1 HIGH | S0 HIGH: high
  digitalWrite(TSL230_s1, LOW);
  digitalWrite(TSL230_s0, HIGH);
}

float readTSL230(int samples) {
  // sample light, return reading in frequency
  // higher number means brighter
  float start = micros();
  int readings = 0;
  while (readings < samples) {
    pulseIn(TSL230_Pin, HIGH);
    readings++;
  }
  float length = micros() - start;
  float freq = (1000000 / (length / samples)) * 10;
  return freq;
}
