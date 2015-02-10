#include "../include/SignalHandler.h"



using namespace std;


volatile sig_atomic_t signalNum = -1;



#include <iostream>

// define the SignalHandler class' signal handler function
void SignalHandler_handler(int sigNum)
{
  signalNum = (sig_atomic_t)sigNum;
}



//////////////////// SignalHandler public member functions ////////////////////
SignalHandler::SignalHandler()
{
  useCustomHandler = false;
  // define the signals
  defineCatchSignals();
  // start catching them
  catchSignals();
}


SignalHandler::~SignalHandler()
{
  // restore handler that was previously active
  restorePreviousHandler();
}



////////////////// SignalHandler protected member functions ///////////////////
void
SignalHandler::defineCatchSignals(void)
{
  signals[SIGINT] = "SIGINT";
  signals[SIGTERM] = "SIGTERM";
  signals[SIGHUP] = "SIGHUP";
  signals[SIGALRM] = "SIGALRM";
  signals[SIGSTOP] = "SIGSTOP";
  signals[SIGQUIT] = "SIGQUIT";
}



//////////////////// SignalHandler private member functions ///////////////////
void
SignalHandler::catchSignals(void)
{
  if(useCustomHandler)
    return;
  
  // preliminaries, don't recall what these do (!)
  sigemptyset (&handledAction.sa_mask);
  handledAction.sa_flags = 0;
  
  // set the handler
  handledAction.sa_handler = SignalHandler_handler;
  
  // register different signals
  map<int, string>::const_iterator sigItr = signals.begin();
  for(; sigItr != signals.end(); ++sigItr)
    sigaction(sigItr->first, &handledAction, &previousAction);
  
  // note that we're now using the custom handler
  useCustomHandler = true;
}



void
SignalHandler::restorePreviousHandler(void)
{
  if(!useCustomHandler)
    return;
  
  // restore the previous handler
  map<int, string>::const_iterator sigItr = signals.begin();
  for(; sigItr != signals.end(); ++sigItr)
    sigaction(sigItr->first, &previousAction, NULL);
  
  // note that we're now using the default handler
  useCustomHandler = false;
}
