#include <Arduino.h>
#include <pinout.h>
#include <RotaryEncoder.h>
#include <Adafruit_NeoPixel.h>
#include <TM1637Display.h>
#include <logger.h>
#include <Timer.h>
#include <EasyButton.h>

enum class State{
    INIT,
    MINUTNIK_CNT,
    STOPER_CNT,
    MINUTNIK_PAUSE,
    STOPER_PAUSE,
    FINISH
};
State currentState;

int programTime = 0;
int currentTime = 0;

RotaryEncoder encoder (Pins::Encoder_A, Pins::Encoder_B, RotaryEncoder::LatchMode::FOUR3);
Timer encoderTimer (1);
Timer secondCounter (1000);
Timer dotsCounter (500);
Timer DoneBlinkTimer (500);
bool displayDots=true;
bool shouldClear=true;

EasyButton btn (Pins::BUTTON);
// 1 dioda WS2812 na pinie Pins::LED
Adafruit_NeoPixel strip(1, Pins::LED, NEO_GRB + NEO_KHZ800);
// Wyświetlacz 7-segmentowy TM1637
TM1637Display display(Pins::DISPLAY_CLK, Pins::DISPLAY_DIO);

int getTimeStep(int programTime)
{
    LOG_DEBUG("getTimeStep() programTime=" << programTime );
    int step;
    if(programTime < 60* 3) step= 15;
    else if(programTime < 60* 7) step= 30;
    else if(programTime < 60* 30) step= 60;
    else if(programTime < 60* 60) step= 60 * 5;
    else step = 60 * 10;
    LOG_DEBUG("return " << step );
    return step;
}

int formatTime( int programTime){
    LOG_DEBUG("formatTime() programTime=" << programTime );
    
    int seconds, minutes, hours;
    seconds = programTime % 60;
    minutes = programTime/60 % 60;
    hours = programTime/3600;

    int result;
    if(hours == 0){
        result = minutes*100 + seconds;
    }    
    else{
        result = hours * 100 + minutes;
    }
    LOG_DEBUG("return " << result );
    return result;
}
// Ostatnio widziana pozycja (do wykrywania zmiany)
int lastPosition = 0;
int encoderPosition;
// Wywoływane przy każdym wciśnięciu przycisku enkodera
void onPressed()
{
    switch (currentState)
    {
        case State::INIT:
            secondCounter.reset();
            dotsCounter.reset();
            if(programTime == 0){
                currentState = State::STOPER_CNT;
                currentTime = programTime;
            }
            else{
                currentState = State::MINUTNIK_CNT;
                currentTime = programTime;
            }
            break;
        case State::MINUTNIK_CNT:
            currentState = State::MINUTNIK_PAUSE;
            display.showNumberDecEx(formatTime(currentTime), 1 << 6, true);
            break;
        case State::STOPER_CNT:
            currentState = State::STOPER_PAUSE;
            display.showNumberDecEx(formatTime(currentTime), 1 << 6, true);
            break;
        case State::MINUTNIK_PAUSE:
            secondCounter.reset();
            dotsCounter.reset();
            currentState = State::MINUTNIK_CNT;
            break;
        case State::STOPER_PAUSE:
            secondCounter.reset();
            dotsCounter.reset();
            currentState = State::STOPER_CNT;
            break;
        case State::FINISH:
            currentState = State::INIT;
            display.showNumberDecEx(formatTime(programTime), 1 << 6, true);
            break;
    }
}
void onLongPressed(){
    switch (currentState)
    {
        case State::INIT:

            break;
        case State::MINUTNIK_CNT:

            break;
        case State::STOPER_CNT:

            break;
        case State::MINUTNIK_PAUSE:
            currentState = State::INIT;
            display.showNumberDecEx(formatTime(programTime), 1 << 6, true);
            break;
        case State::STOPER_PAUSE:
            currentState = State::INIT;
            display.showNumberDecEx(formatTime(programTime), 1 << 6, true);
            break;
        case State::FINISH:
            
            break;
    }
}
void setup()
{
    Serial.begin(115200);

    strip.begin();
    strip.setBrightness(10);   // ogranicz jasność
    strip.show();

    btn.begin();
    btn.onPressed(onPressed);
    btn.onPressedFor(1000, onLongPressed);

    display.setBrightness(1);  // jasność 0-7
    display.showNumberDec(0);  // wyczyść / pokaż startowe 0
    currentState = State::INIT;

    display.showNumberDecEx(programTime, 1 << 6, true);
    LOG_INFO("Encoder + LED start");
}

void loop()
{
    btn.read();

	if(encoderTimer.isReady())
	{
		uint32_t in = REG_READ(GPIO_IN_REG);
		int sig1 = (in >> Pins::Encoder_A) & 1;
		int sig2 = (in >> Pins::Encoder_B) & 1;
		encoder.tick(sig1, sig2);
	}

    switch (currentState)
    {
        case State::INIT: 
            encoderPosition = encoder.getPosition();

            if (encoderPosition != lastPosition) {
                LOG_DEBUG("Zmiana pozycji enkodera");
                if (lastPosition < encoderPosition) programTime += getTimeStep(programTime);
                else if (programTime - getTimeStep(programTime) >= 0) programTime -= getTimeStep(programTime);
                lastPosition = encoderPosition;
                display.showNumberDecEx(formatTime(programTime), 1 << 6, true);
            }
            break;  
        case State::MINUTNIK_CNT:

            if(secondCounter.isReady()){
                if(currentTime == 0){
                    currentState = State::FINISH;
                    break;
                }
                currentTime--;
            }

            if(dotsCounter.isReady()){
                display.showNumberDecEx(formatTime(currentTime), displayDots << 6, true);
                displayDots = !displayDots;
            }
            
            break;
        case State::STOPER_CNT:
            if(secondCounter.isReady()){
                currentTime++;
            }
            if(dotsCounter.isReady()){
                display.showNumberDecEx(formatTime(currentTime), displayDots << 6, true);
                displayDots = !displayDots;
            }
            
            break;
        case State::MINUTNIK_PAUSE:
            encoderPosition = encoder.getPosition();

            if (encoderPosition != lastPosition) {
                if (lastPosition < encoderPosition) currentTime += getTimeStep(currentTime);
                else if (currentTime - getTimeStep(currentTime) >= 0) currentTime -= getTimeStep(currentTime);
                lastPosition = encoderPosition;
                display.showNumberDecEx(formatTime(currentTime), 1 << 6, true);
            }

            break;
        case State::STOPER_PAUSE:
            encoderPosition = encoder.getPosition();

            if (encoderPosition != lastPosition) {
                if (lastPosition < encoderPosition) currentTime += getTimeStep(currentTime);
                else if (currentTime - getTimeStep(currentTime) >= 0) currentTime -= getTimeStep(currentTime);
                lastPosition = encoderPosition;
                display.showNumberDecEx(formatTime(currentTime), 1 << 6, true);
            }
            break;
        case State::FINISH:
            if(DoneBlinkTimer.isReady()){
                if(shouldClear) {
                    display.clear();
                }
                else{
                    display.showNumberDecEx(formatTime(currentTime), 1 << 6, true);
                }
                shouldClear = !shouldClear;
            }
            break;
    }

}
