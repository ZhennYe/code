
#include <default_gui_model.h>

class Acquire : public DefaultGUIModel
{

public:

    Acquire(void);
    virtual ~Acquire(void);

    void execute(void);
    int getSamples(double);

protected:

    void update(DefaultGUIModel::update_flags_t);

private:

    double duration;
    bool trigger_recording;
    int samples;
    double state_samp;
    int trial;

};
