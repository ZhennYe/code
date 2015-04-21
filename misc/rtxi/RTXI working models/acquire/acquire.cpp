#include <acquire.h>
#include <cmath>
#include "/home/rtxi/plugins/data_recorder/data_recorder.h"

extern "C" Plugin::Object *createRTXIPlugin(void) {
    return new Acquire();
}

static DefaultGUIModel::variable_t vars[] = {
    {
        "Duration",
        "ms",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "Trial",
        "",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::INTEGER,
    },
    {
        "Samples",
        "",
        DefaultGUIModel::STATE | DefaultGUIModel::DOUBLE,
    },
};

static size_t num_vars = sizeof(vars)/sizeof(DefaultGUIModel::variable_t);

Acquire::Acquire(void)
    : DefaultGUIModel("Acquire",::vars,::num_vars) {
    
    
    duration = 1000;
    samples = 0;
    trial = 1;
    
    // Initialize the GUI
    setParameter("Duration", duration);
    samples = getSamples(duration);
    
    state_samp = (double)samples;
    setState("Samples", state_samp);
    
    setParameter("Trial", trial);
    
    
    refresh();
}

int Acquire::getSamples(double dur) {
    return static_cast<int>(round(dur / (RT::System::getInstance()->getPeriod()*1e-6)));
}

Acquire::~Acquire(void) {
}

void Acquire::execute(void) {

    // if this is true, should have > 0 samples    
    if(trigger_recording) {
        DataRecorder::startRecording();
        //fprintf(stderr, "Starting Recording\n");
        trigger_recording = false;

    } else if (samples <= 0) {
        DataRecorder::stopRecording();
        setActive(false);
        //fprintf(stderr, "Stopping Recording\n");
        return;
    }
    // don't count down on the first run through, the data recorder
    // doesn't record until the second pass
    samples--;
    //fprintf(stderr, "Samples: %d\n", samples);
    state_samp = (double)samples;
}

void Acquire::update(DefaultGUIModel::update_flags_t flag) {
    if(flag == INIT) {
      
    } if(flag == MODIFY) {
 
        //fprintf(stderr, "Called MODIFY\n");   
        duration = getParameter("Duration").toDouble();
        samples = getSamples(duration);
        //fprintf(stderr, "going to acq for %d samples\n", samples);   
        state_samp = samples;
        
        if (duration > 0) {
            trigger_recording = true;
        } else {
            trigger_recording = false;
        }
        
        trial = getParameter("Trial").toInt();
        //fprintf(stderr, "Mod: setting Trail to %d\n", trial);

    } else if(flag == PAUSE) {
        //fprintf(stderr, "Called PAUSE\n");
                
        // recording over, increment trial
        trial = trial + 1;
        setParameter("Trial", trial);
        //fprintf(stderr, "Pause: setting Trail to %d\n", trial);
        
        // reset samples  
        state_samp = getSamples(getParameter("Duration").toDouble());
        samples = state_samp;
        
        // automatically reset recording
        if (duration > 0) {
            trigger_recording = true;
        } else {
            trigger_recording = false;
        }
        
    } else if(flag == UNPAUSE) {
        //fprintf(stderr, "Called UNPAUSE  samples = %d\n", samples);
        // active = true;
        
        /*if (samples <= 0) {
            fprintf(stderr, "Resetting Recording\n");
            duration = getParameter("Duration").toDouble();       
            samples = getSamples(duration);
            fprintf(stderr, "Resetting Recording. Duration = %f   Samples = %f\n", duration,  samples);
            trigger_recording = true;
        }*/
              
    } else if(flag == PERIOD) {
        samples = getSamples(duration);
    }
}
