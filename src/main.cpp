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

#define SERIAL_BAUD_RATE 115200

#include <NESControllerInterface.h>
#include <Servo.h>
#include <pinDefs.h>
#include <printers.h>

#define NES_DATA_PIN 12
#define NES_LOAD_PIN 13
#define NES_CLOCK_PIN A7

#define FORWARD 0b01
#define BACKWARD 0b10
#define STOP 0b11

#define MAX_POWER 204
#define MAX_SPEED 100

Servo servo;
const int servoPin = 10;

NESControllerInterface nes(NES_DATA_PIN, NES_LOAD_PIN, NES_CLOCK_PIN);

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);

    pinMode(SHIFT_REG_DATA, OUTPUT);
    pinMode(SHIFT_REG_LATCH, OUTPUT);
    pinMode(SHIFT_REG_CLOCK, OUTPUT);

    pinMode(FRONT_LEFT_MOTOR_SPEED_PIN, OUTPUT);
    pinMode(FRONT_RIGHT_MOTOR_SPEED_PIN, OUTPUT);
    pinMode(BACK_LEFT_MOTOR_SPEED_PIN, OUTPUT);
    pinMode(BACK_RIGHT_MOTOR_SPEED_PIN, OUTPUT);

    analogWrite(FRONT_LEFT_MOTOR_SPEED_PIN, 0);
    analogWrite(FRONT_RIGHT_MOTOR_SPEED_PIN, 0);
    analogWrite(BACK_LEFT_MOTOR_SPEED_PIN, 0);
    analogWrite(BACK_RIGHT_MOTOR_SPEED_PIN, 0);

    servo.attach(servoPin);
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

uint8_t getDirectionCode(int8_t *speeds) {
    // Unlike the standard order of motors, (FL,FR,BL,BR).
    // The direction code is ordered as (BR, BL, FL, FR), where FR is the least
    // significant bit. BR BL FL FR

    uint8_t DirectionCode = 0;

    uint8_t FL_code;
    if (speeds[0] > 0) {
        FL_code = FORWARD;
    } else if (speeds[0] < 0) {
        FL_code = BACKWARD;
    } else {
        FL_code = STOP;
    }

    uint8_t FR_code;
    if (speeds[1] > 0) {
        FR_code = FORWARD;
    } else if (speeds[1] < 0) {
        FR_code = BACKWARD;
    } else {
        FR_code = STOP;
    }

    uint8_t BL_code;
    if (speeds[2] > 0) {
        BL_code = FORWARD;
    } else if (speeds[2] < 0) {
        BL_code = BACKWARD;
    } else {
        BL_code = STOP;
    }

    uint8_t BR_code;
    if (speeds[3] > 0) {
        BR_code = FORWARD;
    } else if (speeds[3] < 0) {
        BR_code = BACKWARD;
    } else {
        BR_code = STOP;
    }

    DirectionCode |= FR_code << 0;
    DirectionCode |= FL_code << 2;
    DirectionCode |= BL_code << 4;
    DirectionCode |= BR_code << 6;

    return DirectionCode;
}

void getSpeedsFromNESInput(NESInput input, int8_t *speeds) {
    // A percentage of the maximum speed.
    static uint8_t driveSpeed = MAX_SPEED / 2;

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
        driveSpeed = MAX_SPEED;
    } else if (input.buttonB) {
        driveSpeed = MAX_SPEED / 2;
    }

    // Set all the speeds to between -1 and 1.
    normalize(&FL_speed, &FR_speed, &BL_speed, &BR_speed);

    // Calculate the speeds of each motor as an integer between 0 and 255.
    int8_t flSpeed_abs = FL_speed * driveSpeed;
    int8_t frSpeed_abs = FR_speed * driveSpeed;
    int8_t blSpeed_abs = BL_speed * driveSpeed;
    int8_t brSpeed_abs = BR_speed * driveSpeed;

    // Set the speeds, which are returned by reference.
    speeds[0] = flSpeed_abs;
    speeds[1] = frSpeed_abs;
    speeds[2] = blSpeed_abs;
    speeds[3] = brSpeed_abs;
}

void getSpeedsFromSerial(int8_t *speeds) {
    uint8_t buffer[7];

    Serial.readBytes(buffer, 7);

    speeds[0] = buffer[1];
    speeds[1] = buffer[2];
    speeds[2] = buffer[3];
    speeds[3] = buffer[4];

    buffer[0]++;
    Serial.write(buffer, 7);
}

void calculateMotorPowers(int8_t *speeds, uint8_t *motorPowers) {
    // TODO add functionality to glide from the current speed to the new speed
    // smoothly.

    int16_t power;

    const float powerMultiplier = MAX_POWER / MAX_SPEED;

    for (int i = 0; i < 4; i++) {
        // If the speed is 0, set the power to max to facilitate a brake. This
        // relies on getDirectionCode() correctly setting the direction code to
        // 0b11.
        if (speeds[i] == 0) {
            power = MAX_POWER;
        } else {
            power = abs(speeds[i]) * powerMultiplier;
        }

        if (power > MAX_POWER) {
            power = MAX_POWER;
        }

        motorPowers[i] = power;
    }
}

void setMotors(int8_t *speeds) {
    uint8_t DirectionCode = getDirectionCode(speeds);

    uint8_t motorPowers[4];

    calculateMotorPowers(speeds, motorPowers);

    shiftOutWrapper(DirectionCode);
    analogWrite(FRONT_LEFT_MOTOR_SPEED_PIN, motorPowers[0]);
    analogWrite(FRONT_RIGHT_MOTOR_SPEED_PIN, motorPowers[1]);
    analogWrite(BACK_LEFT_MOTOR_SPEED_PIN, motorPowers[2]);
    analogWrite(BACK_RIGHT_MOTOR_SPEED_PIN, motorPowers[3]);
}

void loop() {
    NESInput input = nes.getNESInput();

    // Bipolar percentages representing the speed of each motor.
    static int8_t speeds[4] = {0, 0, 0, 0};

    static uint32_t lastCommandTime = 0;

    if (input.anyButtonPressed()) {
        getSpeedsFromNESInput(input, speeds);
        lastCommandTime = millis();
    } else if (Serial.available()) {
        getSpeedsFromSerial(speeds);
        lastCommandTime = millis();
    }

    if (lastCommandTime + 100 < millis()) {
        speeds[0] = 0;
        speeds[1] = 0;
        speeds[2] = 0;
        speeds[3] = 0;
    }

    setMotors(speeds);
}
