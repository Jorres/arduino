#include <Keyboard.h>

const int ECHO_L = 13;
const int TRIG_L = 11;
const int ECHO_R = 10;
const int TRIG_R = 9;

int get_single_dist(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  int duration = pulseIn(echoPin, HIGH);

  int cm = duration / 58;

  return cm;
}

int get_dist(int trigPin, int echoPin) {
  int sum = 0, smaller_sum = 0, larger = 0;
  int amount = 3;
  for (int i = 0; i < amount; i++) {
    delay(20);
    int t = get_single_dist(trigPin, echoPin);
    
    if (t > 60)
      larger++;
    else
      smaller_sum += t;
      
    sum += t;
  }
  
  return smaller_sum / (amount - larger);
}


int prev_value;
void setup() {
  Serial.begin (9600);

  pinMode(TRIG_L, OUTPUT);
  pinMode(ECHO_L, INPUT);
  pinMode(TRIG_R, OUTPUT);
  pinMode(ECHO_R, INPUT);
}

void loop() {
  int cur = get_dist(TRIG_L, ECHO_L);
  Serial.println(cur);
  if (cur != -1) {
      Keyboard.press('1');
  }
  if (cur == -1) {
      Keyboard.release('1');
  }
  
  cur = get_dist(TRIG_R, ECHO_R);
  if (cur != -1) {
      Keyboard.press('2');
  }
  if (cur == -1) {
      Keyboard.release('2');
  }
}

