/**
 * @file R4_Car_20231121.ino
 * @author Zack Huang (zackhuang0513@gmail.com)
 * @brief MatrixR4 RC Car Example
 * @version 0.0.1
 * @date 2023-11-21
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "Wire.h"
#include "config.h"
#include <Adafruit_SSD1306.h>
#include <MatrixR4.h>
#include <PS2X_lib.h>
#include <SoftwareSerial.h>

#define pinWS2812B_Set1() (digitalWriteFast(WS2812_PIN, 1))
#define pinWS2812B_Clr0() (digitalWriteFast(WS2812_PIN, 0))

const float BootScale[] = {NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5};

typedef struct
{
    int16_t  m1, m2;
    uint16_t s1, s2;
} MatrixVar;

SoftwareSerial   softSerial(8, 9);   // rx, tx
PS2X             ps2x;
MatrixVar        matrixVar;
MatrixR4         matrixR4(&softSerial);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET);

inline void digitalWriteFast(uint8_t pin, uint8_t val) __attribute__((always_inline, unused));
void        TaskGamePad(void);
void        TaskMatrixR4Comm(void);
void        SetWs2812(uint32_t led1, uint32_t led2);
void        Process_WS2812B_Protocol(uint8_t r, uint8_t g, uint8_t b);

void setup()
{
    Serial.begin(115200);
    while (matrixR4.Init() != MatrixR4::RESULT::OK) {
        Serial.println(F("MatrixR4 Init Error..."));
        delay(100);
    }

    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(WS2812_PIN, OUTPUT);
    pinWS2812B_Clr0();
    SetWs2812(0x00, 0x00);

    for (uint8_t i = 0; i < 4; i++) {
        tone(BUZZER_PIN, BootScale[i]);
        delay(200);
    }
    noTone(BUZZER_PIN);

    while (ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, false, false) != 0) {
        Serial.println(F("GamePad Config Error..."));
        delay(100);
    }
    Serial.println(F("GamePad Config Success"));

    Wire1.begin();
    while (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        delay(100);
    }

    MatrixR4::RESULT result;

    result = matrixR4.SetPowerParam(8.4f, 7.2f, 7.4f);   // 2 Cell
    if (result != MatrixR4::RESULT::OK) {
        Serial.print("SetPowerParam Failed! Result: ");
        Serial.println((int)result);
    } else {
        Serial.println("SetPowerParam Success!");
    }
    // Setting Motor1, Motor2 Direction
    result = matrixR4.SetDCMotorDir(1, MatrixR4::DIR::FORWARD);
    result = matrixR4.SetDCMotorDir(2, MatrixR4::DIR::FORWARD);
    // Setting Motor1, Motor2 Speed Range
    result = matrixR4.SetDCMotorSpeedRange(1, 0, 128);
    result = matrixR4.SetDCMotorSpeedRange(2, 0, 128);
    // Setting Servo1, Servo2 Pulse Range
    result = matrixR4.SetServoPulseRange(1, 1200, 2400);
    result = matrixR4.SetServoPulseRange(2, 908, 2400);
    // Setting Servo1, Servo2 Angle Range
    result = matrixR4.SetServoAngleRange(1, 0, 180);
    result = matrixR4.SetServoAngleRange(2, 0, 180);

    if (result != MatrixR4::RESULT::OK) {
        Serial.print("MatrixR4 Config Failed! Result: ");
        Serial.println((int)result);
    } else {
        Serial.println(F("MatrixR4 Config Success"));
    }

    // Init Servo1, Servo2 Angle
    matrixVar.s1 = 180;
    matrixVar.s2 = 180;
}

void loop()
{
    TaskGamePad();
    TaskMatrixR4Comm();
    DisplayBatterV();
}

void TaskGamePad(void)
{
    static uint32_t gamePadTimeout = 0;
    static uint32_t servo1Timeout  = 0;
    static uint32_t servo2Timeout  = 0;
    static bool     psb_L1_press   = false;
    static bool     psb_L2_press   = false;
    static bool     psb_R1_press   = false;
    static bool     psb_R2_press   = false;

    if (millis() >= gamePadTimeout) {
        gamePadTimeout = millis() + GAMEPAD_UPDATE_INTERVAL;
        ps2x.read_gamepad(false, 0);

        psb_L1_press = ps2x.Button(PSB_L1);
        psb_L2_press = ps2x.Button(PSB_L2);
        psb_R1_press = ps2x.Button(PSB_R1);
        psb_R2_press = ps2x.Button(PSB_R2);

        if (ps2x.ButtonPressed(PSB_GREEN)) {
            SetWs2812(0x00FF00, 0x00FF00);
        }

        if (ps2x.ButtonPressed(PSB_PINK)) {
            SetWs2812(0xfe249a, 0xfe249a);
        }

        if (ps2x.ButtonPressed(PSB_RED)) {
            SetWs2812(0xFF0000, 0xFF0000);
        }

        if (ps2x.ButtonPressed(PSB_BLUE)) {
            SetWs2812(0x0000FF, 0x0000FF);
        }

        if (ps2x.ButtonPressed(PSB_SELECT)) {
            SetWs2812(0x00, 0x00);
        }

        matrixVar.m1 = ((ps2x.Analog(PSS_LY) - 128) * -1);
        matrixVar.m2 = ((ps2x.Analog(PSS_RY) - 128) * -1);
    }
    if (millis() >= servo1Timeout) {
        servo1Timeout = millis() + SERVO1_UPDATE_INTERVAL;

        if (psb_L1_press && matrixVar.s1 < 180) {
            matrixVar.s1++;
        }

        if (psb_L2_press && matrixVar.s1 > 0) {
            matrixVar.s1--;
        }
    }
    if (millis() >= servo2Timeout) {
        servo2Timeout = millis() + SERVO2_UPDATE_INTERVAL;

        if (psb_R1_press && matrixVar.s2 < 180) {
            matrixVar.s2++;
        }

        if (psb_R2_press && matrixVar.s2 > 0) {
            matrixVar.s2--;
        }
    }
}

void TaskMatrixR4Comm(void)
{
    static uint32_t timeout = 0;

    MatrixR4::RESULT result;

    if (millis() >= timeout) {
        timeout = millis() + 20;

        MatrixR4::DIR m1Dir = (matrixVar.m1 > 0) ? MatrixR4::DIR::FORWARD : MatrixR4::DIR::REVERSE;
        MatrixR4::DIR m2Dir = (matrixVar.m2 > 0) ? MatrixR4::DIR::FORWARD : MatrixR4::DIR::REVERSE;
        if (matrixR4.SetDCMotorSpeed(1, abs(matrixVar.m1), m1Dir) != MatrixR4::RESULT::OK) {
            Serial.println("SetDCMotorSpeed Error");
        }
        if (matrixR4.SetDCMotorSpeed(2, abs(matrixVar.m2), m2Dir) != MatrixR4::RESULT::OK) {
            Serial.println("SetDCMotorSpeed Error");
        }

        if (matrixR4.SetServoAngle(1, matrixVar.s1) != MatrixR4::RESULT::OK) {
            Serial.println("SetServoAngle Error");
        }
        if (matrixR4.SetServoAngle(2, matrixVar.s2) != MatrixR4::RESULT::OK) {
            Serial.println("SetServoAngle Error");
        }
    }
}

inline void digitalWriteFast(uint8_t pin, uint8_t val)
{
    if (val) {
        port_table[g_pin_cfg[pin].pin >> 8]->POSR = mask_table[g_pin_cfg[pin].pin & 0xff];
    } else {
        port_table[g_pin_cfg[pin].pin >> 8]->PORR = mask_table[g_pin_cfg[pin].pin & 0xff];
    }
}

void SetWs2812(uint32_t led1, uint32_t led2)
{
    uint8_t r, g, b;
    r = ((led1 >> 16) & 0xFF);
    g = ((led1 >> 8) & 0xFF);
    b = (led1 & 0xFF);
    Process_WS2812B_Protocol(r, g, b);
    r = ((led2 >> 16) & 0xFF);
    g = ((led2 >> 8) & 0xFF);
    b = (led2 & 0xFF);
    Process_WS2812B_Protocol(r, g, b);
    delayMicroseconds(50);
}

void Process_WS2812B_Protocol(uint8_t r, uint8_t g, uint8_t b)
{
    volatile byte i, tempByte, m_Byte_R, m_Byte_G, m_Byte_B;

    m_Byte_R = r;
    m_Byte_G = g;
    m_Byte_B = b;

    noInterrupts();

    // Color: G
    for (i = 0; i < 8; i++) {
        pinWS2812B_Set1();

        if ((m_Byte_G & 0x80) == 0) {
            // asm("nop;");
            m_Byte_G <<= 1;
            tempByte <<= 1;
            pinWS2812B_Clr0();
            tempByte >>= 1;
            tempByte <<= 1;
        } else {
            for (uint32_t i = 0; i < 5; i++) {
                asm("nop;");
            }
            m_Byte_G <<= 1;
            pinWS2812B_Clr0();
        }
    }

    // Color: R
    for (i = 0; i < 8; i++) {
        pinWS2812B_Set1();

        if ((m_Byte_R & 0x80) == 0) {
            // asm("nop;");
            m_Byte_R <<= 1;
            tempByte <<= 1;
            pinWS2812B_Clr0();
            tempByte >>= 1;
            tempByte <<= 1;
        } else {
            for (uint32_t i = 0; i < 5; i++) {
                asm("nop;");
            }
            m_Byte_R <<= 1;
            pinWS2812B_Clr0();
        }
    }

    // Color: B
    for (i = 0; i < 8; i++) {
        pinWS2812B_Set1();

        if ((m_Byte_B & 0x80) == 0) {
            // asm("nop;");
            m_Byte_B <<= 1;
            tempByte <<= 1;
            pinWS2812B_Clr0();
            tempByte >>= 1;
            tempByte <<= 1;
        } else {
            for (uint32_t i = 0; i < 5; i++) {
                asm("nop;");
            }
            m_Byte_B <<= 1;
            pinWS2812B_Clr0();
        }
    }

    interrupts();
}

void DisplayBatterV(void)
{
    static uint32_t timer = 0;

    if (millis() >= timer) {
        timer          = millis() + 1000;
        float batteryV = 0.0f, batteryP = 0.0f;
        if (matrixR4.GetPowerInfo(batteryV, batteryP) == MatrixR4::RESULT::OK) {
            display.clearDisplay();
            display.setTextSize(2);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(13, 10);
            display.print(String(batteryV, 2) + "V");
            display.setCursor(85, 10);
            display.print(String(batteryP, 0) + "%");
            display.display();
        }
    }
}
