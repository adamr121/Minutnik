#include "TM1637DisplayManual.h"

TM1637DisplayManual::TM1637DisplayManual(uint8_t clkPin, uint8_t dioPin)
    : clkPin(clkPin), dioPin(dioPin), brightness(4)
{
}

void TM1637DisplayManual::begin()
{
    pinMode(clkPin, OUTPUT);
    pinMode(dioPin, OUTPUT);
}

void TM1637DisplayManual::setBrightness(uint8_t brightness)
{
    // brightness 0-7 -> 000 - 111
    this->brightness = brightness;
    sendCommand(0x88 | brightness);
}

void TM1637DisplayManual::showNumber(int number)
{
    sendCommand(0x40);

    start();
    writeByte(0xC0);
    writeByte(SEGMENTS[number/1000 % 10]);
    writeByte(SEGMENTS[number/100 % 10]);
    writeByte(SEGMENTS[number/10 % 10]);
    writeByte(SEGMENTS[number % 10]);
    stop();

    setBrightness(brightness);
}

// --- Protokół TM1637 (bit-bang) ---

void TM1637DisplayManual::start()
{
    digitalWrite(dioPin, HIGH);
    digitalWrite(clkPin, HIGH);
    delayMicroseconds(2);
    digitalWrite(dioPin, LOW);
    delayMicroseconds(2);   
    digitalWrite(clkPin, LOW);
}

void TM1637DisplayManual::stop()
{
    digitalWrite(dioPin, LOW);
    digitalWrite(clkPin, HIGH);
    delayMicroseconds(2);
    digitalWrite(dioPin, HIGH);
    delayMicroseconds(2);
    digitalWrite(clkPin, LOW);
}

bool TM1637DisplayManual::writeByte(uint8_t data)
{
    for(uint8_t i=0; i < 8; i++){
        digitalWrite(clkPin, LOW);
        digitalWrite(dioPin, (data >> i) & 1);
        delayMicroseconds(2);
        digitalWrite(clkPin, HIGH);
        delayMicroseconds(2);
    }
    digitalWrite(clkPin, LOW);
    delayMicroseconds(2);

    pinMode(dioPin, INPUT_PULLUP);
    delayMicroseconds(2);
    digitalWrite(clkPin, HIGH);
    delayMicroseconds(2);
    bool ack = (digitalRead(dioPin) == LOW);
    digitalWrite(clkPin, LOW);
    delayMicroseconds(2);
    pinMode(dioPin, OUTPUT);

    return ack;
}

void TM1637DisplayManual::sendCommand(uint8_t cmd)
{
    start();
    writeByte(cmd);
    stop();
}
