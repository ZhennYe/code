/*
 * Copyright (C) 2006 Weill Medical College of Cornell University
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

#include <math.h>
#include <SpikeRateController.h>

extern "C" Plugin::Object *createRTXIPlugin(void) {
    return new SpikeRateController();
}

static DefaultGUIModel::variable_t vars[] = {
    {
        "State",
        "State Variable (0 - 4)",
        DefaultGUIModel::INPUT,
    },
    {
        "Iapp",
        "Current (A)",
        DefaultGUIModel::OUTPUT,
    },
    {
        "ISI",
        "msec",
        DefaultGUIModel::OUTPUT,
    },
    {
        "TargetISI",
        "msec",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "MaxDeltaI",
        "Current (dA/dT)",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },    
    {
        "InitialI",
        "Current (A)",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "IApp",
        "Current(A)",
        DefaultGUIModel::STATE,
    },
};

static size_t num_vars = sizeof(vars)/sizeof(DefaultGUIModel::variable_t);

SpikeRateController::SpikeRateController(void)
    : DefaultGUIModel("SpikeRateController",::vars,::num_vars){
  
  TargetISI=100;
  MaxdeltaI = 1e-5;
  A = 1e-6;
  InitialI=0;

  DeltaI=0;
  ChangedCurrent=0;
  
  LastError=1;
  ThisError=0;
  LastSpikeTime=0;
  ThisISI=0;
  
  count=0;
  
  update(INIT);
  refresh();
}

SpikeRateController::~SpikeRateController(void) {}


void SpikeRateController::execute(void) {

  if(TargetISI<=0.0) return; //This shuts off spike rate conrol
  output(1) = TargetISI/1000.0;

  count++;
  TIME=count * DeltaT;

  //Connect spike detector to input channel 0
  //Spike detector is 1 on upward threhold crossing
  //Spike detetor is 4 if cell is above threshold for a long time, this indicates spike rate controller should be reset
  state=(int)(input(0));

   if(state==4)
    {
      DeltaI=0;
      output(0)= 0;
      DefaultGUIModel::pause(true);
      return;
    }

   if(TIME>LastSpikeTime + 6*TargetISI)
   {

       if (DeltaI < 0.0) {
           DeltaI = 0.0;
       }
       DeltaI+= MaxdeltaI/1000;

       // hardcode max rate of change of current @ 5pA / sec
       // (note 1e-9 below)
       if(DeltaI > (5e-3 * (DeltaT / 1000))) {
           DeltaI = 5e-3 * (DeltaT / 1000);
       }

       output(0) += DeltaI*1e-9;
       ChangedCurrent = 1;
       if(state==1) LastSpikeTime = TIME;
       return;
   }

   if(state==1) //if a spike onset has been detected 
   {  
       ThisSpikeTime = TIME;

       if(ChangedCurrent==0 && LastSpikeTime> 0)
       {
           ThisISI= ThisSpikeTime - LastSpikeTime;
           LastSpikeTime= ThisSpikeTime; 
           ThisError = (ThisISI - TargetISI);

           if((ThisError * LastError)<0) 
           {
               A = A*fabs(LastError)/(fabs(ThisError)+fabs(LastError));
           }
           else if(fabs(ThisError)-fabs(LastError) < 0)
           {
               A*=2.0;
           }

           if(A>10) A= 10;
           if(A< 5e-8) A= 5e-8;
           DeltaI = A*ThisError;
           if(DeltaI>MaxdeltaI) DeltaI=MaxdeltaI;
           else if(DeltaI<-MaxdeltaI) DeltaI= -MaxdeltaI;

           LastError= ThisError;

           output(0)+= DeltaI*1e-9;
	   IApp = output(0)*1e12;
	   //setState("IApp", IApp);
           return;	
        }
      else
	{
	  LastSpikeTime = ThisSpikeTime;
	  ChangedCurrent = 0;
	  output(0)+= 0;
	  return;
	}
    }
  else
    {
      output(0)+=0;  
      return;
    }
return;
}

void SpikeRateController::update(DefaultGUIModel::update_flags_t flag) {
  switch(flag) {
  case INIT:
    
    setParameter("TargetISI",TargetISI);
    setParameter("MaxDeltaI",MaxdeltaI);
    setParameter("InitialI",InitialI);
    setState("IApp",IApp);
    DeltaT = RT::System::getInstance()->getPeriod() * 1e-6;
    break;
    
  case MODIFY:

    TargetISI = getParameter("TargetISI").toDouble();
    MaxdeltaI = getParameter("MaxDeltaI").toDouble();
    InitialI = getParameter("InitialI").toDouble();
            
    DeltaI=0;
    ChangedCurrent=0;
    
    LastError=0;
    ThisError=0;
    LastSpikeTime=0;
    A = 1e-6;
    //InitialI = 0;
    //output(0) = InitialI;
    //IApp = 0;
    setState("IApp",IApp);
    //DeltaT = RT::System::getInstance()->getPeriod() * 1e-6;
    break;
  case PAUSE:
     //InitialI = output(0);
     InitialI = 0;
     setParameter("InitialI",InitialI);
     output(0) = 0;
    break;
  case UNPAUSE:
    output(0) = InitialI;
    break;

  case PERIOD:
    DeltaT =  RT::System::getInstance()->getPeriod() * 1e-6;
    break;

  default:
    break;
    }
}
