#include <Keyboard.h>

const int FIR_REO = A4;

const int F_XAXIS = A0;
const int F_YAXIS = A1;

const int S_XAXIS = A3;
const int S_YAXIS = A2;

void setup() {
  Serial.begin(9600);
 
  Keyboard.begin(); 
 
  pinMode(FIR_REO, INPUT);
  
  pinMode(F_XAXIS, INPUT);
  pinMode(F_YAXIS, INPUT);
  
  pinMode(S_XAXIS, INPUT);
  pinMode(S_YAXIS, INPUT);
}


int state = 0;
int data[23] = {0, 2, 4, 10, 20, 31, 48, 156, 281, 390, 470, 500, 590, 720, 823, 938, 987, 998, 1007, 1013, 1018, 1020, 1022};
void handleThrottler() {
    int current = analogRead(FIR_REO); // + analogRead(SEC_REO);
    int i = 0;
    for (; i < 23 && data[i] < current; i++);//
    i--;

  double res = i * 50;
  res += (double)(current - data[i]) / (data[i + 1] - data[i]);
  res = (int)(res / 50);
  
  if (res > state) {
    Keyboard.print('1');
    state++;
  } else if (state > res) {//21
    Keyboard.print('2');
    state--; 
  }
}

void handleJoystick(int xPin, int yPin, int num) {
    char data[2][4] = {'f', 'a', 'r', 'd', 'o', 'p', '[', ']'};
    int xVal = analogRead(xPin);//
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


void loop() { 
  //handleThrottler();
  
  handleJoystick(F_XAXIS, F_YAXIS, 0);
  handleJoystick(S_XAXIS, S_YAXIS, 1);

  delay(80);
}
