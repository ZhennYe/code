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

#include <default_gui_model.h>
#include "../include/DataLogger.cpp"

class SumPSG : public DefaultGUIModel
{ 
public:
    SumPSG(void);
    virtual ~SumPSG(void);

    virtual void execute(void);

protected:

    virtual void update(DefaultGUIModel::update_flags_t);

private:

    int isSpiking(double);

    // flag whether or not to use NMDA
    int useNMDA;
    int useConductance;

    // 2 exponentials for each synapse
    double exp1;
    double exp2;
    double exp3;
    double exp4;
    double exp5;
    double exp6;

    double gmaxNMDA; 
    double gmaxAMPA;
    double gmaxGABA;

    double esyn;
    double esyn_gaba;

    double nmda_slope;
    double nmda_midpoint;

    double PSGriseAMPA;
    double PSGfallAMPA;
    double PSGriseNMDA;
    double PSGfallNMDA;
    double PSGriseGABA;
    double PSGfallGABA;
    
    double rate;
    double rate_gaba;
    double PSG_template[2000];//make it big enough to handle really fast sampling rates

    double pdone;


    // DataLogger
    DataLogger data;
    double acquire, maxt, tcnt;
    int cellnum;
    string prefix, info;
    vector<double> newdata;

    // variable to count time continuously through the model's lifetime
    // for computing the modulation
    double stepcnt;
    double modamp;
    double modfreq;
    double excite_modamp;
    double excite_modfreq;

    double shift;
};
