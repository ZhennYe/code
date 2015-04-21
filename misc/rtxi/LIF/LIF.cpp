/*
 * 
 */

#include <math.h>
#include <default_gui_model.h>
#include "LIF.h"


extern "C" Plugin::Object *createRTXIPlugin(void) {
    return new LIF();
}

static DefaultGUIModel::variable_t vars[] = {
    {
        "Iapp",
        "Applied Current",
        DefaultGUIModel::INPUT,
    },
    {
        "Vm",
        "Voltage of the Cell",
        DefaultGUIModel::OUTPUT,
    },
    {
        "I_offset",
        "uA/cm^2 - Current added to the input.",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
};

static size_t num_vars = sizeof(vars)/sizeof(DefaultGUIModel::variable_t);

LIF::LIF(void)
    : DefaultGUIModel("LIF",::vars,::num_vars) {

        period = RT::System::getInstance()->getPeriod()*1e-6;
        steps = static_cast<int>(ceil(period/25e-3));

        V = -65;
        I_offset = 0;

        update(INIT);
        refresh();
    }

LIF::~LIF(void) {
}


void LIF::execute(void) {

    double I = I_offset + input(0) * 1e12;

    V = V + period * (-1/1.0) * (0.1 * (V - -70) - I);

    if (V > -40) {
        output(0) = 30 * 1e-3;
        V = -70;
    } else {
        output(0) = V * 1e-3;
    }
}

void LIF::update(DefaultGUIModel::update_flags_t flag) {

    if (flag == INIT) {

        setParameter("I_offset", I_offset);

    } else if (flag == MODIFY) {

        I_offset = getParameter("I_offset").toDouble();

    } else if (flag == PAUSE) {
        output(0) = -65 * 1e-3;

    } else if (flag == PERIOD) {
        period = RT::System::getInstance()->getPeriod()*1e-6;
        steps = static_cast<int>(ceil(period*getParameter("rate").toUInt()/1000.0));
    }
}
