#ifndef MELODY_H
#define MELODY_H
#include <Arduino.h>

enum Melody{
    ODE_TO_JOY,
    PIRATES
};

struct Note{
    uint16_t freq;      // Hz, 0 = pauza
    uint16_t duration;  // ms
};

struct Tune{
    const Note *notes;
    size_t      length;
};

namespace Hz{
    constexpr uint16_t PAUSE = 0;
    constexpr uint16_t C4 = 262, D4 = 294, E4 = 330, F4 = 349, G4 = 392, A4 = 440, B4 = 494;
    constexpr uint16_t C5 = 523, D5 = 587, E5 = 659, F5 = 698, G5 = 784, A5 = 880, B5 = 988;
    constexpr uint16_t A5s = 932;   // A#5 / Bb5
}

namespace OdeToJoy{
    constexpr uint16_t Q  = 350;        // ćwierćnuta [ms] - 
    constexpr uint16_t E  = Q / 2;      // ósemka
    constexpr uint16_t DQ = Q * 3 / 2;  // ćwierćnuta z kropką
    constexpr uint16_t H  = Q * 2;      // półnuta

    constexpr Note NOTES[] = {
        // fraza A
        { Hz::E4, Q  }, { Hz::E4, Q  }, { Hz::F4, Q  }, { Hz::G4, Q  },
        { Hz::G4, Q  }, { Hz::F4, Q  }, { Hz::E4, Q  }, { Hz::D4, Q  },
        { Hz::C4, Q  }, { Hz::C4, Q  }, { Hz::D4, Q  }, { Hz::E4, Q  },
        { Hz::E4, DQ }, { Hz::D4, E  }, { Hz::D4, H  },
        // fraza B
        { Hz::E4, Q  }, { Hz::E4, Q  }, { Hz::F4, Q  }, { Hz::G4, Q  },
        { Hz::G4, Q  }, { Hz::F4, Q  }, { Hz::E4, Q  }, { Hz::D4, Q  },
        { Hz::C4, Q  }, { Hz::C4, Q  }, { Hz::D4, Q  }, { Hz::E4, Q  },
        { Hz::D4, DQ }, { Hz::C4, E  }, { Hz::C4, H  },
        // fraza A (powtórka)
        { Hz::E4, Q  }, { Hz::E4, Q  }, { Hz::F4, Q  }, { Hz::G4, Q  },
        { Hz::G4, Q  }, { Hz::F4, Q  }, { Hz::E4, Q  }, { Hz::D4, Q  },
        { Hz::C4, Q  }, { Hz::C4, Q  }, { Hz::D4, Q  }, { Hz::E4, Q  },
        { Hz::E4, DQ }, { Hz::D4, E  }, { Hz::D4, H  },
    };
}
namespace Pirates{
    constexpr uint16_t Q  = 390;        // ćwierćnuta [ms] - tempo
    constexpr uint16_t E  = Q / 2;      // ósemka

    constexpr Note NOTES[] = {
        // A C D D | D E F F
        { Hz::A4, E  }, { Hz::C5, E  }, { Hz::D5, Q  }, { Hz::D5, Q  },
        { Hz::D5, E  }, { Hz::E5, E  }, { Hz::F5, Q  }, { Hz::F5, Q  },
        // F G E E | D C C D
        { Hz::F5, E  }, { Hz::G5, E  }, { Hz::E5, Q  }, { Hz::E5, Q  },
        { Hz::D5, E  }, { Hz::C5, E  }, { Hz::C5, E  }, { Hz::D5, Q  },
        // A C D D | D E F F
        { Hz::A4, E  }, { Hz::C5, E  }, { Hz::D5, Q  }, { Hz::D5, Q  },
        { Hz::D5, E  }, { Hz::E5, E  }, { Hz::F5, Q  }, { Hz::F5, Q  },
        // F G E E | D C D
        { Hz::F5, E  }, { Hz::G5, E  }, { Hz::E5, Q  }, { Hz::E5, Q  },
        { Hz::D5, E  }, { Hz::C5, E  }, { Hz::D5, Q  },
        // A C D D | D F G G
        { Hz::A4, E  }, { Hz::C5, E  }, { Hz::D5, Q  }, { Hz::D5, Q  },
        { Hz::D5, E  }, { Hz::F5, E  }, { Hz::G5, Q  }, { Hz::G5, Q  },
        // G A A# A# | A G A D
        { Hz::G5, E  }, { Hz::A5, E  }, { Hz::A5s, Q }, { Hz::A5s, Q },
        { Hz::A5, E  }, { Hz::G5, E  }, { Hz::A5, E  }, { Hz::D5, Q  },
        // D E F F | G A D
        { Hz::D5, E  }, { Hz::E5, E  }, { Hz::F5, Q  }, { Hz::F5, Q  },
        { Hz::G5, Q  }, { Hz::A5, E  }, { Hz::D5, Q  },
        // D F E E | F D E
        { Hz::D5, E  }, { Hz::F5, E  }, { Hz::E5, Q  }, { Hz::E5, Q  },
        { Hz::F5, E  }, { Hz::D5, E  }, { Hz::E5, Q  },
    };
}

constexpr Tune TUNES[] = {
    { OdeToJoy::NOTES,    sizeof(OdeToJoy::NOTES)    / sizeof(Note) },
    { Pirates::NOTES,     sizeof(Pirates::NOTES)     / sizeof(Note) },
};

#endif