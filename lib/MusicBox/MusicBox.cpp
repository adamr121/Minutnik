#include "MusicBox.h"

MusicBox::MusicBox(uint8_t buzzerPin, uint8_t PWMChannel) : buzzerPin(buzzerPin), CHANNEL(PWMChannel)
{
}

void MusicBox::begin()
{
    ledcSetup(CHANNEL, 1000, RESOLUTION);
    ledcAttachPin(buzzerPin, CHANNEL);
    ledcWrite(CHANNEL, 0);   // cisza na start
}

void MusicBox::setMelody(Melody melody)
{
    selectedMelody = melody;
}

void MusicBox::playNote(Note note)
{
    noteTimeEnd = millis() + note.duration;
    ledcWriteTone(CHANNEL, note.freq);
    ledcWrite(CHANNEL, DUTY);
    noteIsPlaying = true;
}

void MusicBox::updatePlayNote()
{
    if(noteIsPlaying && millis() > noteTimeEnd)
    {
        ledcWriteTone(CHANNEL, 0);
        ledcWrite(CHANNEL, 0);
        noteIsPlaying = false;
    }
}

void MusicBox::melodyStop()
{
    melodyPlaying = false;
    ledcWriteTone(CHANNEL, 0);
    ledcWrite(CHANNEL, 0);
}

void MusicBox::melodyStart()
{
    const Tune &tune = TUNES[static_cast<int> (selectedMelody)];

    melodyNotes   = tune.notes;
    melodyLength  = tune.length;
    melodyIndex   = 0;
    melodyPlaying = true;
    melodyNextAt  = millis() - 1;   // pierwsza nuta gra od razu
}
void MusicBox::melodyUpdate()
{
    if(!melodyPlaying) return;

    if((int32_t)(millis() - melodyNextAt) < 0) return;   // jeszcze nie czas - nic nie rób

    if(melodyIndex >= melodyLength){    // czas minął, a nut już nie ma -> koniec utworu
        melodyPlaying = false;
        ledcWrite(CHANNEL, 0);
        return;
    }

    const Note &note = melodyNotes[melodyIndex];

    if(note.freq == Hz::PAUSE){
        ledcWrite(CHANNEL, 0);          // pauza
    }
    else{
        ledcWriteTone(CHANNEL, note.freq);
        ledcWrite(CHANNEL, DUTY);
    }

    melodyNextAt += note.duration;
    // po dłuższym zablokowaniu loop() nie nadrabiaj serią szybkich nut
    if((int32_t)(millis() - melodyNextAt) >= 0) melodyNextAt = millis() + note.duration;

    ++melodyIndex;   // ostatnia nuta dogra swój czas; cisza dopiero przy następnym wywołaniu
}

