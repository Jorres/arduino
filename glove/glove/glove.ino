#include <Keyboard.h>
#include <Wire.h>
#include <MPU6050.h>
#include <KalmanFilter.h>
#include "SharpIR.h"


const int I2C_L = 0x68;
const int I2C_R = 0x69;
const int VOUT = A4;
const int MINIMAL_HEIGHT = 11;
const double TIMESTEP = 0.003;

MPU6050 mpuL, mpuR;
struct sensor_state {
    double roll, pitch, yaw;    
    double kalPitch, kalRoll;
} L, R;

KalmanFilter kalXL(0.001, 0.003, 0.03), kalXR(0.001, 0.003, 0.03);
KalmanFilter kalYL(0.001, 0.003, 0.03), kalYR(0.001, 0.003, 0.03);

void setup() {
    Wire.begin();

    mpuL.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G, I2C_L);
    mpuL.calibrateGyro();
    mpuL.setThreshold(1);
    mpuR.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G, I2C_R);
    mpuR.calibrateGyro();
    mpuR.setThreshold(1);

    Keyboard.begin();
    Serial.begin(115200);
}

void local_print(char command) {
    Serial.println(command);
    //Keyboard.press(command);
}

void local_release(char command) {
    //Keyboard.release(command);
}

bool check_greater(int val, char command, double border) {
    if (val > border) {
      local_print(command);
    } else {
      local_release(command);
    }

    return val < border;
}

bool check_smaller(int val, char command, double border) {
    if (val < border) {
      local_print(command);
    } else {
      local_release(command);
    }

    return val > border;
}

double sqr(double a) {
  return a * a;
}

int iteration = 0;
bool handleMPU(MPU6050 *p, struct sensor_state *A, int num, KalmanFilter *pX, KalmanFilter *pY) {
    // Vector - library struct
    Vector norm = p->readNormalizeGyro();
    
    
    if (A->yaw * norm.ZAxis > 0) {
        norm.ZAxis /= 2;
    } else {
        norm.ZAxis *= 2;
    }

    A->yaw = A->yaw + norm.ZAxis * TIMESTEP;

    Vector acc = p->readNormalizeAccel();

    double accPitch = -(atan2(acc.XAxis, sqrt(sqr(acc.YAxis) + sqr(acc.ZAxis))) * 180.0) / M_PI;
    double accRoll  = (atan2(acc.YAxis, acc.ZAxis) * 180.0) / M_PI;

    A->pitch = pY->update(accPitch, norm.YAxis);
    A->roll = pX->update(accRoll, norm.XAxis);
  
    char data[2][6] = {{'1', '2', '3', '4', '5', '6'}, {'7', '8', '9', '0', '-', '='}};
    double limits[2][6] = {{22, 22, 35, 35, 6, 6}, {22, 22, 28, 35, 6, 6}};
    int BORDER = 6;
    bool change = true;
    change &= (check_greater(A->pitch, data[num][0], limits[num][0]) | check_smaller(A->pitch, data[num][1], -limits[num][1]));
    change &= (check_greater(A->roll, data[num][2], limits[num][2]) | check_smaller(A->roll, data[num][3], -limits[num][3]));
    change &= (check_greater(A->yaw, data[num][4], limits[num][4]) | check_smaller(A->yaw, data[num][5], -limits[num][5]));

    return change;
}

SharpIR detector(SharpIR::GP2Y0A21YK0F, VOUT);

int active = 0;
int staticL = 0, staticR = 0;
void loop() {
    unsigned long timer = millis();
    iteration++;
    if (detector.getDistance() < MINIMAL_HEIGHT) {
        Serial.println(1 - active);
        if (active == 0) {
            delay(2000);
        }
        mpuL.calibrateGyro();
        mpuR.calibrateGyro();
        L.pitch = L.roll = L.yaw = 0;
        R.pitch = R.roll = R.yaw = 0;
        active = 1 - active;
    } 

    if (active) {
        if (handleMPU(&mpuL, &L, 0, &kalXL, &kalYL)) {
          staticL++;
        } else {
          staticL = 0;
        }
        
        if (handleMPU(&mpuR, &R, 1, &kalXR, &kalYR)) {
          staticR++;
        } else {
          staticR = 0;
        }

        if (staticL > 400) {
          L.pitch = L.roll = L.yaw = 0;
          staticL = 0;
        }

        if (staticR > 400) {
          R.pitch = R.roll = R.yaw = 0;
          staticR = 0;
        }

        /*Serial.print(L.pitch);

        Serial.print(" ");
        Serial.print(L.roll); 
        
        Serial.print(" ");
        Serial.print(L.yaw); 
        Serial.println(" ");*/
    }
    
    long rem = (TIMESTEP * 1000) - (millis() - timer);
    if (rem > 0) {
      delay(rem); 
    }
}
