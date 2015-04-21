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

#include <Istep.h>
#include <iostream>
#include <sstream>
     

extern "C" Plugin::Object *createRTXIPlugin(void) {
    return new Istep();
}

static DefaultGUIModel::variable_t vars[] = {
    {
        "Vin",
        "",
        DefaultGUIModel::INPUT,
    },
    {
        "PSGin",
        "",
        DefaultGUIModel::INPUT,
    },
    {
        "Iout",
        "",
        DefaultGUIModel::OUTPUT,
    },
    {
        "Period (sec)",
        "Duration of one cycle",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "Delay (sec)",
        "Time until step starts from beginning of cycle",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "Min Amp (pA)",
        "Starting current of the steps",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "Max Amp (pA)",
        "Ending current of the steps",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "Increments",
        "How many steps to take between min and max",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::UINTEGER,
    },
    {
        "Cycles (#)",
        "How many times to repeat the protocol",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::UINTEGER,
    },
    {
        "Duty Cycle (%)",
        "On time of the step during a single cycle",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "Offset (pA)",
        "DC offset to add",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "Acquire?",
        "Acquire data 0 = no, 1 = yes",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::UINTEGER,
    },
    {
        "Cell (#)",
        "",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::UINTEGER,
    },
    {
        "File Prefix",
        "",
        DefaultGUIModel::PARAMETER //| DefaultGUIModel::DOUBLE,
    },
    {
        "File Info",
        "",
        DefaultGUIModel::PARAMETER //| DefaultGUIModel::DOUBLE,
    },
};

static size_t num_vars = sizeof(vars)/sizeof(DefaultGUIModel::variable_t);

Istep::Istep(void)
    : DefaultGUIModel("Istep",::vars,::num_vars), dt(RT::System::getInstance()->getPeriod()*1e-6), period(1.0), delay(0.0), Amin(-20.0), Amax(20.0), Nsteps(2), Ncycles(1), duty(50), offset(0.0), acquire(0), cellnum(1), prefix("Istep"), info("n/a") {


    newdata.push_back(0);
    newdata.push_back(0);
    newdata.push_back(0);
    newdata.push_back(0);

    update(INIT);
    refresh();
}




Istep::~Istep(void) {}




void Istep::execute(void) {
    V = input(0);

    Iout = offset;
    if (cycle < Ncycles) {

        //Do all time keeping in seconds.
        if (step<Nsteps) {
            if (age >= delay && age < delay+period*(duty/100)-EPS) {
                Iout = Iout + Amin + step*deltaI;
            }
            age += dt/1000;

            if (age >= (period-EPS)) {
                step++;
                age = 0;
            }

            //Do data logging and data writing
            if (acquire) {  
                newdata[0] = tcnt;
                newdata[1] = V;
                newdata[2] = Iout*1e-12;
                data.insertdata(newdata);
                tcnt+=dt/1000;
            } 
        }
        if (step==Nsteps) {
            cycle++;
            step = 0;

        }
    } else if (cycle == Ncycles && acquire) {

        // set acquire to 0 first
        // this ensures we stop acquiring right away
        // before real time is broken by writebuffer()
        acquire = 0;
        data.writebuffer(prefix, info);
        data.resetbuffer();
        
        setParameter("Acquire?", acquire);
    }
    output(0) = Iout*1e-12;
}



void Istep::update(DefaultGUIModel::update_flags_t flag) 
{

     std::ostringstream o;
     unsigned int pos = 0;

    switch(flag) {
        case INIT:
            setParameter("Period (sec)",period);
            setParameter("Delay (sec)",delay);
            setParameter("Min Amp (pA)",Amin);
            setParameter("Max Amp (pA)",Amax);
            setParameter("Increments",Nsteps);
            setParameter("Cycles (#)",Ncycles);
            setParameter("Duty Cycle (%)",duty);
            setParameter("Offset (pA)",offset);

            setParameter("Acquire?",acquire);
            setParameter("Cell (#)",cellnum);
            setParameter("File Prefix", prefix);
            //setParameter("File Info", info);

            // set info string
            o << "[" << period << "/" << delay << "/" << Ncycles 
              << "/" << duty << "/" << offset << " - " << Amin 
              << ":" << Nsteps << ":" << Amax << "]";
            info = o.str() + info;
            setParameter("File Info", info);
            
            break;
        case MODIFY:
            period = getParameter("Period (sec)").toDouble();
            delay  = getParameter("Delay (sec)").toDouble();
            Amin   = getParameter("Min Amp (pA)").toDouble();
            Amax   = getParameter("Max Amp (pA)").toDouble();
            Nsteps = getParameter("Increments").toInt();
            Ncycles = getParameter("Cycles (#)").toInt();
            duty   = getParameter("Duty Cycle (%)").toDouble();
            offset = getParameter("Offset (pA)").toDouble();

            acquire = getParameter("Acquire?").toInt();
            cellnum = getParameter("Cell (#)").toInt();
            prefix = getParameter("File Prefix").data();
            //info = getParameter("File Info").data();
            data.newcell(cellnum);
            data.resetbuffer();
            tcnt = 0;

            // find the end of the info string (2 characters) if any
            pos = info.find("]");
            if (pos != string::npos) {
                // remove last runs parameter string
                info = info.substr(pos + 1);
                o << "[" << period << "/" << delay << "/" << Ncycles 
                    << "/" << duty << "/" << offset << " - " << Amin 
                    << ":" << Nsteps << ":" << Amax << "]";
                info = o.str() + info;
                setParameter("File Info", info);
            } else {
                // just set a new string
                o << "[" << period << "/" << delay << "/" << Ncycles 
                    << "/" << duty << "/" << offset << " - " << Amin 
                    << ":" << Nsteps << ":" << Amax << "]";
                info = o.str() + info;
                setParameter("File Info", info);
            }
            
            break;
        case PAUSE:
            output(0) = 0;
        case PERIOD:
            dt = RT::System::getInstance()->getPeriod()*1e-6;
        default:
            break;
    }

    // Some Error Checking for fun

    if (period <= 0) {
        period=1;
        setParameter("Period (sec)",period);
    }

    if (Amin > Amax) {
        Amax = Amin;
        setParameter("Min Amp (pA)",Amin);
        setParameter("Max Amp (pA)",Amax);
    }

    if (Ncycles < 1) {
        Ncycles = 1;
        setParameter("Cycles (#)", Ncycles);
    }
    
    if (Nsteps < 0) {
        Nsteps = 0;
        setParameter("Increments",Nsteps);
    }
    
    if (duty < 0 || duty > 100) {
        duty = 0;
        setParameter("Duty Cycle (%)",duty);
    }
    
    if (delay <= 0 || delay+duty/100 > period) {
        delay=0;
        setParameter("Delay (sec)",delay);
    }

    //Define deltaI based on params
    if (Nsteps > 1) {
       deltaI = (Amax-Amin)/(Nsteps-1);
    } else {
       deltaI = 0;
    }
    
    //Initialize counters
    age = 0;
    step = 0;
    cycle=0;


}



