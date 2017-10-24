#pragma once
#include "ofMain.h"

class timedThread : virtual ofThread
{
public:
	ofTimer timer;

	timedThread();
	virtual ~timedThread();
	void setup(float timesPerSec);
	void threadedFunction();
        virtual void loop() = 0;
        
        void lock() {lock();}
        void unlock() {unlock();}
};

