/*
 * KV -- from Fernando's imagination
 * May 2007
 */

#include <math.h>
#include <default_gui_model.h>

class KV : public DefaultGUIModel
{

    public:

        KV(void);
        virtual ~KV(void);

        virtual void execute(void);

    protected:

        virtual void update(DefaultGUIModel::update_flags_t);

    private:

        // parameters
        double EK;
        double GK;
        double peaktau;
        double midtau;
        double mintau;
        double Vnhalf;
        double ninfk;
        double taun;

        // input / output
        double Vm;  
        double I;   

        // states
        double s;
        double s_inf;

        // rtxi things, i think
        double period;
        double rate;
        int steps;
};


extern "C" Plugin::Object *createRTXIPlugin(void) {
    return new KV();
}

static DefaultGUIModel::variable_t vars[] = {
    {
        "Vm",
        "mV",
        DefaultGUIModel::INPUT,
    },
    {
        "I",
        "pA",
        DefaultGUIModel::OUTPUT,
    },
    {
        "EK",
        "mV - reversal potential",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "GK",
        "mS/cm^2 - maximal conductance",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "peaktau",
        "peak time constant",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "midtau",
        "mid point",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "mintau",
        "minimum tau - baseline tau added to others" ,
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "Vnhalf",
        "midpoint n - half activation voltage (mV)",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "ninfk",
        "midpoint slope n - slope at midpoint (negative for Ih)",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "s",
        "The S Variable",
        DefaultGUIModel::STATE,
    },
    {
        "s_inf",
        "s infinity",
        DefaultGUIModel::STATE,
    },
    {
        "taun",
        "time constant",
        DefaultGUIModel::STATE,
    },
};

static size_t num_vars = sizeof(vars)/sizeof(DefaultGUIModel::variable_t);

KV::KV(void): DefaultGUIModel("KV",::vars,::num_vars) {

    // rtxi timing stuff
    period = RT::System::getInstance()->getPeriod()*1e-6;
    steps = static_cast<int>(ceil(period/25e-3));

    // parameters
    EK = -90;
    GK = 10;
    peaktau = 5;
    midtau = -50;
    mintau= 0.1;
    Vnhalf = -25;
    ninfk = 3;	
    // states
    s = 0.0;
    s_inf = 0.0;

    update(INIT);
    refresh();
}

KV::~KV(void) {}


void KV::execute(void) {

    double s_inf, IKV_s;

    // convert input to mV, correct for junction
    Vm = input(0)*1e3 - 10;

    // the main equation
    taun = peaktau / (exp((Vm-midtau) / 2) + exp((Vm + 6) / -56)) + mintau;

    // compute conductance
    s_inf = 1 / (1 + exp((Vm - Vnhalf) / -ninfk));
    s  = s_inf + (s - s_inf) * exp(-period / taun);

    // nS * mV = pA (are these units right?)
    IKV_s = s * GK * (EK - Vm);    

    // send output converted to Amps
    output(0) = IKV_s / 1e12;  
}

void KV::update(DefaultGUIModel::update_flags_t flag) {

    if (flag == INIT) {

        setParameter("EK",EK);
        setParameter("GK",GK);
        setParameter("Vnhalf",Vnhalf);
        setParameter("ninfk",ninfk);
        setParameter("peaktau",peaktau);
        setParameter("midtau",midtau);
        setParameter("mintau",mintau);

        setState("s", s);
        setState("s_inf", s_inf);
        setState("taun", taun);

        s = 0.0;
        s_inf = 0.0;
        taun = 0.1;

    } else if (flag == MODIFY) {

        EK = getParameter("EK").toDouble();
        GK = getParameter("GK").toDouble();
        Vnhalf = getParameter("Vnhalf").toDouble();
        ninfk = getParameter("ninfk").toDouble();
        peaktau = getParameter("peaktau").toDouble();
        midtau = getParameter("midtau").toDouble();
        mintau = getParameter("mintau").toDouble();

        s = 0.0;
        s_inf = 0.0;
        taun = 0.1;

    } else if (flag == PERIOD) {

        period = RT::System::getInstance()->getPeriod()*1e-6;
        steps = static_cast<int>(ceil(period*getParameter("rate").toUInt()/1000.0));

    } else  if (flag == PAUSE) {
        s = 0.0;
        s_inf = 0.0;
        output(0) = 0.0;
    } else {
        // nothing
    }
}

