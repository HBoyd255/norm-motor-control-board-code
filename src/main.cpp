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

#define DATA_PIN 8
#define LOAD_PIN 9
#define CLOCK_PIN 10

#define MOTOR_ENA 7
#define MOTOR_IN1 6
#define MOTOR_IN2 5

NESControllerInterface nes(DATA_PIN, LOAD_PIN, CLOCK_PIN);

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);

    pinMode(LED_BUILTIN, OUTPUT);

    pinMode(MOTOR_ENA, OUTPUT);
    pinMode(MOTOR_IN1, OUTPUT);
    pinMode(MOTOR_IN2, OUTPUT);
}

void loop() {
    NESInput input = nes.getNESInput();

    digitalWrite(LED_BUILTIN, input.buttonA);

    static int speed = 0;

    if (input.buttonUp) {
        speed += 5;
    }
    if (input.buttonDown) {
        speed -= 5;
    }

    Serial.println(speed);

    int abs_speed = abs(speed);
    bool direction = (speed >= 0);

    if (input.buttonA) {
        analogWrite(MOTOR_ENA, 255);
        digitalWrite(MOTOR_IN1, 1);
        digitalWrite(MOTOR_IN2, 0);
    } else if (input.buttonB) {
        analogWrite(MOTOR_ENA, 255);
        digitalWrite(MOTOR_IN1, 0);
        digitalWrite(MOTOR_IN2, 1);
    } else {
        analogWrite(MOTOR_ENA, 255);
        digitalWrite(MOTOR_IN1, 1);
        digitalWrite(MOTOR_IN2, 1);
    }

    delay(100);
}

// Pin out
// https://docs.arduino.cc/static/74c80ed6511cded14f143abb4abd96a7/a6d36/pinout.png

// Motor modes

// Stopping

// Free spin.
// analogWrite(MOTOR_ENA, 0);
// digitalWrite(MOTOR_IN1, 0);
// digitalWrite(MOTOR_IN2, 0);

// Free spin.
// analogWrite(MOTOR_ENA, 0);
// digitalWrite(MOTOR_IN1, 1);
// digitalWrite(MOTOR_IN2, 1);

// Brake.
// analogWrite(MOTOR_ENA, 255);
// digitalWrite(MOTOR_IN1, 0);
// digitalWrite(MOTOR_IN2, 0);

// Brake.
// analogWrite(MOTOR_ENA, 255);
// digitalWrite(MOTOR_IN1, 1);
// digitalWrite(MOTOR_IN2, 1);

// Direction

// Anti-clockwise, (wheel at front)
// analogWrite(MOTOR_ENA, 255);
// digitalWrite(MOTOR_IN1, 1);
// digitalWrite(MOTOR_IN2, 0);

//// Clockwise, (wheel at front)
// analogWrite(MOTOR_ENA, 255);
// digitalWrite(MOTOR_IN1, 0);
// digitalWrite(MOTOR_IN2, 1);