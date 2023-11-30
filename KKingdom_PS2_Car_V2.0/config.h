#ifndef CONFIG_H
#define CONFIG_H

// I2C MUX
#define PCA9548_ADDR 0x70

// User Setting
#define GAMEPAD_UPDATE_INTERVAL 20
#define SERVO1_UPDATE_INTERVAL  15
#define SERVO2_UPDATE_INTERVAL  4

// OLED
#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT  32
#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C

// WS2812
#define WS2812_PIN 7

// Buzzer
#define NOTE_C4    261.63
#define NOTE_D4    293.66
#define NOTE_E4    329.63
#define NOTE_F4    349.23
#define NOTE_G4    392.00
#define NOTE_A4    440.00
#define NOTE_B4    493.88
#define NOTE_C5    523.25
#define NOTE_E5    659.26
#define BUZZER_PIN 6

// GamePad
// CN8(D4)
#define PS2_DAT 13
#define PS2_CMD 10
// CN7(D3)
#define PS2_SEL 12
#define PS2_CLK 11

// Digital Write Fast
R_PORT0_Type* port_table[] = {
    R_PORT0, R_PORT1, R_PORT2, R_PORT3, R_PORT4, R_PORT5, R_PORT6, R_PORT7};

const uint16_t mask_table[] = {
    1 << 0,
    1 << 1,
    1 << 2,
    1 << 3,
    1 << 4,
    1 << 5,
    1 << 6,
    1 << 7,
    1 << 8,
    1 << 9,
    1 << 10,
    1 << 11,
    1 << 12,
    1 << 13,
    1 << 14,
    1 << 15};

#endif