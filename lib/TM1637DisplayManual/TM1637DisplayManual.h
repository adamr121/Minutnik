#ifndef TM1637DISPLAYMANUAL_H
#define TM1637DISPLAYMANUAL_H

#include <Arduino.h>

// Klasa do obsługi wyświetlacza 7-segmentowego TM1637 przez bit-bang
// (sterowanie pinami CLK i DIO bezpośrednio z GPIO — bez gotowej biblioteki).
class TM1637DisplayManual
{
public:
    TM1637DisplayManual(uint8_t clkPin, uint8_t dioPin);

    // Inicjalizacja pinów
    void begin();

    // Jasność 0-7 (0 = wyłączony, 7 = najjaśniejszy)
    void setBrightness(uint8_t brightness);

    // Wyświetlenie liczby (całkowitej) na 4 cyfrach
    void showNumber(int number);

private:
    // --- Protokół TM1637 (bit-bang) ---
    void start();          // sygnał START
    void stop();           // sygnał STOP
    bool writeByte(uint8_t data);  // wyślij bajt, zwróć bit ACK
    void sendCommand(uint8_t cmd); // wyślij polecenie (tryb/adres/jasność)

    uint8_t clkPin;   // pin CLK
    uint8_t dioPin;   // pin DIO
    uint8_t brightness;  // aktualna jasność 0-7
    static constexpr uint8_t SEGMENTS[10] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F };
};

#endif // TM1637DISPLAYMANUAL_H
