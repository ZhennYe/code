/**
 * Spike Detector based on Tay's Code
 *
  Spike detect watches the voltage and detects the onset and offsets of spikes
  It has two parameters, 
      Threshold: voltage when passed causes a spike to be detected.
      MinInterval: The minimum time it will allow between spikes detected

  The cell can be in one of 5 states
      State     Action
      0:   Looking for voltage to cross above threhsold;
      1:   Cell has crossed threshold going up;
      2:   Cell is above threshold;
      3:   Cell is crossing threshold going down
      4:   Depolarization block. cell has been above threshold for more than 100ms.
      -1:  Reset state. Will reset if cell hasn't spike since the minimum interval
*/
#include <math.h>
#include "SpikeDetect.h"

// constructors
// provides default values for paramters
SpikeDetect::SpikeDetect(void) {
    thresh = -20.0;
    minint = 30; 
    last_spike = 0;
    state = 0;
    count = 0;
}

SpikeDetect::SpikeDetect(double th, double mi) {
    thresh = th;
    minint = mi; 
    last_spike = 0;
    state = 0;
    count = 0;
}

SpikeDetect::~SpikeDetect(void) {}

/**
 * Update state
 *
 * v - membrane voltage
 * t - RTXI period RT::System::getInstance()->getPeriod()*1e-6
 * returns state
 */
int SpikeDetect::update(double v, double t) {
    
    // membrane voltage in volts
    double vm = v;
    
    // current time
    double time = count * t;
    
    switch (state) {
    case 0:
        if (vm > thresh) {
            state = 1;
            last_spike = time;
        }
        break;
	    
    case 1:
        state = 2;
        break;
    case 2:
        if (vm > thresh && (time - last_spike) > 100) {
            state = 4;
	    } else if (vm < thresh) {
            state = 3;
        }
        break;
    case 3:
        state = -1;
        break;
    case 4:
        if (vm < thresh) {
            state = -1;
        }
        break;
    case -1:
        if (time - last_spike > minint) {
            state = 0;
        }
	    
        break;
    default:
        break;
    }
    count++;
    return state;
}

int SpikeDetect::getState() {
    return state;
}

void SpikeDetect::setState(int s) {
    state = s;
}

double SpikeDetect::getMinInt() {
    return minint;
}

void SpikeDetect::setMinInt(double mi) {
    //printf("setting minint %f\n", mi);
    minint = mi;
}
