#include <SharpIR.h>

const int VOUT = A5;

void setup() {
    Serial.begin(9600);
}

SharpIR sensor(SharpIR::GP2Y0A21YK0F, VOUT);

void loop() {
    int distance = sensor.getDistance();
    Serial.println(distance);
    delay(100);
}
