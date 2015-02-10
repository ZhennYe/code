#ifndef _SIGNAL_HANDLER_H_
#define _SIGNAL_HANDLER_H_



#include <signal.h>
#include <stdexcept>
#include <map>



// reference
//http://www.gnu.org/s/libc/manual/html_node/Advanced-Signal-Handling.html#Advanced-Signal-Handling


// Create a class that will catch signals and throw exceptions
// To use this file, simply #include it and it will be active
// Catch the appropriate exceptions in try/catch blocks

// To catch different things, derive a class from SignalHandler and override
// defineCatchSignals()


// class to catch signals and throw exceptions
class SignalHandler
{
  public:
    // constructor, calls catchSignals()
    SignalHandler();
    // destructor, calls restorePreviousHandler()
    ~SignalHandler();
    
    // check if any signals caught, if so, throw exception
    inline void check(void);
    
  protected:
    // define which signals to catch
    virtual void defineCatchSignals(void);
    
    // this describes which signals are caught
    std::map<int, std::string> signals;
  
  private:
    // catch signals and throw exceptions
    void catchSignals(void);
    
    // use default signal handler
    void restorePreviousHandler(void);
    
    // the actual signal handler function, a friend function
    friend void SignalHandler_handler(int sigNum);
  
    // bookkeeping:  is the custom handler in use?
    bool useCustomHandler;
    
    // struct that stores signal handler behavior
    struct sigaction handledAction;
    struct sigaction previousAction;    
};



extern volatile sig_atomic_t signalNum;



// define some exceptions, these will be thrown when the corresponding signal
//  is handled
class SignalException : public std::runtime_error
{
  public:
    explicit SignalException() : std::runtime_error("Caught Signal") {}
    explicit SignalException(const std::string & str) : std::runtime_error(str) {}
};



inline void
SignalHandler::check(void)
{
  if(signalNum >= 0) {
    // save the old signalNum
    int sigNum = (int)signalNum;
    // then clear it out
    signalNum = -1;
    // throw a SignalException
    throw SignalException("Caught " + signals[sigNum]);
  }
}



#endif
