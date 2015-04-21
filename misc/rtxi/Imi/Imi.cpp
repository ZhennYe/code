/*
 * I_mi (proctolin current model)
 * Taken from Goldman et al., 2001
 * December 2013
 * Implemented by Tilman Kispersky
 */

#include <math.h>
#include <default_gui_model.h>

class Imi : public DefaultGUIModel
{

    public:

        Imi(void);
        virtual ~Imi(void);

        virtual void execute(void);

    protected:

        virtual void update(DefaultGUIModel::update_flags_t);

    private:

        // parameters
        double Erev;   // reversal potential
        double Km;     // logistic function slope
        double Vhalf;  // half maximal activation
        double Gmax;   // maximal conductance
        double taum;   // time constant

        // input / output
        double Vm;  
        double I;   

        // states
        double m;

        // rtxi things, i think
        double period;
        double rate;
        int steps;

        // junction potential
        double jpot;
};


extern "C" Plugin::Object *createRTXIPlugin(void) {
    return new Imi();
}

static DefaultGUIModel::variable_t vars[] = {
    {
        "Vm",
        "Cell voltage in mV",
        DefaultGUIModel::INPUT,
    },
    {
        "I",
        "Proctolin current in pA",
        DefaultGUIModel::OUTPUT,
    },
    {
        "Erev",
        "mV - reversal potential",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "Km",
        "logistic function slope",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "Vhalf",
        "Half maximal activation",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "Gmax",
        "mS/cm^2 - maximal conductance",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "taum",
        "time constant",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "m",
        "activation variable",
        DefaultGUIModel::STATE, 
    },
};

static size_t num_vars = sizeof(vars)/sizeof(DefaultGUIModel::variable_t);

Imi::Imi(void): DefaultGUIModel("Imi",::vars,::num_vars) {

    // rtxi timing stuff
    period = RT::System::getInstance()->getPeriod()*1e-6;
    steps = static_cast<int>(ceil(period/25e-3));

    // junction potential
    jpot = 0.0;

    // parameters
    Erev = -10;
    Gmax = 1;
    Km = -5;
    Vhalf = -57;
    taum = 6;

    // states
    m = 0;
    I = 0;

    update(INIT);
    refresh();
}

Imi::~Imi(void) {}


void Imi::execute(void) {

    double minf, dm;

    // convert input to mV, correct for junction
    Vm = input(0) * 1e3 - jpot;

    // activation variable
    minf = 1 / (1 + exp((Vm - Vhalf) / Km));
    dm = (minf - m) / taum;
    m = m + period * dm;

    // nS * mV = pA 
    I = Gmax * m * (Erev - Vm);    

    // send output converted to Amps
    output(0) = I / 1e12;  
}

void Imi::update(DefaultGUIModel::update_flags_t flag) {

    if (flag == INIT) {

        setParameter("Erev", Erev);
        setParameter("Gmax", Gmax);
        setParameter("Vhalf", Vhalf);
        setParameter("Km", Km);
        setParameter("taum", taum);

        // states
        setState("m", m);
        m = 0.0;

    } else if (flag == MODIFY) {

        Erev = getParameter("Erev").toDouble();
        Gmax = getParameter("Gmax").toDouble();
        Vhalf = getParameter("Vhalf").toDouble();
        Km = getParameter("Km").toDouble();
        taum = getParameter("taum").toDouble();

        m = 0.0;
        I = 0.0;

    } else if (flag == PERIOD) {

        period = RT::System::getInstance()->getPeriod()*1e-6;
        steps = static_cast<int>(ceil(period*getParameter("rate").toUInt()/1000.0));

    } else  if (flag == PAUSE) {

        m = 0.0;
        I = 0.0;
        output(0) = 0.0;        

    } else {
        // nothing
    }
}

