/*
 * 
 */

#include <math.h>
#include <default_gui_model.h>
#include "IZCell.h"


extern "C" Plugin::Object *createRTXIPlugin(void) {
    return new IZCell();
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
        "u",
        "Adaptation Variable",
        DefaultGUIModel::OUTPUT,
    },
    {
        "I_offset",
        "uA/cm^2 - Current added to the input.",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "a",
        "",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "b",
        "",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "c",
        "",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
    {
        "d",
        "",
        DefaultGUIModel::PARAMETER | DefaultGUIModel::DOUBLE,
    },
};

static size_t num_vars = sizeof(vars)/sizeof(DefaultGUIModel::variable_t);

IZCell::IZCell(void)
    : DefaultGUIModel("IZCell",::vars,::num_vars) {

        period = RT::System::getInstance()->getPeriod()*1e-6;
        steps = static_cast<int>(ceil(period/25e-3));

        a = 0.02;
        b = -0.1;
        c = -55;
        d = 6;

        V = c;
        u = V * b;
        I_offset = 0;

        update(INIT);
        refresh();
    }

IZCell::~IZCell(void) {
}


void IZCell::execute(void) {

    double I = I_offset + input(0) * 1e12;

    // V = V + period * (0.04 * V * V + 4.1 * V + 108 - u + I);
    V = V + period * (0.04 * V * V + 5 * V + 140 - u + I);
    u = u + period * a * (b * V - u);

    if (V > 30) {
        output(0) = 30 * 1e-3;
        V = c;
        u = u + d;
    } else {
        output(0) = V * 1e-3;
    }
    output(1) = u;
}

void IZCell::update(DefaultGUIModel::update_flags_t flag) {

    if (flag == INIT) {

        setParameter("I_offset", I_offset);

        setParameter("a",a);
        setParameter("b",b);
        setParameter("c",c);
        setParameter("d",d);

    } else if (flag == MODIFY) {

        I_offset = getParameter("I_offset").toDouble();

        a = getParameter("a").toDouble();
        b = getParameter("b").toDouble();
        c = getParameter("c").toDouble();
        d = getParameter("d").toDouble();

    } else if (flag == PAUSE) {
        output(0) = c * 1e-3;
        V = c;
        u = V * b;

    } else if (flag == PERIOD) {
        period = RT::System::getInstance()->getPeriod()*1e-6;
        steps = static_cast<int>(ceil(period*getParameter("rate").toUInt()/1000.0));
    }
}
