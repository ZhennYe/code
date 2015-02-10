template <class FunctorClass>
class MpiFunctorParameterPop : public MpiParameterPopulation
{
  public:
    virtual void initialize(void);
  
    FunctorClass* functorPtr;
    double (FunctorClass::*f)(const std::vector<double> &);
};



template <class FunctorClass>
void
MpiFunctorParameterPop<FunctorClass>::initialize(void)
{
  if(initialized)
    return;
  
  functorPtr->initialize();
  
  setParameterDescriptions(functorPtr->getParameterDescriptions());
  
  initialized = true;
}


template <class FunctorClass>
class MpiFunctorEvaluator : public MpiWorker
{
  public:
    FunctorClass* functorPtr;
    double (FunctorClass::*f)(const std::vector<double> &);
    
  protected:
    virtual void initialize(void);
    virtual double evaluateParameters(const std::vector<double> & parameters);
};



template <class FunctorClass>
void
MpiFunctorEvaluator<FunctorClass>::initialize(void)
{
  functorPtr->initialize();
}



template <class FunctorClass>
double
MpiFunctorEvaluator<FunctorClass>::evaluateParameters(
  const std::vector<double> & params)
{
  return (functorPtr->*f)(params);
}



template <class FunctorClass>
MinimizeResults
minimizeMpi(FunctorClass & functor,
            double (FunctorClass::* f)(const std::vector<double> &),
            const size_t & populationSize,
            const double & tol, const double & fTol,
            bool logToDisk, bool logToTerminal)
{
  // minimize functor.f, obtain bestParameters, keeping track of numEvaluations
  
  // FunctorClass is required to be a subtype of Minimizable
  Minimizable* p = (FunctorClass*)0;
  (void)p;  // mark p as unused
  
  // create a class to store results
  MinimizeResults results;
  
  const int rank = MPI::COMM_WORLD.Get_rank();
  if(rank == 0) {
    // create a master process
    MpiFunctorParameterPop<FunctorClass> master;
    
    // set the population size
    master.setPopulationSize(populationSize);
    
    // define the function to minimize
    master.functorPtr = &functor;
    master.f = f;
    
    // set the tolerance
    master.tol = tol;
    master.fTol = fTol;
    
    // set the logging rules
    master.logToDisk = logToDisk;
    master.logToTerminal = logToTerminal;
    
    // execute the genetic algorithm
    master.geneticAlgorithm();
    
    // calculate the number of generations
    results.numGenerations = master.getGenerationNum();
    
    // calculate the number of evaluations
    results.numEvaluations = master.getNumEvaluations();

    // set bestParameters to be the parameters found by master
    results.bestParameters = master.getBestParameters();
    
    // try to clean up, and determine if an MPI abort is needed
    master.cleanup();
    results.needsMpiAbort = master.needAbort();
  }
  else {
    // slave process
    MpiFunctorEvaluator<FunctorClass> slave;
    
    // define the function to minimize
    slave.functorPtr = &functor;
    slave.f = f;
    
    // evaluate parameter sets until shutdown
    slave.evaluatePopulation();
    
    // slave has no clear idea what the results are, so make null
    results.numGenerations = 0;
    results.numEvaluations = 0;
    results.bestParameters.parameters.clear();
    results.bestParameters.value = Inf;
    results.needsMpiAbort = false;
  }
  return results;
}
