#ifndef MUSICBOX_H
#define MUSICBOX_H

#include"Melody.h"

class MusicBox{
public:
    MusicBox(uint8_t BuzzerPin, uint8_t PWMChannel = 0);
    void begin();
    void setMelody(Melody melody);
    void playNote(Note note);
    void updatePlayNote();
    void melodyStop();
    void melodyStart();
    void melodyUpdate();

private:
    uint8_t buzzerPin;
    Melody selectedMelody;
    bool        melodyPlaying = false;
    size_t      melodyIndex   = 0;
    uint32_t    melodyNextAt  = 0;
    uint32_t    noteTimeEnd  = 0;
    bool        noteIsPlaying = false;
    const Note *melodyNotes   = nullptr;   // aktualnie grana melodia (ustawiane w melodyStart)
    size_t      melodyLength  = 0;
    const uint8_t  CHANNEL;
    const uint8_t  RESOLUTION = 10;    // 10 bit -> duty 0..1023
    const uint16_t DUTY       = 512;   // 50% = najgłośniej
};

#endif