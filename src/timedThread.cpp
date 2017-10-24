#include "timedThread.h"

timedThread::timedThread()
{
}

timedThread::~timedThread()
{
}

void timedThread::setup(float timesPerSec) {
	timer.setPeriodicEvent(1/timesPerSec * 1000000000);
        startThread(true);
}

void timedThread::threadedFunction() {
    while(isThreadRunning()) {
        loop();
        timer.waitNext();
    }
}

