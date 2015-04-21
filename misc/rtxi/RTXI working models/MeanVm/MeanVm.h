/*
 * 
 */

#include <default_gui_model.h>
#include <vector>

using namespace std;

class MeanVm : public DefaultGUIModel
{

public:

    MeanVm(void);
    virtual ~MeanVm(void);

    // the main function run every time step, contains model logic
    virtual void execute(void);

protected:

    // run each time model parameters are updated
    virtual void update(DefaultGUIModel::update_flags_t);

private:

    double vm;
    double meanvm;
    double stddev;
    double len;
    double dt;
    
    vector <double> voltage_segment;
 };
