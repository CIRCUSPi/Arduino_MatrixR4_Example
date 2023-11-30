#include "SmartCamReader.h"
#include <MatrixR4.h>

unsigned int data[20];
unsigned int cX, cY, AREA;

SoftwareSerial softSerial(8, 9);   // rx, tx
MatrixR4       matrixR4(&softSerial);

void setup()
{
    Serial.begin(115200);
    Serial1.begin(115200);

    while (matrixR4.Init() != MatrixR4::RESULT::OK) {
        Serial.println(F("MatrixR4 Init Error..."));
        delay(100);
    }
    Serial.println("MatrixR4 Init OK...");
    delay(1000);
    matrixR4.SetDCMotorSpeed(3, 750, MatrixR4::DIR::REVERSE);
}

void loop()
{
    int result = SmartCamReader(data);
    if (result > 0) {
        cX   = data[0];
        cY   = data[1];
        AREA = data[2];

        matrixR4.SetStateLED(255, 0xFF0000);
        if (cX > 155 && cX >= 0 && cY < 200) {
            uint16_t angle = map(cX, 155, 320, 90, 140);
            matrixR4.SetServoAngle(3, angle);
        } else if (cX < 145 && cX >= 0 && cY < 200) {
            uint16_t angle = map(cX, 145, 0, 90, 50);
            matrixR4.SetServoAngle(3, angle);
        }

        Serial.print("X: ");
        Serial.println(cX);
        Serial.print("Y: ");
        Serial.println(cY);
        Serial.print("AREA: ");
        Serial.println(AREA);
    } else {
        matrixR4.SetStateLED(255, 0x0000FF);
        matrixR4.SetServoAngle(3, 90);
    }
}
