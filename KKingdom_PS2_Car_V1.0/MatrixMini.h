/**
 * @file MatrixMini.h
 * @author Zack Huang (zackhuang0513@gmail.com)
 * @brief Matrix Mini Example
 * @version 0.0.1
 * @date 2023-10-26
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef MATRIX_MINI_H
#define MATRIX_MINI_H
#include "BitConverter.h"
#include <Arduino.h>
#include <SoftwareSerial.h>

#define MatrixMini_COMM_LEAD     0xAA
#define MatrixMini_COMM_BAUDRATE 115200

class MatrixMini
{
public:
    MatrixMini(SoftwareSerial* uart);

    enum class COMM_CMD
    {
        // Application
        COMM_CMD_SET_ALL_MOTOR = 0x01,
        COMM_CMD_SET_ALL_SERVO = 0x02,
        // Button
        COMM_CMD_GET_BTN1_ST = 0x12,
        COMM_CMD_GET_BTN2_ST = 0x13,
        // Servo Config
        COMM_CMD_SET_SERVO_1_CFG = 0x04,
        COMM_CMD_SET_SERVO_2_CFG = 0x05,
        COMM_CMD_SET_SERVO_3_CFG = 0x06,
        COMM_CMD_SET_SERVO_4_CFG = 0x07,
        // Motor Config
        COMM_CMD_SET_ALL_MOTOR_CFG = 0x08,

    };

    enum class COMM_ERROR
    {
        COMM_TIMEOUT,
        COMM_NOT_FOUND_LEAD,
        COMM_NOT_FOUND_NLEAD,
        COMM_NOT_FOUND_CMD,
        COMM_OK,
    };

    bool Init();
    // Motor
    bool SetMotorSpeed(int16_t m1, int16_t m2, int16_t m3, int16_t m4);
    bool SetMotorMaxSpeed(int16_t m1, int16_t m2, int16_t m3, int16_t m4);
    // Servo
    bool SetServoAngle(uint16_t s1, uint16_t s2, uint16_t s3, uint16_t s4);
    bool SetServoConfig(
        uint8_t index, uint16_t minPulse, uint16_t maxPulse, uint16_t minAngle, uint16_t maxAngle);
    // User Button
    bool GetButton1State(void);
    bool GetButton2State(void);

private:
    SoftwareSerial* commSerial;

    void       CommSendData(COMM_CMD cmd, uint8_t* data = NULL, uint16_t size = 0);
    void       CommSendData(COMM_CMD cmd, uint8_t data);
    COMM_ERROR CommReadData(COMM_CMD cmd, uint8_t* data, uint16_t size, uint32_t timeout_ms);
    void       CommClear(void);
};

#endif   // MATRIX_MINI_V4_H
