#ifndef _MPI_PARARAMETER_POPULATION_H_
#define _MPI_PARARAMETER_POPULATION_H_



#include "ParameterPopulation.h"
#include "mpi_interface.h"

#include <vector>
#include <fstream>



/*
Class/MPI logic:
  Comms (derived in a chain with .Clone())
  COMM_WORLD -> commShutdown -> commMain -> comm

  The MasterMPI class:
    1)handles the optimization proceedure (via ParameterPopulation)
    2)sends parameters to the SlaveMPI class.          (commMain)
    3)receives and stores the resulting values.        (commMain)
    4)listens for purported new best values.           (commBestParams)
    6)in the event of a new best value for a given slave:
      a)sends back current best overall value          (commBestParams)
      b)if new value is greater than current overall
        best, both procs understand it's a new
        overall best
      c)if a new best, receives the parameters and any
        implementation-specific info                   (commBestParams)
    5)in the event of shut-down, send boolean          (commShutdown)
  The SlaveMPI class:
    1)receives parameters from the MasterMPI class     (commMain)
    2)shuts down if receives boolean on commShutdown   (commShutdown)
    3)performs computations and calculates values
      [implementation-specific]
    4)sends resulting values back                      (commMain)
    5)in the event of a new best for this slave:
      a)sends the best value again                     (commBestParams)
      b)receives master proc's current overall best    (commBestParams)
      c)if new value is greater than current overall
        best, both procs understand it's a new
        overall best
      d)if a new best, sends the parameters and any
        detailed [implementation-specific] info        (commBestParams)
      e)either way updates internal storage of
        best value
  Thus: commMain handles routine sending of parameters and values
        commBestParams handles sending of new best parameters, values,
          and implemenation-specific data.
*/



class MpiParameterPopulation : public ParameterPopulation
{
  public:
    MpiParameterPopulation(void);
    virtual ~MpiParameterPopulation(void);
    // evaluate population
    virtual void evaluatePopulation(void);
    // tell slave processes to shutdown
    virtual void cleanup(void);
    // return true/false if calling process must call MPI::COMM_WORLD.Abort(-1)
    inline bool needAbort(void) { return needsMpiAbort; }
  
    //  initialize() must:
    //    -do nothing if initialized == true
    //    -call setParameterDescriptions
    //    -call setPopulationSize()
    //    -load in any derived-class specific data
    //    -set initialized = true on success
    //    -throw an exception on failure    // problem-specific function
    virtual void initialize(void) = 0;
    
  protected:
    
    // problem-specific functions
    virtual void saveSpecific(std::ofstream & fileOut) const {(void)fileOut;}
    virtual void resumeSpecific(std::ifstream & fileIn){(void)fileIn;}

  private:
    // cycle assignProc through all procs, checking for exception once per loop
    inline void cycleThroughProcs(void);
    // Wait until all existing assigned parameters are calculated.
    void waitForExistingParameters(void);
    // evaluate next existing parameter set whose value is NaN
    void assignExistingParameters(size_t & trialNum);
    // create and evaluate randomly-generated parameter set
    void assignRandomParameters(void);
    // create and evaluate differential-crossover offspring
    void assignDifferentialCrossParameters(void);

    size_t numProcs;  // number of MPI processes
    int assignProc;   // proc to assign next parameter set to
    // different MPI Intracomms are used for different purposes:
    MPI::Intracomm commErr;        // for communicating shutdown
    MPI::Intracomm commMain;       // for communicating parameters/values

    // needed for bookkeeping
    std::vector<std::vector<MPI::Request> > commMainRecvRequests;
    std::vector<std::vector<ParameterSet> > sentParameterSets;
    std::vector<MPI::Request> commErrRecvRequests;
    std::vector<size_t> commErrMessageLengths;
    std::vector<int> commErrRecvTags;
    std::vector<MPI::Request> commMainSendRequests;
    std::vector<int> commMainSendTags;
    std::vector<int> commMainRecvTags;
    MPI::Status status;

    size_t numOutAtOnce;  // make sure each slave proc has this many parameter
                          // sets to evaluate
    size_t numSent;       // number of parameter sets assigned for evaluation
    size_t lastNumEvaluations;  // number evaluated last cycle
    std::vector<size_t> commMainSendInds;  // index which handle to send with
    
    double sleepTime;
    bool firstEval;    // first Population evaluation since starting MPI?
    bool needsMpiAbort;// calling process needs to call MPI::COMM_WORLD.Abort()
};



// exception class for MpiParameterPopulation
class MpiParameterPopulationException : public ParameterPopulationException
{
  public:
    explicit MpiParameterPopulationException() :
      ParameterPopulationException("MpiParameterPopulationException") {}
    explicit MpiParameterPopulationException(const std::string & str) :
      ParameterPopulationException(str) {}
};



class MpiWorker
{
  public:
    MpiWorker(void);
    virtual ~MpiWorker(void);
    void evaluatePopulation(void);
    const size_t & getRank(void) const;
    
    int verbosity;
    
  protected:
    //Problem-specific functions
    virtual void initialize(void) {}
    virtual double evaluateParameters(
                            const std::vector<double> & evalParams) = 0;

    SignalHandler signalHandler;

  private:
    // wait for request(s) to complete than free it/them
    inline void wait(MPI::Request request);
    inline void wait(std::vector<MPI::Request> requests);
    // get first parameter set sent by population
    void getFirstParameters(void);
    // get next parameter set sent by population
    void getNextParameters(void);
    // Asynchronously send value to Master, send best data if new best value
    void sendValue(double value);
    // send message to master process, reporting reason for shutdown
    inline void reportShutdown(const std::string & shutdownMessage);
  
    size_t rank;
    static int masterProc;
    
    // different MPI Intracomms are used for different purposes:
    MPI::Intracomm commErr;        // for communicating shutdown
    MPI::Intracomm commMain;       // for communicating parameters/values
    
    int commMainSendTag;
    int commMainRecvTag;
    size_t numParameters;
    std::vector<double> parameters;
    std::vector<double> nextParameters;
    MPI::Request commMainSendRequest;
    std::vector<MPI::Request> commMainRecvRequests;
    MPI::Request commMainSendHandle;
    mutable MPI::Request shutdownRequest;
    
    double sentValue1;
    double sentValue2;
    bool toggleSendNum;
    bool shutdown;
    char shutdownChar;
};



// Exception class for MpiWorker
class MpiWorkerException : public std::runtime_error
{
  public:
    explicit MpiWorkerException() : std::runtime_error("MpiWorkerException") {}
    explicit MpiWorkerException(const std::string & str) : std::runtime_error(str) {}
 };
// Special shutdown exception, trap it and quit without exception
class MpiWorkerShutdown : public MpiWorkerException
{
  public:
    explicit MpiWorkerShutdown() : MpiWorkerException("shutdown") {}
};

#endif
