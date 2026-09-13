#include <TimeControler.h>

TimeControler::TimeControler(unsigned long programTime, unsigned long currentTime)
                            :programTime(programTime), currentTime(currentTime) {}

void TimeControler::start()
{
    isStopper = (programTime == 0);
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
        if(!isStopper){
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
        if(!isStopper){
            programTime = (programTime > step) ? programTime - step : 0;
        }
    }
    else{
        step = getTimeStep(programTime);
        programTime = (programTime > step) ? programTime - step : 0;
    }
}

int TimeControler::getProgramTime() const
{
    return programTime;
}

int TimeControler::getTime(bool isFormatted) const
{   
    unsigned long time = programTime;
    if(isCounting){
        time = currentTime;
    }
    if(isFormatted){
        int seconds, minutes, hours;
        seconds = time % 60;
        minutes = time / 60 % 60;
        hours = time / 3600;

        int result;
        if(hours == 0){
            result = minutes*100 + seconds;
        }    
        else{
            result = hours * 100 + minutes;
        }
        return result;
    }
    return time;
}

unsigned long TimeControler::getProgramTime()
{
    return programTime;
}

void TimeControler::setProgramTime(unsigned long time)
{
    programTime = time;
}

unsigned long TimeControler::getCurrentTime()
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

int TimeControler::getTimeStep(unsigned long time)
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
