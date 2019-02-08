#include <Keyboard.h>
//
const int FIR_REO = A0;
const int SEC_REO = A1;

const int XAXIS = A2;
const int YAXIS = A3;

const int ZAXIS = 7;

void setup() {
  Keyboard.begin();
  pinMode(FIR_REO, INPUT);
  pinMode(SEC_REO, INPUT);
  Serial.begin(9600);
}

int state = 0;
const int EPS = 1;

int data[23] = {0, 2, 4, 10, 20, 31, 48, 156, 281, 390, 470, 500, 590, 720, 823, 938, 987, 998, 1007, 1013, 1018, 1020, 1022};
void handle_throttler() {
    int current = analogRead(FIR_REO) + analogRead(SEC_REO);
    int i = 0;
    for (; i < 23 && data[i] < current; i++);
    i--;

  double res = i * 50;
  res += (double)(current - data[i]) / (data[i + 1] - data[i]);
  res = (int)(res / 50);
  
  if (res > state) {
    Keyboard.print('1');
    state++;
  } else if (state > res) {
    Keyboard.print('2');
    state--; 
  }
}

void handle_joystick() {
    int x_val = analogRead(XAXIS);
    int y_val = analogRead(YAXIS);
    /*Serial.print(x_val);
    Serial.print(" ");
    Serial.print(y_val);
    Serial.print("\n");*/
}

void loop() { 
  //handle_throttler();
  handle_joystick();
  delay(80);
}
