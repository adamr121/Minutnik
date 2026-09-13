#ifndef TIMECONTROLER_H
#define TIMECONTROLER_H

#include <Arduino.h>

class TimeControler {

public:
    TimeControler(unsigned long programTime = 0, unsigned long currentTime = 0);

    void start();
    void stop();
    void stepUp();
    void stepDown();
    int getProgramTime() const;
    int getTime(bool isFormatted = true) const;
    unsigned long getProgramTime();
    void setProgramTime(unsigned long time);
    unsigned long getCurrentTime();
    void countDown();
    void countUp();

private:
    int getTimeStep(unsigned long time);
    unsigned long programTime;
    unsigned long currentTime;

    bool isCounting = false;
    bool isStopper = true;
};

#endif // TIMECONTROLER_H
