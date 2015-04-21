#ifndef SPIKEDETECT_H
#define SPIKEDETECT_H

class SpikeDetect
{

public:

    SpikeDetect(void);
    SpikeDetect(double, double);
    virtual ~SpikeDetect(void);

    // the main function run every time step, contains model logic
    int update(double, double);
    
    int getState();
    void setState(int);
    double getMinInt();
    void setMinInt(double);

private:

    // parameters
    double thresh;
    double minint;
    
    // time of last spike
    double last_spike;
    
    // the internal state variable, sent as output
    int state;
    
    // keep track of time
    int count;
};

#endif //SPIKEDETECT_H
