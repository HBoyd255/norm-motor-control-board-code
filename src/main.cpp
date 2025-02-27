/**
 * @file main.cpp
 * @brief The main file and entry point of the project.
 *
 * @author Harry Boyd - https://github.com/HBoyd255
 * @date 30/10/2024
 * @copyright Copyright (c) 2024
 */

//  ██████╗ ██████╗  ██████╗      ██╗███████╗ ██████╗████████╗
//  ██╔══██╗██╔══██╗██╔═══██╗     ██║██╔════╝██╔════╝╚══██╔══╝
//  ██████╔╝██████╔╝██║   ██║     ██║█████╗  ██║        ██║
//  ██╔═══╝ ██╔══██╗██║   ██║██   ██║██╔══╝  ██║        ██║
//  ██║     ██║  ██║╚██████╔╝╚█████╔╝███████╗╚██████╗   ██║
//  ╚═╝     ╚═╝  ╚═╝ ╚═════╝  ╚════╝ ╚══════╝ ╚═════╝   ╚═╝
//
//  ███╗   ██╗    ██████╗    ██████╗    ███╗   ███╗
//  ████╗  ██║   ██╔═══██╗   ██╔══██╗   ████╗ ████║
//  ██╔██╗ ██║   ██║   ██║   ██████╔╝   ██╔████╔██║
//  ██║╚██╗██║   ██║   ██║   ██╔══██╗   ██║╚██╔╝██║
//  ██║ ╚████║██╗╚██████╔╝██╗██║  ██║██╗██║ ╚═╝ ██║██╗
//  ╚═╝  ╚═══╝╚═╝ ╚═════╝ ╚═╝╚═╝  ╚═╝╚═╝╚═╝     ╚═╝╚═╝

// (Neat Omnidirectional Robotic Module)

// Ascii text generated at https://patorjk.com/software/taag/
// Font used - ANSI Shadow

#include <Arduino.h>

#define SERIAL_BAUD_RATE 38400

#include <NESControllerInterface.h>
#include <pinDefs.h>

int speed = 200;

#define DATA_PIN 10
#define LOAD_PIN 11
#define CLOCK_PIN 12

#define FORWARD 0b01
#define BACKWARD 0b10

NESControllerInterface nes(DATA_PIN, LOAD_PIN, CLOCK_PIN);

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);

    pinMode(SHIFT_REG_DATA, OUTPUT);
    pinMode(SHIFT_REG_LATCH, OUTPUT);
    pinMode(SHIFT_REG_CLOCK, OUTPUT);

    pinMode(FRONT_LEFT_MOTOR_SPEED_PIN, OUTPUT);
    pinMode(FRONT_RIGHT_MOTOR_SPEED_PIN, OUTPUT);
    pinMode(BACK_LEFT_MOTOR_SPEED_PIN, OUTPUT);
    pinMode(BACK_RIGHT_MOTOR_SPEED_PIN, OUTPUT);

    analogWrite(FRONT_LEFT_MOTOR_SPEED_PIN, speed);
    analogWrite(FRONT_RIGHT_MOTOR_SPEED_PIN, speed);
    analogWrite(BACK_LEFT_MOTOR_SPEED_PIN, speed);
    analogWrite(BACK_RIGHT_MOTOR_SPEED_PIN, speed);

    pinMode(LED_BUILTIN, OUTPUT);
}

void shiftOutWrapper(uint8_t value) {
    digitalWrite(SHIFT_REG_LATCH, LOW);
    shiftOut(SHIFT_REG_DATA, SHIFT_REG_CLOCK, MSBFIRST, value);
    digitalWrite(SHIFT_REG_LATCH, HIGH);
}

int max(float a, float b, float c, float d) {
    {
        // checking if a is largest
        if (a >= b && a >= c && a >= d) {
            return a;
        }

        // checking if b is largest
        else if (b >= a && b >= c && b >= d) {
            return b;
        }
        // checking if c is largest
        else if (c >= a && c >= b && c >= d) {
            return c;
        }
        return d;
    }
}

void normalize(float *a, float *b, float *c, float *d) {
    int aABS = abs(*a);
    int bABS = abs(*b);
    int cABS = abs(*c);
    int dABS = abs(*d);

    int denominator = max(aABS, bABS, cABS, dABS);

    if (denominator < 1) {
        denominator = 1;
    }

    *a /= denominator;
    *b /= denominator;
    *c /= denominator;
    *d /= denominator;
}

uint8_t getDirectionCode(float FL_speed, float FR_speed, float BL_speed,
                         float BR_speed) {
    // Forwards = 01
    // Backwards = 10

    // Order = BR, BL, FL, FR

    uint8_t DirectionCode = 0;

    uint8_t FL_code = (FL_speed >= 0) ? FORWARD : BACKWARD;
    uint8_t FR_code = (FR_speed >= 0) ? FORWARD : BACKWARD;
    uint8_t BL_code = (BL_speed >= 0) ? FORWARD : BACKWARD;
    uint8_t BR_code = (BR_speed >= 0) ? FORWARD : BACKWARD;

    DirectionCode |= FR_code << 0;
    DirectionCode |= FL_code << 2;
    DirectionCode |= BL_code << 4;
    DirectionCode |= BR_code << 6;

    return DirectionCode;
}

void loop() {
    NESInput input = nes.getNESInput();

    static int driveSpeed = 100;

    float FL_speed = 0;
    float FR_speed = 0;
    float BL_speed = 0;
    float BR_speed = 0;

    if (input.buttonUp) {
        FL_speed += 1;
        FR_speed += 1;
        BL_speed += 1;
        BR_speed += 1;
    }
    if (input.buttonDown) {
        FL_speed += -1;
        FR_speed += -1;
        BL_speed += -1;
        BR_speed += -1;
    }
    if (input.buttonLeft) {
        FL_speed += -1;
        FR_speed += 1;
        BL_speed += 1;
        BR_speed += -1;
    }
    if (input.buttonRight) {
        FL_speed += 1;
        FR_speed += -1;
        BL_speed += -1;
        BR_speed += 1;
    }
    if (input.buttonSelect) {
        FL_speed += -1;
        FR_speed += 1;
        BL_speed += -1;
        BR_speed += 1;
    }
    if (input.buttonStart) {
        FL_speed += 1;
        FR_speed += -1;
        BL_speed += 1;
        BR_speed += -1;
    }

    if (input.buttonA) {
        driveSpeed = 200;
    } else if (input.buttonB) {
        driveSpeed = 100;
    }

    normalize(&FL_speed, &FR_speed, &BL_speed, &BR_speed);

    uint8_t DirectionCode =
        getDirectionCode(FL_speed, FR_speed, BL_speed, BR_speed);

    int flSpeed_abs = abs(FL_speed) * driveSpeed;
    int frSpeed_abs = abs(FR_speed) * driveSpeed;
    int blSpeed_abs = abs(BL_speed) * driveSpeed;
    int brSpeed_abs = abs(BR_speed) * driveSpeed;

    shiftOutWrapper(DirectionCode);

    analogWrite(FRONT_LEFT_MOTOR_SPEED_PIN, flSpeed_abs);
    analogWrite(FRONT_RIGHT_MOTOR_SPEED_PIN, frSpeed_abs);
    analogWrite(BACK_LEFT_MOTOR_SPEED_PIN, blSpeed_abs);
    analogWrite(BACK_RIGHT_MOTOR_SPEED_PIN, brSpeed_abs);

    Serial.print(" flSpeed_abs:");
    Serial.print(flSpeed_abs);
    Serial.print(" frSpeed_abs:");
    Serial.print(frSpeed_abs);
    Serial.print(" blSpeed_abs:");
    Serial.print(blSpeed_abs);
    Serial.print(" brSpeed_abs:");
    Serial.print(brSpeed_abs);
    Serial.print(" DirectionCode:");
    Serial.println(DirectionCode);
}
