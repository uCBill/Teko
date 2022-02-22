#ifndef WATCHY_TEKO_H
#define WATCHY_TEKO_H

#include <Watchy.h>
#include "Teko_Regular40pt7b.h"
#include "Teko_Regular12pt7b.h"
#include "Teko_Regular20pt7b.h"
#include "Poky20pt7b.h"
#include "icons.h"

class WatchyTEKO : public Watchy{
    using Watchy::Watchy;
    public:
        void drawWatchFace();
        void drawTime();
        void drawDate();
        void drawSteps();
        void drawWeather();
        void drawBattery();
};

#endif
