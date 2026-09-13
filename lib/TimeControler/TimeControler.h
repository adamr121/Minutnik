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
    // Aktualnie liczacy czas w sekundach (podczas liczenia currentTime, inaczej programTime).
    unsigned long getTimeSeconds() const;

    // Czas gotowy dla wyswietlacza: MMSS, a przy godzinach HHMM.
    int getFormattedTime() const;

    unsigned long getProgramTime() const;
    unsigned long getCurrentTime() const;
    void countDown();
    void countUp();
    bool isStopper() const;

private:
    int getTimeStep(unsigned long time) const;
    unsigned long programTime;
    unsigned long currentTime;

    bool isCounting = false;
};

#endif // TIMECONTROLER_H
