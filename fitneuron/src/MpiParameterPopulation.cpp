#include "../include/MpiParameterPopulation.h"
#include "../include/constants.h"



using namespace std;



//////////////////////////////// Define macros ////////////////////////////////



#define MPI_STARTPROC 1  // start on proc = 1 (not proc = 0)
#define POP_SLEEPTIME 1  // sleep duration in us if defined and nonzero
#define WORKER_SLEEPTIME 100  // sleep duration in us if defined and nonzero

#if defined(POP_SLEEPTIME) || defined(WORKER_SLEEPTIME)
#include <unistd.h>
#endif
//#define USE_EXPERIMENTAL_CROSS

//////////////////////////////// Helper functions /////////////////////////////




/////////////// MpiParameterPopulation public member functions ////////////////



MpiParameterPopulation::MpiParameterPopulation(void) : ParameterPopulation ()
{
  // get the number of processes from COMM_WORLD
  numProcs = MPI::COMM_WORLD.Get_size();
  // clone COMM_WORLD to get commErr
  commErr = MPI::COMM_WORLD.Clone();
  // clone commErr to get the main comm
  commMain = commErr.Clone();
  
  numOutAtOnce = 2;  // make sure slave procs have two parameters out at once

  // allocate space for various bookkeeping lists
  commErrRecvRequests.resize(numProcs);
  commErrMessageLengths.resize(numProcs);
  commErrRecvTags.resize(numProcs);
  commMainSendInds.resize(numProcs);
  commMainRecvRequests.resize(numProcs);
  commMainSendTags.resize(numProcs);
  commMainRecvTags.resize(numProcs);

  for(int proc = MPI_STARTPROC; proc < (int)numProcs; ++proc) {
    commErrRecvTags[proc] = 0;
    commErrRecvRequests[proc] = IRecvMPI(commErrMessageLengths[proc],
                                  proc, commErrRecvTags[proc], commErr);
    commMainSendTags[proc] = 0;
    commMainRecvTags[proc] = 0;
      
    commMainSendInds[proc] = 0;
    commMainRecvRequests[proc].resize(numOutAtOnce);
    for(size_t n = 0; n < numOutAtOnce; ++n)
    	commMainRecvRequests[proc][n] = MPI_REQUEST_NULL;
  }

  firstEval = true;  // haven't evaluated population yet
  needsMpiAbort = false;  // so far, so good!
  assignProc = MPI_STARTPROC;
  sleepTime = 1.0e-4;
}



MpiParameterPopulation::~MpiParameterPopulation(void)
{
  cleanup();
}


void
MpiParameterPopulation::evaluatePopulation(void)
{
  
  initialize();  // necessary if not called by geneticAlgorithm()
  
  if(populationSize == 0) 
    // this should never happen
    throw MpiParameterPopulationException("Error initializing population parameters and/or "
                             "values!\n");
  
  if(firstEval) {
    // This is the first-ever call to evaluatePopulation()
    
    // for each proc, non-blocking send the number of parameters, and allocate
    // space for parameter sets that will be sent to the proc
    const size_t numParameters = parameterDescriptions.size();
    sentParameterSets.resize(numProcs);
    
    for(int proc = MPI_STARTPROC; proc < (int)numProcs; ++proc) {
      // non-blocking send number of parameters to proc
      MPI::Request tempRequest =
        ISendMPI(numParameters, proc, commMainSendTags[proc], commMain);
      tempRequest.Free();  // free send request
      
      // allocate some parameter sets for the proc, flag as unevaluated
      sentParameterSets[proc].resize(numOutAtOnce);
      for(ParameterSet & pSet : sentParameterSets[proc]) {
        pSet.parameters.resize(numParameters);
        pSet.value = Inf;  // this won't be swapped in
      }
    }
    
    if(generationSize == 0)
      generationSize = populationSize;
    numSent = numGenEvaluations;
    lastNumEvaluations = numEvaluations;
    assignProc = MPI_STARTPROC;
    
    // evaluate any existing parameter sets whose value is NaN
    size_t trialNum;
    for(trialNum = 0; trialNum < population.size(); ++trialNum)
      if(isNaN(population[trialNum].value))
        break;
    
    while(trialNum < population.size()) {
      // check if assignProc can take new parameters, if so, send them
      assignExistingParameters(trialNum);
      // cycle assignProc through all procs,
      //  checking for exception once per loop
      cycleThroughProcs();
    }
    
    if(trialNum == generationSize) {
      // this is typically what happens when a one-type population is evaluated
      // *INSTEAD* of running a genetic algorithm. 
      
      // Wait until all existing assigned parameters are calculated.
      waitForExistingParameters();
      
    }
    
    firstEval = false;  // done with the first evaluation, don't do this again
  }
  else {
    numGenEvaluations = 0;
    numSent = 0;
  }
  
  cout << "Population size = " << populationSize << '\n';
  
  // evaluate randomly-generated parameter sets until the population is full
  while(numEvaluations < populationSize &&
         population.size() < populationSize &&
         numGenEvaluations < generationSize) {
    // check if assignProc can take new parameters, if so, send them
    assignRandomParameters();
    // cycle assignProc through all procs, checking for exception once per loop
    cycleThroughProcs();
  }
  
  // evaluate differential-crossover offspring until the generation is complete
  while(numGenEvaluations < generationSize) {
    // check if assignProc can take new parameters, if so, send them
    assignDifferentialCrossParameters();
    // cycle assignProc through all procs, checking for exception once per loop
    cycleThroughProcs();
  }
}



void
MpiParameterPopulation::cleanup(void)
{
  if(clean)
    return;
  
  if(logToTerminal) {
    cout << "Shutting down...\n";
    cout.flush();
  }
  
  // tell each slave process to shutdown
  int proc;
  const char shutdownChar = 'Q';
  for(proc = MPI_STARTPROC; proc < (int)numProcs; ++proc) {
    // non-blocking buffer send shutdown signal (boolean, value irrelevant)
    int tag = 0;
    voidIBSendMPI(shutdownChar, proc, tag, commErr);
  }
  
  
  // wait briefly for slave processes to shutdown
  Timer shutdownTimer;
  const double shutdownMaxWait = 1.0e-4 * (double)numProcs * sleepTime;  // seconds
  for(proc = MPI_STARTPROC; proc < (int)numProcs; ++proc) {
    while(!commErrRecvRequests[proc].Test())
      // still haven't received shutdown notification from slave proc
      if(shutdownTimer.elapsedTime() > shutdownMaxWait) {
        // it's been too long, stop waiting and call MPI::Abort
        break;
      }
      else {
        usleep((useconds_t)sleepTime);
      }
    
    if(shutdownTimer.elapsedTime() > shutdownMaxWait) {
        // it's been too long, stop waiting and call MPI::Abort
        break;
    }
    else {
      string procMessage (commErrMessageLengths[proc], ' ');
      RecvArrMPI(procMessage, proc, commErrRecvTags[proc], commErr);
      if(logToTerminal) {
        cout << "  received from " << proc << ": " << procMessage << '\n';
        cout.flush();
      }
    }
  }
  
  if(proc < (int)numProcs) {
    // waited too long, signal to call MPI::Abort at earliest convenience
    //   Abort() makes a "best attempt" to abort all tasks in the group of comm
    //commErr.Abort(-1);
    // Note: no subsequent code will execute
    needsMpiAbort = true;
  }
  
  // shut down MPI environment
  //MPI::Finalize();
  
  // mark clean
  clean = true;
  
  if(logToTerminal) {
    cout << "\t...done.\n";
    cout.flush();
  }
}



///////////////////// MpiParameterPopulation protected member functions ////////////////////



// none



////////////////////// MpiParameterPopulation private member functions /////////////////////



inline void
MpiParameterPopulation::cycleThroughProcs(void)
{
  // cycle assignProc through all procs, checking for exception once per loop
  
  ++assignProc;
  
  if(assignProc == (int)numProcs) {
    signalHandler.check();  // throw exception if signal caught
    assignProc = MPI_STARTPROC;
    #if defined(POP_SLEEPTIME) && POP_SLEEPTIME > 0
      if(lastNumEvaluations == numEvaluations) {
        sleepTime *= 1.01;
      }
      else {
        sleepTime *= 0.95;
      }
      usleep((useconds_t)sleepTime);
      lastNumEvaluations = numEvaluations;
    #endif
  }
}



void
MpiParameterPopulation::waitForExistingParameters(void)
{
  // Wait until all existing assigned parameters are calculated.
  
  for(int checkProc = MPI_STARTPROC; checkProc < (int)numProcs; ++checkProc) {
    for(auto & receiveRequest : commMainRecvRequests[checkProc]) {
      while(!receiveRequest.Test(status)) {
        signalHandler.check();  // throw exception if signal caught
        usleep((useconds_t)sleepTime);
      }
    
      if(status.Get_source() != MPI_PROC_NULL &&
         status.Get_source() != MPI_ANY_SOURCE) {
        // a parameter set was actually evaluated, inc number of evaluations
        ++numGenEvaluations;
        ++numEvaluations;
      }
    }
  }
}



void
MpiParameterPopulation::assignExistingParameters(size_t & trialNum)
{
  // evaluate next existing parameter set whose value is NaN
  
  // get assignProc's last recieve request
  size_t & sendInd = commMainSendInds[assignProc];
  MPI::Request & receiveRequest = commMainRecvRequests[assignProc][sendInd];
  if(!receiveRequest.Test(status))
    // if worker process isn't ready for a new set of parameters, skip it
      return;
  
  if(status.Get_source() != MPI_PROC_NULL &&
     status.Get_source() != MPI_ANY_SOURCE) {
    // a parameter set was actually evaluated, inc number of evaluations
    ++numGenEvaluations;
    ++numEvaluations;
  }
  
  // increase trialNum until the value of the parameter set is NaN
  for(; trialNum < population.size(); ++trialNum)
    if(isNaN(population[trialNum].value))
      break;
  
  if(trialNum >= population.size()) {
    receiveRequest = MPI_REQUEST_NULL;
    return;
  }

  //Send the new parameters
  voidISendArrMPI(population[trialNum].parameters, assignProc,
                  commMainSendTags[assignProc], commMain);
  ++numSent;
  
  //Request the value of the parameters
  receiveRequest = IRecvMPI(population[trialNum].value, assignProc,
                            commMainRecvTags[assignProc], commMain);

   // inc trialNum
  ++trialNum;
  
  // cycle sendInd
  ++sendInd;
  if(sendInd == numOutAtOnce)
    sendInd = 0;
}



void
MpiParameterPopulation::assignRandomParameters(void)
{
  // create and evaluate randomly-generated parameter set
  
  // get assignProc's last recieve request
  size_t & sendInd = commMainSendInds[assignProc];
  MPI::Request & receiveRequest = commMainRecvRequests[assignProc][sendInd];
  if(!receiveRequest.Test(status))
    // if worker process isn't ready for a new set of parameters, skip it
      return;
  
  // get the parameter set that was sent
  ParameterSet & pSet = sentParameterSets[assignProc][sendInd];
  
  if(status.Get_source() != MPI_PROC_NULL &&
     status.Get_source() != MPI_ANY_SOURCE) {
    // a parameter set was actually evaluated, inc number of evaluations
    
    ++numGenEvaluations;
    ++numEvaluations;
    
    if(population.size() < populationSize) {
      // if the parameter set is not infinitely bad, append it in
      if(pSet.value < Inf)
        appendIn(pSet);
    }
    else
      swapIn(pSet);
  }

  // generate a new parameter set to be evaluated
  if(numSent < populationSize || population.size() < 3)
    // continue generating initial population of random parameter sets
    randomPSet(pSet, r);
  else
    // transition to differential crossover
    #ifndef USE_EXPERIMENTAL_CROSS
      differentialCross(pSet, r);
    #else
      experimentalCross(pSet, r);
    #endif
  
  //Send the new parameters
  voidISendArrMPI(pSet.parameters, assignProc, commMainSendTags[assignProc],
                  commMain);
  ++numSent;
  
  //Request the value of the parameters
  receiveRequest = IRecvMPI(pSet.value, assignProc,
                            commMainRecvTags[assignProc], commMain);
  
  // cycle sendInd
  ++sendInd;
  if(sendInd == numOutAtOnce)
    sendInd = 0;
}



void
MpiParameterPopulation::assignDifferentialCrossParameters(void)
{
  // create and evaluate differential-crossover offspring
  
  // get assignProc's last recieve request
  size_t & sendInd = commMainSendInds[assignProc];
  MPI::Request & receiveRequest = commMainRecvRequests[assignProc][sendInd];
  if(!receiveRequest.Test(status))
    // if worker process isn't ready for a new set of parameters, skip it
      return;
  
  // get the parameter set that was sent
  ParameterSet & pSet = sentParameterSets[assignProc][sendInd];
  
  if(status.Get_source() != MPI_PROC_NULL &&
     status.Get_source() != MPI_ANY_SOURCE) {
    // a parameter set was actually evaluated, inc number of evaluations
    ++numGenEvaluations;
    ++numEvaluations;
    
    // swap the pSet into the population
    //swapIn(pSet);
    if(population.size() < populationSize) {
      // if the parameter set is not infinitely bad, append it in
      if(pSet.value < Inf)
        appendIn(pSet);
    }
    else
      swapIn(pSet);
  }
  
  // generate a new parameter set via differential crossover
  #ifndef USE_EXPERIMENTAL_CROSS
    differentialCross(pSet, r);
  #else
    experimentalCross(pSet, r);
  #endif

  
  //Send the new parameters
  voidISendArrMPI(pSet.parameters, assignProc, commMainSendTags[assignProc],
                  commMain);
  ++numSent;
  
  //Request the value of the parameters
  receiveRequest = IRecvMPI(pSet.value, assignProc,
                            commMainRecvTags[assignProc], commMain);
  
  // cycle sendInd
  ++sendInd;
  if(sendInd == numOutAtOnce)
    sendInd = 0; 
}



///////////////////// MpiWorker static member definitions //////////////////////



int MpiWorker::masterProc = 0;



////////////////////// MpiWorker public member functions ///////////////////////



MpiWorker::MpiWorker(void)
{
  // clone COMM_WORLD to get commErr
  commErr = MPI::COMM_WORLD.Clone();
  // clone commErr to get the main comm
  commMain = commErr.Clone();
  
  // get the rank
  rank = (size_t)MPI::COMM_WORLD.Get_rank();
  
  // set toggleSendNum to true (this value is arbitrary)
  toggleSendNum = true;
  
  // set shutdown to false
  shutdown = false;
  // store dummy char. Doesn't matter what's sent, only that something *was*
  // sent
  shutdownChar = ' ';
    
  numParameters = 0;
  int commErrRecvTag = 0;
  commMainSendTag = 0;
  commMainRecvTag = 0;

  // wait to get shutdown request
  shutdownRequest = IRecvMPI(shutdownChar, masterProc, commErrRecvTag, commErr);

  commMainSendRequest = MPI_REQUEST_NULL;
}



MpiWorker::~MpiWorker(void)
{
  //MPI::Finalize();
}



void
MpiWorker::evaluatePopulation(void)
{
  try {
    // initialize (problem-specific, overridden by derived class)
    initialize();
    
    // get the first two parameter sets (waits until it is received)
    getFirstParameters();
    
    while(!shutdown) {
      // loop until shutdown requested
      // evalute current parameter set (overridden by derived class)
      double value = evaluateParameters(parameters);

      // send value back to master process
      sendValue(std::move(value));
      
      // get more parameters (asynchronous)
      getNextParameters();
    }
    reportShutdown("shutdown");
  }
  catch(exception & e) {
    // report shutdown to master
    reportShutdown(e.what());
  }
}



const size_t &
MpiWorker::getRank(void) const
{
  return rank;
}



///////////////////// MpiWorker protected member functions /////////////////////



////////////////////// MpiWorker private member functions //////////////////////



inline void
MpiWorker::wait(MPI::Request request)
{
  if(shutdownRequest.Test()) {
    shutdown = true;
    //throw MpiWorkerShutdown();
    return;
  }
  signalHandler.check();

  // wait for request to complete and then free it
  while(!request.Test()) {
    if(shutdownRequest.Test()) {
      shutdown = true;
      return;
    }
    signalHandler.check();
    #if defined(WORKER_SLEEPTIME) && WORKER_SLEEPTIME > 0
      usleep(WORKER_SLEEPTIME);
    #endif
  }
}



inline void
MpiWorker::wait(vector<MPI::Request> requests)
{
  // wait for each request to complete and then free it
  for(auto & request : requests)
    wait(request);
}



void
MpiWorker::getFirstParameters(void)
{
  // Get the first set of parameters (wait until they're received)
  
  // First get the number of parameters
  wait( IRecvMPI(numParameters, masterProc, commMainRecvTag, commMain) );
  if(shutdown)
    return;
  
  if(numParameters == 0)
    throw MpiWorkerException("numParameters = 0");
  
  // resize the vectors that hold parameters
  parameters.resize(numParameters);
  nextParameters.resize(numParameters);
  
  // Get the actual first set of parameters
  wait( IRecvArrMPI(parameters, masterProc, commMainRecvTag, commMain) );
  if(shutdown)
    return;
  
  // make non-blocking request for the second set of parameters
  commMainRecvRequests =
    IRecvArrMPI(nextParameters, masterProc, commMainRecvTag, commMain);
}



void
MpiWorker::getNextParameters(void)
{
  // Make sure we have at least one set of parameter to evaluate (wait if
  // necessary) and then asynchronously request one extra set.
  
  //First ensure that previous request for next params has completed
  wait(commMainRecvRequests);
  if(shutdown)
    return;

  //Set the current params to be the previously requested next params
  parameters.swap(nextParameters);

  // make non-blocking request for the next set of parameters
  commMainRecvRequests =
    IRecvArrMPI(nextParameters, masterProc, commMainRecvTag, commMain);
}



void
MpiWorker::sendValue(double value)
{
  // Asynchronously send value to Master
  //  If this is a new best value, synchronously send best data
  
  // while previous value has not yet been received, wait
  //wait(commMainSendRequest);

  if(isNaN(value)) {
    // This is an error condition. Never send NaN, only Inf
    string errMessage = "Computed NaN value from parameters:";
    for(size_t n = 0; n < parameters.size(); ++n)
      errMessage += " " + numToString(parameters[n]);
    throw MpiWorkerException(errMessage);
  }
  
  // copy value into a variable that absolutely WON'T change until send is
  // complete, then non-blocking send value to master process
  if(toggleSendNum) {
    sentValue1 = value;
    voidISendMPI(sentValue1, masterProc, commMainSendTag, commMain);
    toggleSendNum = false;
  }
  else {
    sentValue2 = value;
    voidISendMPI(sentValue2, masterProc, commMainSendTag, commMain);
    toggleSendNum = true;
  }
}



inline void
MpiWorker::reportShutdown(const string & shutdownMessage)
{
  // send message to master process, reporting reason for shutdown
  
  int errTag = 0;
  // send length of shutdown message
  voidIBSendMPI(shutdownMessage.size(), masterProc, errTag, commErr);
  
  // send body of shutdown message
  voidIBSendArrMPI(shutdownMessage, masterProc, errTag, commErr);
}
