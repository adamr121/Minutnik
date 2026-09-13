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
State currentState = static_cast<State>(-1);

int programTime = 0;
int currentTime = 0;

Timer encoderTimer (5);
Timer secondCounter (1000);
Timer dotsCounter (500);
Timer DoneBlinkTimer (500);
Timer finishTimer (20000);

bool displayDots=true;
bool shouldClear=true;

// hardware
RotaryEncoder encoder (Pins::Encoder_A, Pins::Encoder_B, RotaryEncoder::LatchMode::FOUR3);
EasyButton btn (Pins::BUTTON);
Adafruit_NeoPixel strip(1, Pins::LED, NEO_GRB + NEO_KHZ800);
TM1637Display display(Pins::DISPLAY_CLK, Pins::DISPLAY_DIO);

int getTimeStep(int programTime)
{
    LOG_DEBUG("getTimeStep() programTime=" << programTime );
    int step;
    if(programTime < 60* 2) step= 15;
    else if(programTime < 60* 5) step= 30;
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

void setState(State newState){
    
    if(currentState != newState){
        switch (newState)
        {
            case State::INIT:
                lastPosition = encoder.getPosition();
                display.showNumberDecEx(formatTime(programTime), 1 << 6, true);
                break;
            case State::MINUTNIK_CNT:
                secondCounter.reset();
                dotsCounter.reset();
                if(currentState == State::INIT){
                    currentTime = programTime;
                }
                break;
            case State::STOPER_CNT:
                secondCounter.reset();
                dotsCounter.reset();
                if(currentState == State::INIT){
                    currentTime = programTime;
                }
                break;
            case State::MINUTNIK_PAUSE:
            case State::STOPER_PAUSE:
                lastPosition = encoder.getPosition();
                display.showNumberDecEx(formatTime(currentTime), 1 << 6, true);
                break;
            case State::FINISH:
                finishTimer.reset();
                DoneBlinkTimer.reset();
                shouldClear=false;
                break;
        }
        currentState = newState;
    }
}
// Wywoływane przy każdym wciśnięciu przycisku enkodera
void onPressed()
{
    switch (currentState)
    {
        case State::INIT:
            if(programTime == 0){
                setState(State::STOPER_CNT);
            }
            else{
                setState(State::MINUTNIK_CNT);
            }
            break;
        case State::MINUTNIK_CNT:
            setState(State::MINUTNIK_PAUSE);
            break;
        case State::STOPER_CNT:
            setState(State::STOPER_PAUSE);
            break;
        case State::MINUTNIK_PAUSE:
            setState(State::MINUTNIK_CNT);
            break;
        case State::STOPER_PAUSE:
            setState(State::STOPER_CNT);
            break;
        case State::FINISH:
            setState(State::INIT);
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
        case State::STOPER_PAUSE:
            setState(State::INIT);  
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
    setState(State::INIT);
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

            if(dotsCounter.isReady()){ 
                displayDots = !displayDots;
                display.showNumberDecEx(formatTime(currentTime), displayDots << 6, true);
            }

            if(secondCounter.isReady()){
                if(currentTime == 0){
                    setState(State::FINISH);
                    break;
                }
                currentTime--;
                display.showNumberDecEx(formatTime(currentTime), displayDots << 6, true);
            }
            break;
        case State::STOPER_CNT:
            if(secondCounter.isReady()){
                currentTime++;
                display.showNumberDecEx(formatTime(currentTime), displayDots << 6, true);
            }
            if(dotsCounter.isReady()){
                display.showNumberDecEx(formatTime(currentTime), displayDots << 6, true);
                displayDots = !displayDots;
            }
            
            break;
        case State::MINUTNIK_PAUSE:
            encoderPosition = encoder.getPosition();

            if (encoderPosition != lastPosition) {
                if (lastPosition < encoderPosition) {
                    currentTime += getTimeStep(currentTime);
                    programTime += getTimeStep(currentTime);
                }
                else if (currentTime - getTimeStep(currentTime) >= 0) {
                    currentTime -= getTimeStep(currentTime);
                    programTime -= getTimeStep(currentTime);
                }
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
            if(finishTimer.isReady()){
                setState(State::INIT);
            }
            break;
    }

}
