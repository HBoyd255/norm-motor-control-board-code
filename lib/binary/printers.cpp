
#include <printers.h>

void printByte(uint8_t byteToPrint) {
    for (uint8_t b = 7; b != 0; b--) {
        Serial.print((byteToPrint >> b) & 1);
        Serial.print("");
    }
    Serial.println(byteToPrint & 1);
}

void printSpeeds(int8_t *speeds) {
    Serial.print("Speeds: ");
    Serial.print(" FL:");
    Serial.print(speeds[0]);
    Serial.print(" FR:");
    Serial.print(speeds[1]);
    Serial.print(" BL:");
    Serial.print(speeds[2]);
    Serial.print(" BR:");
    Serial.println(speeds[3]);
}