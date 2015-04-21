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

#include <SumPSG.h>
#include <math.h>
#include <iostream>
#include <sstream>

extern "C" Plugin::Object *createRTXIPlugin(void) {
    return new SumPSG();
}

#define PERIOD RT::System::getInstance()->getPeriod()*1e-6
#define PSG_LEN (int)(50.0 / (RT::System::getInstance()->getPeriod()*1.0e-6))

static DefaultGUIModel::variable_t vars[] = {
    {
        "PreSpikeState",
        "Output of spike detect model",
        DefaultGUIModel::INPUT,
    },
    {
      "Vpost",
      "",
      DefaultGUIModel::INPUT,
    },
    {
        "Iout",
        "",
        DefaultGUIModel::OUTPUT,
    },
    {
        "Acquire?",
        "0 or 1",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "Record Len (sec)",
        "Length of Data Record, in seconds, to record",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "Cell (#)",
        "",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "File Prefix",
        "",
        DefaultGUIModel::PARAMETER,
    },
    {
        "File Info",
        "",
        DefaultGUIModel::PARAMETER
    },
    {
        "Percent Done",
        "",
        DefaultGUIModel::STATE | DefaultGUIModel::DOUBLE,
    },
    {
        "Use Conductance?",
        "Whether or not to use conductance or current synapses. O = current, 1 = conductance",
        DefaultGUIModel::PARAMETER
    },
    {
        "GmaxAMPA",
        "Maximum Synaptic Conductance (nS)",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "GmaxNMDA",
        "Maximum Synaptic Conductance (nS)",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "GmaxGABA",
        "Maximum Synaptic Conductance (nS)",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "rate",
        "Mean PSG frequency (Hz)",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "rate GABA",
        "Mean PSG frequency (Hz)",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "Esyn",
        "Synaptic Reversal Potential (mV)",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "Esyn GABA",
        "Synaptic Reversal Potential (mV)",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    /*
    {
        "PSGRiseAMPA",
        "PSG Rise Time (ms)",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "PSGFallAMPA",
        "PSG Fall Time (ms)",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "PSGRiseNMDA",
        "PSG Rise Time (ms)",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "PSGFallNMDA",
        "PSG Fall Time (ms)",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "PSGRiseGABA",
        "PSG Rise Time (ms)",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "PSGFallGABA",
        "PSG Fall Time (ms)",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "NMDA Slope",
        "Slope of activation",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "NMDA Midpoint",
        "Midpoint of activation",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    */
    {
        "GABA Modulation Amp",
        "Modulation percentage: 0.1 = 10% modulation",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "GABA Modulation Freq",
        "Frequency at which GABA is modulated (Hz)",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "AMPA NMDA Modulation Amp",
        "Modulation percentage: 0.1 = 10% modulation",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "AMPA NMDA Modulation Freq",
        "Frequency at which AMPA NMDA is modulated (Hz)",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "Shift",
        "Phase shift of the two modulations as a fraction of pi",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
};

static size_t num_vars = sizeof(vars)/sizeof(DefaultGUIModel::variable_t);

SumPSG::SumPSG(void)
  : DefaultGUIModel("PSG",::vars,::num_vars) {

      useNMDA = 0;
      useConductance = 0;
      
      gmaxAMPA = 0.1;
      gmaxNMDA = 0.1,
      gmaxGABA = 0.1;
      
      esyn = 0.0;
      esyn_gaba = -75.0;
      
      PSGriseAMPA = 1;
      PSGfallAMPA = 8; 
      PSGriseNMDA = 1.68;
      PSGfallNMDA = 6.21;
      PSGriseGABA = 1.68;
      PSGfallGABA = 6.21;

      nmda_slope = 10; 
      nmda_midpoint = -10;

      rate = 1;
      rate_gaba = 1;

      shift = 0;
      
      acquire = 0.0;
      maxt = 5;
      prefix = "SumPSG";
      info = "no comment";
      cellnum = 1;

      tcnt = 0.0;
      stepcnt = 0.0;

      modamp = 0.0;
      modfreq = 0.0;
      excite_modfreq = 0.0;
      excite_modamp = 0.0;

      exp1 = 0.0;
      exp2 = 0.0;
      exp3 = 0.0;
      exp4 = 0.0;
      exp5 = 0.0;
      exp6 = 0.0;

      pdone = 0;
      setState("Percent Done", pdone);

      // clear the temp vector
      newdata.clear();

      update(INIT);
      refresh();
}

SumPSG::~SumPSG(void) {}

static inline void SetPSG(double adEPSG[], double dEPSGRise, double dEPSGFall, double dAqRate) 
{ 
	int i;
	double dMax = -10000000.0;
	// AqRate is in ms. 
	// dEPSGRise & dEPSGFall should be in ms. 
	
	for(i=0; i<PSG_LEN; i++)
	{ 
		adEPSG[i] = exp(-((i+1)*dAqRate)/(dEPSGFall)) -	
			exp(-((i+1)*dAqRate)/(dEPSGRise)); 
		if(adEPSG[i] > dMax) dMax = adEPSG[i];
	} 
	
	for(i=0; i<PSG_LEN; i++) 
	{ 
		adEPSG[i] /= dMax; 
	} 


} 

int SumPSG::isSpiking(double r) {

	// random number between 0 and 1
	double temp = (double)rand() / (double)RAND_MAX;

	if (temp < r*PERIOD/1000) {
		return 1;
	} else {
		return 0;
	}
	
}

void SumPSG::execute(void) {
    
    #define Vpost input(1)*1e3
    
    double dt = PERIOD;
    double modrate = 0.0;
    double excite_modrate = 0.0;

    double fact1 = exp (-dt / PSGfallAMPA);
    double fact2 = exp (-dt / PSGriseAMPA);
    double fact5 = exp (-dt / PSGfallGABA);
    double fact6 = exp (-dt / PSGriseGABA);
    
    double peakval = exp ((1/(1/PSGfallAMPA-1/PSGriseAMPA)*log(PSGfallAMPA/PSGriseAMPA))/PSGriseAMPA) - exp ((1/(1/PSGfallAMPA-1/PSGriseAMPA)*log(PSGfallAMPA/PSGriseAMPA))/PSGfallAMPA);
    double peakval_gaba = exp ((1/(1/PSGfallGABA-1/PSGriseGABA)*log(PSGfallGABA/PSGriseGABA))/PSGriseGABA) - exp ((1/(1/PSGfallGABA-1/PSGriseGABA)*log(PSGfallGABA/PSGriseGABA))/PSGfallGABA);

    double fact3 = 0.0, fact4 = 0.0, peakval_nmda = 0.0;

    if (useNMDA) {
        fact3 = exp (-dt / PSGfallNMDA);
        fact4 = exp (-dt / PSGriseNMDA);
        peakval_nmda = exp((1/(1/PSGfallNMDA-1/PSGriseNMDA)*log(PSGfallNMDA/PSGriseNMDA))/PSGriseNMDA) - exp ((1/(1/PSGfallNMDA-1/PSGriseNMDA)*log(PSGfallNMDA/PSGriseNMDA))/PSGfallNMDA);
    }
	
    //	printf("dt: %f -- fact1: %f    fact2: %f\n", dt, fact1, fact2);

    // modulate the rate of "spiking" if necessary
    // modulate only the rate of gaba
    if (modamp > 0) {
        //modrate = rate_gaba * (1 + modamp * cos(stepcnt * modfreq * 2 * M_PI + shift));
        modrate = rate_gaba * (1 + modamp * cos(stepcnt * modfreq * 2 * M_PI + shift));
    } else {
        modrate = rate_gaba;
    }

    if (excite_modamp > 0) {
        excite_modrate = rate * (1 + excite_modamp * cos(stepcnt * excite_modfreq * 2 * M_PI));
    } else {
        excite_modrate = rate;
    }

    if (isSpiking(excite_modrate)) {
        exp1 += 1/peakval;
        exp2 += 1/peakval;

        if (useNMDA) {
            exp3 += 1/peakval_nmda;
            exp4 += 1/peakval_nmda;
        }
    }

    // use the modulated spikerate
    if (isSpiking(modrate)) {
        exp5 += 1/peakval_gaba;
        exp6 += 1/peakval_gaba;
    }

    exp1 = exp1 * fact1;
    exp2 = exp2 * fact2;
    exp5 = exp5 * fact5;
    exp6 = exp6 * fact6;
    
    if (useNMDA) {
        exp3 = exp3 * fact3;
        exp4 = exp4 * fact4;
    }
    // debug
    //if ((int)stepcnt % 100000 == 0)
    //    fprintf(stderr, "0. e1: %.2f, e2: %.2f, f1: %.2f, f2: %.2f\n", exp1, exp2, fact1, fact2);

    double sum = exp1 - exp2;
    double sum_gaba = exp5 - exp6;

    double igaba = 0.0;
    double iampa = 0.0;
    double inmda = 0.0;
    double sum_nmda = 0.0;
    double gv_nmda = 0.0;

    // decide whether to use driving force or not
    if (useConductance) {
        igaba = sum_gaba * gmaxGABA * (Vpost - esyn_gaba);
        iampa = sum * gmaxAMPA * (Vpost - esyn);
    } else {
        igaba = sum_gaba * gmaxGABA;
        iampa = -sum * gmaxAMPA;
    }

    double total_curr = 0.0;

    // compute NMDA if needed 
    if (useNMDA) {
        sum_nmda = exp3 - exp4;
        gv_nmda = 1 / (1 + exp((Vpost - nmda_midpoint) / nmda_slope));

        if (useConductance) {
            inmda = sum_nmda * gv_nmda * gmaxNMDA * (Vpost - esyn);
        } else {
            inmda = -sum_nmda * gv_nmda * gmaxNMDA;
        }
        
        total_curr = (igaba + inmda + iampa) * 1e-12;
        output(0) = (igaba + inmda + iampa) * 1e-12;
    } else {
        total_curr = (igaba + iampa) * 1e-12;
        output(0) = (igaba + iampa) * 1e-12;
    }

    // count time for the modulation calculation
    if (modamp > 0 || excite_modamp > 0) {
        stepcnt += RT::System::getInstance()->getPeriod()*1e-6 / 1000.0;
    }

    // Log some data and then we're done
    int myacq = (int)acquire;

    if (myacq && tcnt < maxt) {
        //newdata.clear();

        // save the time, voltage in volts
        //newdata.push_back(tcnt);
        newdata.push_back(total_curr);
        newdata.push_back(Vpost / 1000);
        
        // only add this part if you want to save the current as well
        // newdata.push_back(sum);
        //newdata.push_back(sum_gaba);
        // if (useNMDA) {
        //     newdata.push_back(sum_nmda * gv_nmda);
        // }

        data.insertdata(newdata);
        newdata.clear();

        tcnt += RT::System::getInstance()->getPeriod()*1e-6 / 1000.0;
        pdone = (tcnt / maxt * 100);

    } else if (myacq && tcnt > maxt) {
        // set counters to 0 before the RT-breaking writebuffer
        tcnt = 0;
        acquire = 0;

        data.writebuffer(prefix, info);
        data.resetbuffer();

        setParameter("Acquire?", 0.0);
        refresh();
    }
}


void SumPSG::update(DefaultGUIModel::update_flags_t flag) {

    std::ostringstream o;
    unsigned int pos = 0;
    
    switch(flag) {
        case INIT:
            setParameter("Use Conductance?", useConductance);

            // set NMDA flag based on gmaxNMDA
            (((int)gmaxNMDA == 0) ? useNMDA = 0 : useNMDA = 1);
            
            setParameter("GmaxAMPA",gmaxAMPA);
            //setParameter("PSGRiseAMPA",PSGriseAMPA);
            //setParameter("PSGFallAMPA",PSGfallAMPA);

            setParameter("GmaxNMDA",gmaxNMDA);
            //setParameter("PSGRiseNMDA",PSGriseNMDA);
            //setParameter("PSGFallNMDA",PSGfallNMDA);

            setParameter("GmaxGABA",gmaxGABA);
            //setParameter("PSGRiseGABA",PSGriseGABA);
            //setParameter("PSGFallGABA",PSGfallGABA);

            setParameter("Esyn",esyn);
            setParameter("Esyn GABA",esyn_gaba);
            setParameter("rate",rate);
            setParameter("rate GABA",rate_gaba);

            //setParameter("NMDA Slope", nmda_slope);
            //setParameter("NMDA Midpoint", nmda_midpoint);

            setParameter("Record Len (sec)",maxt);
            setParameter("Acquire?",acquire);
            setParameter("Cell (#)",cellnum);
            setParameter("File Prefix", prefix);
            //setParameter("File Info", info); 

            setParameter("GABA Modulation Freq", modfreq);
            setParameter("GABA Modulation Amp", modamp);

	        setParameter("AMPA NMDA Modulation Freq", excite_modfreq);
            setParameter("AMPA NMDA Modulation Amp", excite_modamp);

            setParameter("Shift", shift);

            // set info string automatically to be useful
            o << "[" << useConductance << " - "
                << gmaxAMPA << "," << PSGriseAMPA << "," << PSGfallAMPA << " - "
                << gmaxGABA << "," << PSGriseGABA << "," << PSGfallGABA << " - "
                << gmaxNMDA << "," << PSGriseNMDA << "," << PSGfallNMDA << " - "
                << esyn << "," << esyn_gaba << "," << rate << "," << rate_gaba << " - "
                << nmda_slope << "," << nmda_midpoint << " - "
                << modfreq << "," << modamp <<  "," << shift << "]";
            info = o.str() + info;
            setParameter("File Info", info);

            output(0) = 0.0;
            newdata.clear();
            data.resetbuffer();
            break;

        case MODIFY:
            useConductance = getParameter("Use Conductance?").toUInt();
            
            gmaxAMPA   = getParameter("GmaxAMPA").toDouble();
            //PSGriseAMPA = getParameter("PSGRiseAMPA").toDouble();
            //PSGfallAMPA = getParameter("PSGFallAMPA").toDouble();

            // set NMDA flag based on gmaxNMDA
            gmaxNMDA   = getParameter("GmaxNMDA").toDouble();
            ((gmaxNMDA > 0) ? useNMDA = 1 : useNMDA = 0);
            //PSGriseNMDA = getParameter("PSGRiseNMDA").toDouble();
            //PSGfallNMDA = getParameter("PSGFallNMDA").toDouble();

            gmaxGABA  = getParameter("GmaxGABA").toDouble();
            //PSGriseGABA = getParameter("PSGRiseGABA").toDouble();
            //PSGfallGABA = getParameter("PSGFallGABA").toDouble();

            rate = getParameter("rate").toDouble();
            rate_gaba = getParameter("rate GABA").toDouble();
            esyn = getParameter("Esyn").toDouble();
            esyn_gaba = getParameter("Esyn GABA").toDouble();

            //nmda_slope = getParameter("NMDA Slope").toDouble();
            //nmda_midpoint = getParameter("NMDA Midpoint").toDouble();

            // data acquisition
            maxt = getParameter ("Record Len (sec)").toDouble ();
            acquire = getParameter ("Acquire?").toUInt();
            cellnum = getParameter ("Cell (#)").toInt();
            prefix = getParameter ("File Prefix").data();
            info = getParameter ("File Info").data();

            modfreq = getParameter("GABA Modulation Freq").toDouble();
            modamp = getParameter("GABA Modulation Amp").toDouble();

	        excite_modfreq = getParameter("AMPA NMDA Modulation Freq").toDouble();
            excite_modamp = getParameter("AMPA NMDA Modulation Amp").toDouble();

            shift = getParameter("Shift").toDouble();

            // find the end of the info string (2 characters) if any
            pos = info.find("]");
            if (pos != string::npos) {
                // remove last runs parameter string
                info = info.substr(pos + 1);
                o << "[" << useConductance << " - "
                    << gmaxAMPA << "," << PSGriseAMPA << "," << PSGfallAMPA << " - "
                    << gmaxGABA << "," << PSGriseGABA << "," << PSGfallGABA << " - "
                    << gmaxNMDA << "," << PSGriseNMDA << "," << PSGfallNMDA << " - "
                    << esyn << "," << esyn_gaba << "," << rate << "," << rate_gaba << " - "
                    << nmda_slope << "," << nmda_midpoint << " - "
                    << modfreq << "," << modamp << "," << shift << "]";
                info = o.str() + info;
                setParameter("File Info", info);
            } else {
                // just set a new string
                o << "[" << useConductance << " - "
                    << gmaxAMPA << "," << PSGriseAMPA << "," << PSGfallAMPA << " - "
                    << gmaxGABA << "," << PSGriseGABA << "," << PSGfallGABA << " - "
                    << gmaxNMDA << "," << PSGriseNMDA << "," << PSGfallNMDA << " - "
                    << esyn << "," << esyn_gaba << "," << rate << "," << rate_gaba << " - "
                    << nmda_slope << "," << nmda_midpoint << " - "
                    << modfreq << "," << modamp << "," << shift << "]";
                info = o.str() + info;
                setParameter("File Info", info);
            }

            // new cell number for logging purposes
            data.newcell (cellnum);

            // reset time counter
            tcnt = 0.0;
            data.resetbuffer();

            break;
        case PAUSE:
            output(0) = 0.0;
            break;
        default:
            break;
    }
}
