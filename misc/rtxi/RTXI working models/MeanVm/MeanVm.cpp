/*
* Measure the ongoing average voltage
*/

#include <MeanVm.h>
#include <math.h>

extern "C" Plugin::Object *createRTXIPlugin(void) {
    return new MeanVm();
}

// inputs, outputs, parameters
static DefaultGUIModel::variable_t vars[] = {
    {
        "Vm",
        "Membrane Voltage (in mV)",
        DefaultGUIModel::INPUT,
    },
     {
        "Mean Vm",
        "Average membrane voltage over the given interval",
        DefaultGUIModel::OUTPUT,
    },
    {
        "Interval (s)",
        "Time over which to average",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "Mean Vm",
        "Mean voltage computed over the window",
        DefaultGUIModel::STATE | DefaultGUIModel::DOUBLE,
    },
    {
        "Std Vm",
        "Standard Deviation of the voltage computed over the window",
        DefaultGUIModel::STATE | DefaultGUIModel::DOUBLE,
    },

};

// some necessary variable
static size_t num_vars = sizeof(vars)/sizeof(DefaultGUIModel::variable_t);

// constructor 
// provides default values for paramters, calls update(INIT)
MeanVm::MeanVm(void)
    : DefaultGUIModel("MeanVm",::vars,::num_vars) {
    
    len = 1000;
    vm = 0;
    meanvm = 0;
    stddev = 0;
    dt = 0;
    
    update(INIT);
    refresh();
}

MeanVm::~MeanVm(void) {}

// execute is run every time step
void MeanVm::execute(void) {

    vm = input(0);
    
    // add new value to end
    voltage_segment.push_back(vm);
    
    // remove elements if vector is full or has been resized
    if (voltage_segment.size() > len) {
        // resize to fit the data length
        voltage_segment.erase(voltage_segment.begin(), voltage_segment.end() - len);
    }
    
    // compute mean
    double sum = 0;
    for (unsigned int i = 0; i < voltage_segment.size(); i++) {
        sum += voltage_segment[i];
    }
    meanvm = sum / voltage_segment.size();
    
    // compute standard deviation
    sum = 0;
    for (unsigned int i = 0; i < voltage_segment.size(); i++) {
        sum += pow((voltage_segment[i] - meanvm), 2);
    }
    stddev = sqrt(sum / voltage_segment.size());
    
    output(0) = meanvm;
}

void MeanVm::update(DefaultGUIModel::update_flags_t flag) {
    if(flag == INIT) {
    
        // fprintf(stdout, "INIT called\n");
        // set dt initially
        dt =  RT::System::getInstance()->getPeriod() * 1e-6;
        
        // user types in interval as seconds, converted to samples here
        setParameter("Interval (s)", len);
                
        // state variable to display running mean in GUI window
        setState("Mean Vm", meanvm);
        setState("Std Vm", stddev);
        
    } else if (flag == MODIFY) {
    
        // fprintf(stdout, "MODIFY called\n");
        
        len = getParameter("Interval (s)").toDouble();
        len = len * (1 / dt) * 1000;   // dt is in (ms)
        if (len < 1) {
            len = 1;
            // fprintf(stdout, "Setting interval to %.2f samples\n", len);
        }

        // fprintf(stdout, "len is %2f samples, dt is %.2f, volt_seg is: %d long\n", len, dt, voltage_segment.size());
        
    } else if (flag == PAUSE) {
    } else if (flag == UNPAUSE) {
    } else if (flag == PERIOD) {
        dt =  RT::System::getInstance()->getPeriod() * 1e-6;
    } else {
    }
}
