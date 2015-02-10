#ifndef _MINIMIZE_MPI_H_
#define _MINIMIZE_MPI_H_



#include "MpiParameterPopulation.h"
#include "Minimizable.h"
#include "constants.h"



// FunctorClass must:
//   -be derived from Minimizable
//   -implement a function (to be minimized) that:
//     -takes as input a vector of doubles
//     -returns a double

class MinimizeResults
{
  public:
    ParameterSet bestParameters;
    size_t       numEvaluations;
    size_t       numGenerations;
    bool         needsMpiAbort;
};


template <class FunctorClass>
MinimizeResults
minimizeMpi(FunctorClass & functor,
            double (FunctorClass::* f)(const std::vector<double> &),
            const size_t & populationSize,
            const double & tol, const double & fTol,
            bool logToDisk, bool logToTerminal);

#include "minimizeMpi_template_src.h"
#endif
