/**
 * @file MatrixMini.cpp
 * @author Zack Huang (zackhuang0513@gmail.com)
 * @brief Matrix Mini Example
 * @version 0.0.1
 * @date 2023-10-26
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "MatrixMini.h"

MatrixMini::MatrixMini(SoftwareSerial* commSerial)
    : commSerial(commSerial)
{}

bool MatrixMini::Init()
{
    return commSerial->begin(MatrixMini_COMM_BAUDRATE, SERIAL_8N1);
}

bool MatrixMini::SetMotorSpeed(int16_t m1, int16_t m2, int16_t m3, int16_t m4)
{
    uint8_t buff[8];
    BitConverter::GetBytes(buff, m1);
    BitConverter::GetBytes(buff + 2, m2);
    BitConverter::GetBytes(buff + 4, m3);
    BitConverter::GetBytes(buff + 6, m4);

    CommClear();
    CommSendData(COMM_CMD::COMM_CMD_SET_ALL_MOTOR, buff, 8);

    uint8_t    respCode = 0x01;
    COMM_ERROR ret      = CommReadData(COMM_CMD::COMM_CMD_SET_ALL_MOTOR, &respCode, 1, 5);
    return (ret == COMM_ERROR::COMM_OK && respCode == 0x00);
}

bool MatrixMini::SetMotorMaxSpeed(int16_t m1, int16_t m2, int16_t m3, int16_t m4)
{
    uint8_t buff[8];
    BitConverter::GetBytes(buff, m1);
    BitConverter::GetBytes(buff + 2, m2);
    BitConverter::GetBytes(buff + 4, m3);
    BitConverter::GetBytes(buff + 6, m4);

    CommClear();
    CommSendData(COMM_CMD::COMM_CMD_SET_ALL_MOTOR_CFG, buff, 8);

    uint8_t    respCode = 0x01;
    COMM_ERROR ret      = CommReadData(COMM_CMD::COMM_CMD_SET_ALL_MOTOR_CFG, &respCode, 1, 5);
    return (ret == COMM_ERROR::COMM_OK && respCode == 0x00);
}

bool MatrixMini::SetServoAngle(uint16_t s1, uint16_t s2, uint16_t s3, uint16_t s4)
{
    uint8_t buff[8];
    BitConverter::GetBytes(buff, s1);
    BitConverter::GetBytes(buff + 2, s2);
    BitConverter::GetBytes(buff + 4, s3);
    BitConverter::GetBytes(buff + 6, s4);

    CommClear();
    CommSendData(COMM_CMD::COMM_CMD_SET_ALL_SERVO, buff, 8);

    uint8_t    respCode = 0x01;
    COMM_ERROR ret      = CommReadData(COMM_CMD::COMM_CMD_SET_ALL_SERVO, &respCode, 1, 5);
    return (ret == COMM_ERROR::COMM_OK && respCode == 0x00);
}

bool MatrixMini::SetServoConfig(
    uint8_t index, uint16_t minPulse, uint16_t maxPulse, uint16_t minAngle, uint16_t maxAngle)
{
    COMM_CMD cmd;
    switch (index) {
    case 1: cmd = COMM_CMD::COMM_CMD_SET_SERVO_1_CFG; break;
    case 2: cmd = COMM_CMD::COMM_CMD_SET_SERVO_2_CFG; break;
    case 3: cmd = COMM_CMD::COMM_CMD_SET_SERVO_3_CFG; break;
    case 4: cmd = COMM_CMD::COMM_CMD_SET_SERVO_4_CFG; break;
    default: return false;
    }

    uint8_t buff[8];
    BitConverter::GetBytes(buff, minPulse);
    BitConverter::GetBytes(buff + 2, maxPulse);
    BitConverter::GetBytes(buff + 4, minAngle);
    BitConverter::GetBytes(buff + 6, maxAngle);

    CommClear();
    CommSendData(cmd, buff, 8);

    uint8_t    respCode = 0x01;
    COMM_ERROR ret      = CommReadData(cmd, &respCode, 1, 5);
    return (ret == COMM_ERROR::COMM_OK && respCode == 0x00);
}

bool MatrixMini::GetButton1State(void)
{
    CommClear();
    CommSendData(COMM_CMD::COMM_CMD_GET_BTN1_ST);
    uint8_t    respCode = 0x00;
    COMM_ERROR ret      = CommReadData(COMM_CMD::COMM_CMD_GET_BTN1_ST, &respCode, 1, 5);
    if (ret == COMM_ERROR::COMM_OK) {
        return respCode;
    }
    return 0x00;
}

bool MatrixMini::GetButton2State(void)
{
    CommClear();
    CommSendData(COMM_CMD::COMM_CMD_GET_BTN2_ST);
    uint8_t    respCode = 0x00;
    COMM_ERROR ret      = CommReadData(COMM_CMD::COMM_CMD_GET_BTN2_ST, &respCode, 1, 5);
    if (ret == COMM_ERROR::COMM_OK) {
        return respCode;
    }
    return 0x00;
}

void MatrixMini::CommSendData(COMM_CMD cmd, uint8_t* data, uint16_t size)
{
    uint8_t  arr[3 + size];
    uint8_t* ptr = arr;

    *ptr++ = MatrixMini_COMM_LEAD;
    *ptr++ = ((~MatrixMini_COMM_LEAD) & 0xFF);
    *ptr++ = (uint8_t)cmd;

    for (uint16_t i = 0; i < size; i++) {
        *ptr++ = data[i];
    }
    commSerial->write(arr, 3 + size);
    commSerial->flush();
}

void MatrixMini::CommSendData(COMM_CMD cmd, uint8_t data)
{
    uint8_t _data[1] = {data};
    CommSendData((COMM_CMD)cmd, _data, 1);
}

MatrixMini::COMM_ERROR MatrixMini::CommReadData(
    COMM_CMD cmd, uint8_t* data, uint16_t size, uint32_t timeout_ms)
{
    uint8_t  totalLen = 3 + size;
    uint32_t time     = millis();
    while ((commSerial->available() < totalLen)) {
        if (millis() >= time + timeout_ms) {
            return COMM_ERROR::COMM_TIMEOUT;
        }
    }

    uint8_t buff[totalLen];
    for (uint8_t i = 0; i < totalLen; i++) {
        buff[i] = (uint8_t)commSerial->read();
    }

    uint8_t* ptr = buff;
    if (*ptr++ != MatrixMini_COMM_LEAD) {
        return COMM_ERROR::COMM_NOT_FOUND_LEAD;
    }
    if (*ptr++ != ((~MatrixMini_COMM_LEAD) & 0xFF)) {
        return COMM_ERROR::COMM_NOT_FOUND_NLEAD;
    }
    if (*ptr++ != (uint8_t)cmd) {
        return COMM_ERROR::COMM_NOT_FOUND_CMD;
    }

    for (uint8_t i = 0; i < size; i++) {
        data[i] = *ptr++;
    }
    return COMM_ERROR::COMM_OK;
}

void MatrixMini::CommClear(void)
{
    while (commSerial->available() > 0) commSerial->read();
}
