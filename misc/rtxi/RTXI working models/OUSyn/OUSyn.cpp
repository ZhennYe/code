/**
 * 
 */

#include "OUSyn.h"
#include <iostream>
#include <cmath>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <math.h>
     
#define EPS 1e-9

extern "C" Plugin::Object *createRTXIPlugin(void) {
    return new OUSyn();
}

static DefaultGUIModel::variable_t vars[] = {
    {
        "Vin",
        "Cell's Voltage",
        DefaultGUIModel::INPUT,
    },
    {
        "I Out",
        "Excitatory Conductance",
        DefaultGUIModel::OUTPUT,
    },
    {
        "Ge",
        "Excitatory Conductance",
        DefaultGUIModel::STATE,
    },
    {
        "Gi",
        "Inhibitory Conductance",
        DefaultGUIModel::STATE,
    },
    /*
    {
        "Record Length (s)",
        "How many seconds to record for",
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
        DefaultGUIModel::PARAMETER,
    },
    {
        "File Info",
        "",
        DefaultGUIModel::PARAMETER,
    },
    */
    {
        "Mean E",
        "Mean Value of the Excitatory OU Process",
        DefaultGUIModel::PARAMETER,
    },
    {
        "Mean I",
        "Mean Value of the Inhibitory OU Process",
        DefaultGUIModel::PARAMETER,
    },
    {
        "Tau E",
        "Time constant of Excitatory OU Process",
        DefaultGUIModel::PARAMETER,
    },
    {
        "Tau I",
        "Time constant of Inhibitory OU Process",
        DefaultGUIModel::PARAMETER,
    },
    {
        "De",
        "Diffusion Constant for Excitation",
        DefaultGUIModel::PARAMETER,
    },
    {
        "Di",
        "Diffusion Constant for Inhibition",
        DefaultGUIModel::PARAMETER,
    },
    {
        "Corr",
        "Degree of Correlation between Exciation and Inhibition (0 - 1)",
        DefaultGUIModel::PARAMETER,
    },
    {
        "Erev",
        "Excitation Reversal Potential (mV)",
        DefaultGUIModel::PARAMETER,
    },
    {
        "Irev",
        "Inhibition Reversal Potential (mV)",
        DefaultGUIModel::PARAMETER,
    },
};

static size_t num_vars = sizeof(vars)/sizeof(DefaultGUIModel::variable_t);

OUSyn::OUSyn(void)
    : DefaultGUIModel("OUSyn",::vars,::num_vars) { 
    
    dt = (RT::System::getInstance()->getPeriod()*1e-6);
/*
    len = 1.0;
    acquire = 0;
    cellnum = 1;
    prefix = "OUSyn";
    info = "n/a";
*/
    taue = 0;
    taui = 0;
    De = 0;
    Di = 0;
    meane = 0;
    meani = 0; 
    correlation = 0;
    erev = 0;
    irev = -80;

    Ae = 0;
    Ai = 0;

    Ge = 0;
    Gi = 0;

    // random number generators
    //rng = gsl_rng_alloc(gsl_rng_gfsr4);
    rng = gsl_rng_alloc(gsl_rng_default);

    srand(time(NULL));
    gsl_rng_set(rng, rand());

    setState("Ge", Ge);
    setState("Gi", Gi);

    update(INIT);
    refresh();
}




OUSyn::~OUSyn(void) {
    if(rng) {
        gsl_rng_free(rng);
    }
}

void OUSyn::execute(void) {

    // do nothing more than save a trace
    V = input(0) * 1e3;

    // get randn
    double randn1 = 0;
    double randn2 = 0;

    // negatively correlated
    if (correlation < 0 && ((rand()/(double)RAND_MAX) < abs(correlation))) {
        randn1 = gsl_ran_gaussian(rng, 1);
        randn2 = -randn1;

    // positively correlated
    } else if (correlation > 0 && ((rand()/(double)RAND_MAX) < correlation)) {
        randn1 = gsl_ran_gaussian(rng, 1);
        randn2 = randn1;

    // not correlated
    } else {
        randn1 = gsl_ran_gaussian(rng, 1);
        randn2 = gsl_ran_gaussian(rng, 1);
    }
    
    // compute OU Processes
    Ge = (meane + (Ge - meane) * exp(-dt / taue) + Ae * randn1);
    Gi = (meani + (Gi - meani) * exp(-dt / taui) + Ai * randn2);

    //double Ie = -Ge * (V - erev);
    //double Ii = -Gi * (V - irev);
    double Ie = -Ge; 
    double Ii = -Gi;

    output(0) = (Ie + Ii) * 1e-12;

	/*
    // Data Logging
    if (tcnt < (len - EPS)) {

        // count forward make sure we are using seconds
        tcnt += dt / 1000;

        // log data if needed
        if (acquire) {
            newdata.push_back(tcnt);
            newdata.push_back(V * 1e-3);
            newdata.push_back((Ie + Ii)*1e-12);
            newdata.push_back(Ge);
            newdata.push_back(Gi);
            data.insertdata(newdata);
            newdata.clear();
        }
    } else if (acquire) {

        // set acq to 0 first so we're sure we are stopping 
        // to collect data before writebuffer() breaks realtime
        acquire = 0;
        data.writebuffer(prefix, info);
        data.resetbuffer();
        
        setParameter("Acquire?", acquire);
    }*/
}



void OUSyn::update(DefaultGUIModel::update_flags_t flag) 
{

    if (flag == INIT) {
        // recording params
       /* setParameter("Record Length (s)", len);
        setParameter("Acquire?",acquire);
        setParameter("Cell (#)",cellnum);
        setParameter("File Prefix", prefix);
        setParameter("File Info", info);*/

        // ou paramters
        setParameter("Mean E", meane);
        setParameter("Mean I", meani);
        setParameter("Tau I", taue);
        setParameter("Tau E", taui);
        setParameter("De", De);
        setParameter("Di", Di);
        setParameter("Corr", correlation);
        setParameter("Erev", erev);
        setParameter("Irev", irev);

        // compute Ae and Ai
        updateA();

    } else if(flag == MODIFY) {               
       // len = getParameter("Record Length (s)").toDouble();

       // acquire = getParameter("Acquire?").toInt();
       // cellnum = getParameter("Cell (#)").toInt();
       // prefix = getParameter("File Prefix").data();
       // info = getParameter("File Info").data();

        //data.newcell(cellnum);
       // data.resetbuffer();
        //tcnt = 0;

        // ou params
        meane = getParameter("Mean E").toDouble();
        meani = getParameter("Mean I").toDouble();
        taue = getParameter("Tau E").toDouble();
        taui = getParameter("Tau I").toDouble();
        De = getParameter("De").toDouble();
        Di = getParameter("Di").toDouble();
        correlation = getParameter("Corr").toDouble();
        erev = getParameter("Erev").toDouble();
        irev = getParameter("Irev").toDouble();

        // compute Ae and Ai
        updateA();

    } else if(flag == PAUSE) {
        output(0) = 0;

    } else if(flag == PERIOD) {
        dt = RT::System::getInstance()->getPeriod()*1e-6;
        updateA();
    }

    // Some Error Checking for fun
    if (len < 0.0) {
        setParameter("Record Length (s)", 0.0);
        refresh();
    }

    if (correlation < -1.0 || correlation > 1.0) {
        correlation = 0;
        setParameter("Corr", correlation);
        refresh();       
    }
}

void OUSyn::updateA() {
    // compute Ae and Ai
    Ae = sqrt((De * taue / 2) * (1 - exp(-2 * dt / taue)));
    Ai = sqrt((Di * taui / 2) * (1 - exp(-2 * dt / taui)));
}



