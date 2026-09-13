#include <TimeControler.h>

TimeControler::TimeControler(unsigned long programTime, unsigned long currentTime)
                            :programTime(programTime), currentTime(currentTime) {}

void TimeControler::start()
{
    isCounting = true;
    currentTime = programTime;
}

void TimeControler::stop()
{
    isCounting = false;
}

void TimeControler::stepUp()
{
    if(isCounting){
        unsigned long step = getTimeStep(currentTime);
        currentTime += step;
        if(!isStopper()){
            programTime += step;
        }
    }
    else{
        programTime += getTimeStep(programTime);
    }
}

void TimeControler::stepDown()
{
    unsigned long step;
    if(isCounting){
        step = getTimeStep(currentTime);
        currentTime = (currentTime > step) ? currentTime - step : 0; 
        if(!isStopper()){
            programTime = (programTime > step) ? programTime - step : 0;
        }
    }
    else{
        step = getTimeStep(programTime);
        programTime = (programTime > step) ? programTime - step : 0;
    }
}

unsigned long TimeControler::getTimeSeconds() const
{
    return isCounting ? currentTime : programTime;
}

int TimeControler::getFormattedTime() const
{
    unsigned long time = getTimeSeconds();

    int seconds = time % 60;
    int minutes = time / 60 % 60;
    int hours   = time / 3600;

    if(hours == 0){
        return minutes * 100 + seconds;
    }
    return hours * 100 + minutes;
}

unsigned long TimeControler::getProgramTime() const
{
    return programTime;
}

unsigned long TimeControler::getCurrentTime() const
{
    return currentTime;
}

void TimeControler::countDown()
{
    currentTime = (currentTime > 0) ? currentTime - 1 : 0;
}

void TimeControler::countUp()
{
    currentTime++;
}

bool TimeControler::isStopper() const
{
    return programTime == 0;
}

int TimeControler::getTimeStep(unsigned long time) const
{
    //LOG_DEBUG("getTimeStep() programTime=" << programTime );
    int step;
    if(time < 60* 2) step= 15;
    else if(time < 60* 5) step= 30;
    else if(time < 60* 30) step= 60;
    else if(time < 60* 60) step= 60 * 5;
    else step = 60 * 10;
    // LOG_DEBUG("return " << step );
    return step;
}
