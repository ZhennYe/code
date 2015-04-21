/**
* Autapse -- Couple a cell to itself
* I took the PSC and SpikeDetecting from Autapse
* and the DataLogger stuff from Istep
*
* [0:0.1:1] runs a ramp from gmax = 0 to gmax = 1 @ 0.1 per second
* [2:10:2] runs a contant gmax @ 2 for 10 seconds
*/

#include <default_gui_model.h>

#include <math.h>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

#include "../include/PSC.h"
#include "../include/DataLogger.cpp"

// epsilon, the fudge factor used to compare doubles
#define EPS 1e-9
using namespace std;

class Autapse : public DefaultGUIModel
{
    public:
    
        Autapse(void);
        virtual ~Autapse(void);
        
        void execute(void);
    
    protected:
    
        void update(DefaultGUIModel::update_flags_t);
    
    private:
    
        double dt;
        double gstart, gend, grate, gout, V;
        double delay, onset_delay;
        double onset_cnt, const_cnt;
        int isconst;
        
        // synaptic rev. potentials and kinetics
        static const double esyn = 0;
        //static const double psgrise = 0.3;
        //static const double psgfall = 5.6;
        double psgrise;
        double psgfall;
        
        // spike detector
        double spikeState;
        
        // psg
        PSC *psgSelf;
        
        // autapse conductance (units?)
        //double gmaxSelf; 
        
        // DataLogger
        DataLogger data;
        double acquire, maxt, tcnt, cellnum;
        string prefix, info;
        vector<double> newdata;
};


extern "C" Plugin::Object *createRTXIPlugin(void) {
    return new Autapse();
}

static DefaultGUIModel::variable_t vars[] = {
    {
        "VpostSelf",
        "Cells own voltage",
        DefaultGUIModel::INPUT,  //0
    },
    {
        "SpikeState",
        "Spike detector input",
        DefaultGUIModel::INPUT,  //1
    },
    {
        "IsynSelf",
        "Synaptic Output Current in Amps",
        DefaultGUIModel::OUTPUT,  //0
    },
    {
        "G Out",
        "an output to test this thing",
        DefaultGUIModel::OUTPUT,  //1
    },
    {
        "Start G (nS)",
        "",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "End G (nS)",
        "",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "Rate (nS/sec)",
        "",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "Delay (ms)",
        "",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "Onset Delay (s)",
        "",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "PSG Rise (ms)",
        "Rise time of synaptic double exponential function",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "PSG Fall (ms)",
        "Fall time of synaptic double exponential function",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "Acquire?",
        "",
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
    {
        "onset_cnt",
        "",
        DefaultGUIModel::STATE
    },
};

static size_t num_vars = sizeof(vars)/sizeof(DefaultGUIModel::variable_t);

Autapse::Autapse(void)
: DefaultGUIModel("Autapse",::vars,::num_vars), dt(RT::System::getInstance()->getPeriod()*1e-6) {
    
    grate = 1.0;
    gout = gstart = 0.0;
    gend = 2.0;  
    acquire = 0.0; 
    cellnum = 1.0;
    prefix = "autapse";
    info = "n/a";
    delay = 3.0;
    onset_delay = 10;
    onset_cnt = 0;
    const_cnt = 0;
    isconst = 0;
    maxt = 0.0;

    psgrise = 0.3;
    psgfall = 5.6;

    
    // make this smaller to account for doublets that you often get
    spikeState = 0;
    
    psgSelf = new PSC(gstart, esyn, psgrise, psgfall, dt, delay);

    newdata.push_back(0);
    newdata.push_back(0);
    newdata.push_back(0);
    newdata.push_back(0);
    
    update(INIT);
    refresh();
}

Autapse::~Autapse(void) {
    delete(psgSelf);
}

void Autapse::execute(void) {
    
    int myacquire = (int)acquire;
    double iout = 0.0;
    
    // get cell's voltage (convert to mv)
    V = input(0) * 1e3;

    // if we have passed the onset delay
    if (onset_cnt > (onset_delay - EPS)) {

        // update the spike detector's state, and output
        spikeState = input(1);
        psgSelf->setState((int)spikeState);

        // set the PSC to use the updated gmax
        psgSelf->setGMax(gout);

        // The Current returned by the PSC class is in amps
        iout = psgSelf->update(V, dt);

        // set output channels
        output(0) = iout;
        output(1) = gout;

        if (gout <= (gend - EPS) && !isconst) {
            // increment ramp
            gout += grate * dt / 1000;

        } else if (gout > (gend - EPS) && !isconst) {
            // this ramp is done, stop output and set as const
            gout = 0.0;
            isconst = 1; 
        } else if (isconst) {
            // do nothing, keep going
        } else {
            // error
        }

    } else {
        // increment the delay before the protocol starts
        onset_cnt += dt / 1000;
        output(0) = 0.0;
        output(1) = 0.0;
    }

    myacquire = (int)acquire;

    // log data
    if (myacquire && tcnt < maxt) {
        newdata[0] = tcnt;
        newdata[1] = V;

        // fudge this so gout is output as 0 until protocol has started
        if (onset_cnt > onset_delay) {
            newdata[2] = gout;
        } else {
            newdata[2] = 0.0;
        }

        newdata[3] = iout;
        data.insertdata(newdata);
        tcnt += dt / 1000;

    } else if (myacquire && tcnt >= maxt) {
        // make sure to set aquire to 0 before we write data
        // to stop collecting more while realtime is broken
        tcnt = 0;
        acquire = 0;
        setParameter("Acquire?", 0.0);
        refresh();

        // check to make sure we didn't accidentally acquire 0 length
        if (maxt > EPS) {
            data.writebuffer(prefix, info);
            data.resetbuffer();
        }
    }
}

void Autapse::update(DefaultGUIModel::update_flags_t flag) {

    std::ostringstream o;

    if (flag == PAUSE) {
        output(0) = 0.0;
        output(1) = 0.0;

    } else if (flag == INIT) {

        setParameter("Start G (nS)", gstart);
        setParameter("End G (nS)", gend);
        setParameter("Rate (nS/sec)", grate);
        setParameter("PSG Rise (ms)", psgrise);
        setParameter("PSG Fall (ms)", psgfall);
        
        setParameter("Acquire?",acquire);
        setParameter("Cell (#)",cellnum);
        setParameter("File Prefix", prefix);
        setParameter("Delay (ms)", delay);
        setParameter("Onset Delay (s)", onset_delay);

        setState("onset_cnt", onset_cnt);
        
        // set info string 
        o << "[" << onset_delay << " -- " << gstart << ":" << grate 
          << ":" << gend << " -- " << delay << "]";
        info = o.str();
        setParameter("File Info", info);

        // check to see if we are running a constant protocol
        if ((gend - gstart) < 0.001) {
            isconst = 1;
        }

        // set runtime
        if (isconst) {
            maxt = onset_delay + grate;
        } else {
            maxt = onset_delay + ((gend - gstart) / grate);
        }

        psgSelf->setRiseFall(psgrise, psgfall);
        
    } else if (flag == MODIFY) {
        
        grate   = getParameter("Rate (nS/sec)").toDouble();
        gstart = getParameter("Start G (nS)").toDouble();
        gend   = getParameter("End G (nS)").toDouble();
        delay = getParameter("Delay (ms)").toDouble();
        onset_delay = getParameter("Onset Delay (s)").toDouble();
        psgrise = getParameter("PSG Rise (ms)").toDouble();
        psgfall = getParameter("PSG Fall (ms)").toDouble();
        
        acquire = getParameter("Acquire?").toInt();
        cellnum = getParameter("Cell (#)").toInt();
        prefix = getParameter("File Prefix").data();
        
        // set info string 
        o << "[" << onset_delay << " -- " << gstart << ":" << grate 
          << ":" << gend << " -- " << delay << "]";
        info = o.str();
        setParameter("File Info", info);
        
        // set gout to be the starting value
        gout = gstart;

        tcnt = 0;
        onset_cnt = 0;

        // if gend and start are the same, assume constant protocol
        // also if its a negative ramp
        if ((gend - gstart) < 0.001) {
            isconst = 1;
        } else {
            isconst = 0;
        }

        // set runtime
        if (isconst) {
            maxt = onset_delay + grate;
        } else {
            maxt = onset_delay + ((gend - gstart) / grate);
        }

        psgSelf->setGMax(gout);
        psgSelf->setDelay(delay);
        psgSelf->setRiseFall(psgrise, psgfall);
        
        // reset data saving
        data.newcell((int)cellnum);
        data.resetbuffer();
        
    } else if (flag == PERIOD) {
        // dt is in
        dt = RT::System::getInstance()->getPeriod()*1e-6;
    } else if (flag == UNPAUSE) {
    } else {
        cerr << "Error: Flag not matched: " << flag << endl;
    }
}
