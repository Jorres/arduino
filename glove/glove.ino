#include <Keyboard.h>
#include <Wire.h>
#include "SharpIR.h"

const int I2C_L = 0x68;
const int I2C_R = 0x69;

const int F_XAXIS = A0;
const int F_YAXIS = A1;

const int S_XAXIS = A3;
const int S_YAXIS = A2;

const int VOUT = A4;

const int MINIMAL_HEIGHT = 15;
const int BURST_AMOUNT = 5;

void init(int address) { 
  Wire.beginTransmission(address);
  Wire.write(0x6B);
  Wire.write(0b00000000);
  Wire.endTransmission();  

  Wire.beginTransmission(address);
  Wire.write(0x1B);
  Wire.write(0x00000000);
  Wire.endTransmission(); 

  Wire.beginTransmission(address);
  Wire.write(0x1C);
  Wire.write(0b00000000);
  Wire.endTransmission(); 
}

void setup() {
  Wire.begin();
  init(I2C_L);
  init(I2C_R);

  Keyboard.begin(); 

  pinMode(F_XAXIS, INPUT);
  pinMode(F_YAXIS, INPUT);
  
  pinMode(S_XAXIS, INPUT);
  pinMode(S_YAXIS, INPUT);

  Serial.begin(9600); // he has other one
}

int accelX, accelY, accelZ;
double gyroX, gyroY, gyroZ;

void transmit(int address, int val) {
    Wire.beginTransmission(address);
    Wire.write(val);
    Wire.endTransmission();
}

void update_values(int address) {
    transmit(address, 0x3B);
    Wire.requestFrom(address, 6);
    while (Wire.available() < 6);
    accelX = (Wire.read() << 8) | Wire.read(); 
    accelY = (Wire.read() << 8) | Wire.read(); 
    accelZ = (Wire.read() << 8) | Wire.read(); 

    transmit(address, 0x43);
    Wire.requestFrom(address, 6);
    while (Wire.available() < 6);
    gyroX = (Wire.read() << 8) | Wire.read(); 
    gyroY = (Wire.read() << 8) | Wire.read();
    gyroZ = (Wire.read() << 8) | Wire.read(); 
}


void handleMPU(int address) {
    update_values(address);

    Serial.print(accelX);
    Serial.print(" ");
    Serial.print(accelY);
    Serial.print(" ");
    Serial.print(accelZ);
    Serial.print(" || ");

    Serial.print(gyroX);
    Serial.print(" ");
    Serial.print(gyroY);
    Serial.print(" ");
    Serial.print(gyroZ);
    Serial.println(" ");

    /*char data[2][4] = {'w', 'a', 's', 'd', 'i', 'j', 'k', 'l'};
    int xVal = accelX;
    int yVal = accelY;
    int joystickEps = 5000;
    int mid = -700;
    
    int num = 0;
    if (address == 0x69) {
        num = 1;
    }

    if (xVal > mid + joystickEps) {
      //Keyboard.press(data[num][0]);
      Serial.print(data[num][0]);
    } else {
      Keyboard.release(data[num][0]);
    }
    
    if (xVal < mid - joystickEps) {
      //Keyboard.press(data[num][2]);
      Serial.print(data[num][2]);
    } else {
      Keyboard.release(data[num][2]);
    }
    
    if (yVal > mid + joystickEps) {
      //Keyboard.press(data[num][1]);
      Serial.print(data[num][1]);
    } else {
      Keyboard.release(data[num][1]);
    }
    
    if (yVal < mid - joystickEps) {
      //Keyboard.press(data[num][3]);
      Serial.print(data[num][3]);
    } else {
      Keyboard.release(data[num][3]);
    }*/
}

void handleJoystick(int xPin, int yPin, int num) {
    char data[2][4] = {'f', 'a', 'r', 'd', 'o', 'p', '[', ']'};
    int xVal = analogRead(xPin);
    int yVal = analogRead(yPin);
    int joystickEps = 200;
    int mid = 512;

    if (xVal > mid + joystickEps) {
      Keyboard.press(data[num][0]);
    } else {
      Keyboard.release(data[num][0]);
    }
    
    if (xVal < mid - joystickEps) {
      Keyboard.press(data[num][2]);
    } else {
      Keyboard.release(data[num][2]);
    }
    
    if (yVal > mid + joystickEps) {
      Keyboard.press(data[num][1]);
    } else {
      Keyboard.release(data[num][1]);
    }
    
    if (yVal < mid - joystickEps) {
      Keyboard.press(data[num][3]);
    } else {
      Keyboard.release(data[num][3]);
    }
}

SharpIR sensor(SharpIR::GP2Y0A21YK0F, VOUT);

int getBurstDistance(bool burst) {
    int sum = 0;
    for (int i = 0; i < BURST_AMOUNT; i++) {
        sum += sensor.getDistance(burst);
    }
    return sum / BURST_AMOUNT;
}

int active = 0;
int abc = 0;
void loop() { 
    abc++;
    Serial.print(abc);
    Serial.print(" ");
    //handleJoystick(F_XAXIS, F_YAXIS, 0);
    //handleJoystick(S_XAXIS, S_YAXIS, 1);
  
  /*if (getBurstDistance(false) < MINIMAL_HEIGHT) {
    if (active == 0) {
      delay(1000);
    }
    active = 1;
    handleMPU(I2C_L);
    handleMPU(I2C_R);
  } else {
    active = 0;
  }*/

    handleMPU(I2C_L);
    handleMPU(I2C_R);

    delay(200);
}
