#include <Arduino.h>
#include <pinout.h>
#include <RotaryEncoder.h>
#include <Adafruit_NeoPixel.h>
#include <TM1637Display.h>
#include <logger.h>
#include <Timer.h>

RotaryEncoder encoder (Pins::Encoder_A, Pins::Encoder_B, RotaryEncoder::LatchMode::FOUR3);
Timer encoderTimer (1);

// 1 dioda WS2812 na pinie Pins::LED
Adafruit_NeoPixel strip(1, Pins::LED, NEO_GRB + NEO_KHZ800);
// Wyświetlacz 7-segmentowy TM1637
TM1637Display display(Pins::DISPLAY_CLK, Pins::DISPLAY_DIO);

// Liczba dostępnych kolorów
constexpr uint8_t NUM_COLORS = 6;

// Ostatnio widziana pozycja (do wykrywania zmiany)
int lastPosition = -999;

void setup()
{
    Serial.begin(115200);

    strip.begin();
    strip.setBrightness(10);   // ogranicz jasność
    strip.show();

    display.setBrightness(4);  // jasność 0-7
    display.showNumberDec(0);  // wyczyść / pokaż startowe 0


    LOG_INFO("Encoder + LED start");
}

void loop()
{
	if(encoderTimer.isReady())
	{
		uint32_t in = REG_READ(GPIO_IN_REG);
		int sig1 = (in >> Pins::Encoder_A) & 1;
		int sig2 = (in >> Pins::Encoder_B) & 1;
		encoder.tick(sig1, sig2);
	}
    int position = encoder.getPosition();

    if (position != lastPosition) {
        lastPosition = position;

        // Obsługuje też ujemne pozycje (kręcenie w lewo)
        int index = ((position % NUM_COLORS) + NUM_COLORS) % NUM_COLORS;

        strip.setPixelColor(0, 0, 255, 0);
        strip.show();

        // Pokaż aktualną pozycję enkodera na wyświetlaczu TM1637
        display.showNumberDec(position);

        LOG_INFO("Pos: " << position << " -> kolor #" << index);
    }
}
