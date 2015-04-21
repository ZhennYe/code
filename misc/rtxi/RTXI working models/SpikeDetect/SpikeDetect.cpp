/*
* Copyright (C) 2004 Boston University
*
*  This program is free software; you can redistribute it and/or
*  modify it under the terms of the GNU General Public License as
*  published by the Free Software Foundation; either version 2 of the
*  License, or (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*  General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <SpikeDetect.h>


extern "C" Plugin::Object *createRTXIPlugin(void) {
    return new SpikeDetect();
}

// inputs, outputs, parameters
static DefaultGUIModel::variable_t vars[] = {
    {
        "Vm",
        "Membrane Voltage (in mV)",
        DefaultGUIModel::INPUT,
    },
    {
        "State",
        "State Variable (0 - 4)",
        DefaultGUIModel::OUTPUT,
    },
    {
        "Threshold(mV)",
        "Threshold (mV) at which to detect a spike",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "Min Interval(s)",
        "Minimum interval(s) that must pass between spikes",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    }
};

// some necessary variable
static size_t num_vars = sizeof(vars)/sizeof(DefaultGUIModel::variable_t);

// constructor 
// provides default values for paramters, calls update(INIT)
SpikeDetect::SpikeDetect(void) : DefaultGUIModel("Spike Detector",::vars,::num_vars), thresh(-0.02), min_int(5e-3), last_spike(0),count(0),state(0) {
    update(INIT);
    refresh();
}

SpikeDetect::~SpikeDetect(void) {}

// execute is run every time step
void SpikeDetect::execute(void) {
    
    // membrane voltage
    double vm = input(0);
    
    // current time  MUST CONVERT TO MS
    double time = count * RT::System::getInstance()->getPeriod() * 1e-9;  // run time in s
    
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
            if (time - last_spike > min_int) {
                state = 0;
            }
	    
            break;
        default:
            break;
    }
    
    output(0) = state;  // send state information as output
    count++;
}

void SpikeDetect::update(DefaultGUIModel::update_flags_t flag) {
    switch(flag) {
        case INIT:
	  setParameter("Threshold(mV)",QString::number(thresh*1000.0));
	    setParameter("Min Interval(s)",QString::number(min_int));
	    break;
        case MODIFY:
	    thresh = getParameter("Threshold(mV)").toDouble()/1000.; // threshold is specified by user in mV
	    min_int = getParameter("Min Interval(s)").toDouble();
	    break;
        default:
	    break;
    }
}
