#include <Arduino.h>
#include <pinout.h>
#include <RotaryEncoder.h>
#include <TM1637Display.h>
#include <logger.h>
#include <Timer.h>
#include <EasyButton.h>
#include "TimeControler.h"
#include "MusicBox.h"

enum class State{
    INIT,
    MINUTNIK_CNT,
    STOPER_CNT,
    MINUTNIK_PAUSE,
    STOPER_PAUSE,
    FINISH
};
State currentState = static_cast<State>(-1);

Timer secondCounter (1000);
Timer dotsCounter (500);
Timer finishBlinkTimer (500);
Timer finishExitTimer (20000);

bool displayDots=true;
bool shouldClear=true;
uint8_t dotOn = 1 << 6;

// hardware
RotaryEncoder encoder (Pins::Encoder_A, Pins::Encoder_B, RotaryEncoder::LatchMode::FOUR3);
EasyButton btn (Pins::BUTTON);
TM1637Display display(Pins::DISPLAY_CLK, Pins::DISPLAY_DIO);
MusicBox music (Pins::Buzzer);

TimeControler timeControler;

// Ostatnio widziana pozycja (do wykrywania zmiany)
int lastPosition = 0;
int encoderPosition;

void setState(State newState){
    
    if(currentState != newState){

        if(currentState == State::FINISH) music.melodyStop();

        switch (newState)
        {
            case State::INIT:
                timeControler.stop();
                lastPosition = encoder.getPosition();
                display.showNumberDecEx(timeControler.getFormattedTime(), dotOn, true);
                break;
            case State::MINUTNIK_CNT:
                secondCounter.reset();
                dotsCounter.reset();
                if(currentState == State::INIT){
                    timeControler.start();
                }
                break;
            case State::STOPER_CNT:
                secondCounter.reset();
                dotsCounter.reset();
                if(currentState == State::INIT){
                    timeControler.start();
                }
                break;
            case State::MINUTNIK_PAUSE:
            case State::STOPER_PAUSE:
                lastPosition = encoder.getPosition();
                display.showNumberDecEx(timeControler.getFormattedTime(), dotOn, true);
                break;
            case State::FINISH:
                finishExitTimer.reset();
                finishBlinkTimer.reset();
                shouldClear=false;
                music.melodyStart();   // Oda do radości, zapętlana
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
            if(timeControler.isStopper()){
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

void IRAM_ATTR onEncoderSample(){
    uint32_t in = REG_READ(GPIO_IN_REG);
    encoder.tick((in >> Pins::Encoder_A) & 1, (in >> Pins::Encoder_B) & 1);
}

void setup()
{
    Serial.begin(115200);

    btn.begin();
    btn.onPressed(onPressed);
    btn.onPressedFor(1000, onLongPressed);

    music.begin();
    music.setMelody(Melody::PIRATES);
   
    // timer sprzetowy do obslugi enkodera
    uint16_t timerPrescaler = 80;
    uint64_t timerValue = 1000;
    hw_timer_s *encTimer = timerBegin(0, timerPrescaler, true);
    timerAttachInterrupt(encTimer, onEncoderSample, true);
    timerAlarmWrite(encTimer, timerValue, true);
    timerAlarmEnable(encTimer);

    display.setBrightness(1);  // jasność 0-7
    display.showNumberDec(0);  // wyczyść / pokaż startowe 0
    setState(State::INIT);
}

void loop()
{
    btn.read();
    music.updatePlayNote();

    switch (currentState)
    {
        case State::INIT: 
            encoderPosition = encoder.getPosition();

            if (encoderPosition != lastPosition) {
                LOG_DEBUG("Zmiana pozycji enkodera");
                if (lastPosition < encoderPosition) {
                    music.playNote({Hz::A5, 100});
                    timeControler.stepUp();
                }
                else{
                    music.playNote({Hz::A4, 100});
                    timeControler.stepDown();
                }
                
                lastPosition = encoderPosition;
                display.showNumberDecEx(timeControler.getFormattedTime(), dotOn, true);
            }
            break;  
        case State::MINUTNIK_CNT:

            if(dotsCounter.isReady()){ 
                displayDots = !displayDots;
                display.showNumberDecEx(timeControler.getFormattedTime(), displayDots << 6, true);
            }

            if(secondCounter.isReady()){
                if(timeControler.getCurrentTime() == 0){
                    setState(State::FINISH);
                    break;
                }
                timeControler.countDown();
                display.showNumberDecEx(timeControler.getFormattedTime(), displayDots << 6, true);
            }
            break;
        case State::STOPER_CNT:
            if(secondCounter.isReady()){
                timeControler.countUp();
                display.showNumberDecEx(timeControler.getFormattedTime(), displayDots << 6, true);
            }
            if(dotsCounter.isReady()){
                displayDots = !displayDots;
                display.showNumberDecEx(timeControler.getFormattedTime(), displayDots << 6, true);
            }
            
            break;
        case State::MINUTNIK_PAUSE:
            encoderPosition = encoder.getPosition();

            if (encoderPosition != lastPosition) {
                if (lastPosition < encoderPosition) {
                    timeControler.stepUp();
                    music.playNote({Hz::A5, 100});
                }
                else {
                    timeControler.stepDown();
                    music.playNote({Hz::A4, 100});
                }
                
                lastPosition = encoderPosition;
                display.showNumberDecEx(timeControler.getFormattedTime(), dotOn, true);
            }

            break;
        case State::STOPER_PAUSE:
            encoderPosition = encoder.getPosition();

            if (encoderPosition != lastPosition) {
                if (lastPosition < encoderPosition) 
                {
                    timeControler.stepUp();
                    music.playNote({Hz::A5, 100});
                }
                else {
                    timeControler.stepDown();
                    music.playNote({Hz::A4, 100});
                }
                
                lastPosition = encoderPosition;
                display.showNumberDecEx(timeControler.getFormattedTime(), dotOn, true);
            }
            break;
        case State::FINISH:
            music.melodyUpdate();

            if(finishBlinkTimer.isReady()){
                if(shouldClear) {
                    display.clear();
                }
                else{
                    display.showNumberDecEx(timeControler.getFormattedTime(), dotOn, true);
                }
                shouldClear = !shouldClear;
            }
            if(finishExitTimer.isReady()){
                setState(State::INIT);
            }
            break;
    }
}
