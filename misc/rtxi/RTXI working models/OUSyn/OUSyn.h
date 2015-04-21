#ifndef OUSYN_H
#define OUSYN_H

/**
 * Record a Single trace
 */

#include <default_gui_model.h>
#include <gsl/gsl_rng.h>

class OUSyn : public DefaultGUIModel
{

public:

    OUSyn(void);
    virtual ~OUSyn(void);
    virtual void execute(void);
    void updateA();

protected:

    virtual void update(DefaultGUIModel::update_flags_t);

private:
    double V, Iout;
    double len;
    double dt;

    //ou parameters
    double taue, taui;
    double De, Di;
    double meane, meani; 
    double correlation;
    double Ae, Ai;

    // conductances
    double Ge, Gi;
    double erev, irev;
    
    // random
    gsl_rng *rng;
    
};

#endif
