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
 
/*
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

#include <default_gui_model.h>

class SpikeDetect : public DefaultGUIModel
{

public:

    SpikeDetect(void);
    virtual ~SpikeDetect(void);

    // the main function run every time step, contains model logic
    virtual void execute(void);

protected:

    // run each time model parameters are updated
    virtual void update(DefaultGUIModel::update_flags_t);

private:

    // parameters
    double thresh;
    double min_int;
    
    // time of last spike
    double last_spike;
    
    // the internal state variable, sent as output
    int state;
    long long count;
};
