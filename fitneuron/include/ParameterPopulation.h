#ifndef ParameterPopulation_h
#define ParameterPopulation_h



#include "Timer.h"
#include "probability.h"
#include "SignalHandler.h"
#include "ParameterSet.h"
#include "ParameterDescription.h"


#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <limits>
#include <assert.h>


#define USE_EXPERIMENTAL_CROSS

#ifdef USE_EXPERIMENTAL_CROSS
#include <Eigen/Dense>
//USE_COMPILE_FLAG -I${PROJECTDIR}/include
//USE_COMPILE_FLAG -Wno-shadow -Wno-conversion
#endif


double
maxDistance(const ParameterSet & pS1, const ParameterSet & pS2,
            const ParameterDescriptionList & pDList);



// organize summary of a generation in genetic algorithm
struct GenerationSummary
{
  GenerationSummary() {}
  GenerationSummary(const ParameterSet & paramSet,
                    const Timer & genTime,
                    const Timer & totTime,
                    const double & f,
                    const double & logRange,
                    const double & uniformRange);

  friend std::ostream & operator<<(std::ostream & out,
                                   const GenerationSummary & genSum);
  friend std::istream & operator>>(std::istream & in,
                                   GenerationSummary & genSum);

  ParameterSet bestParameters;
  std::string generationTime;
  std::string totalTime;
  double fRange;           // range of values of competetive parameter sets
  double maxLogRange;      // max current range of log-distributed params
  double maxUniformRange;  // max current range of uniform-distributed params
};



/*
 Mandatory function overrides:
   //  initialize() must:
   //    -call setParameterDescriptions
   //    -call setPopulationSize()
   //    -load in any derived-class specific data
   //    -throw an exception on failure
   void initialize(void)

    //  evaluateParameters() should:
    //    -calculate value for any ParameterSet in population whose value is
    //     NaN
    //    -ignore ParameterSet whos value is not NaN
    //    -maintain bestParameters to be a copy of the ParameterSet with the
    //     lowest value
    void evaluateParameters(void);

 Optional function overrides:
   //  cleanup()  -clean up if necessary
   void shutDown(void)
   //  saveSpecific()   -save derived-class specific state/data
   void saveSpecific(std::ofstream & fileOut)
   //  resumeSpecific() -load in derived-class specific state/data
   void resumeSpecific(std::ifstream & fileIn)
*/

class ParameterPopulation
{
  public:
    ParameterPopulation(void);
    virtual ~ParameterPopulation(void);
    
    // optimize population with genetic algorithm
    virtual void geneticAlgorithm(void);
    
    // get the current generation number
    const size_t & getGenerationNum(void) const;
    // get the population size
    const size_t & getPopulationSize(void) const;
    // get the number of evaluations
    const size_t & getNumEvaluations(void) const;
    // get the number of evaluations this generation
    const size_t & getNumGenEvaluations(void) const;
    // get the current best parameters
    const ParameterSet & getBestParameters(void) const;
    
    // set the parameter descriptions (usually called by initialize())
    void setParameterDescriptions(
                           const ParameterDescriptionList & paramDescriptions);
    // set the population size (usually called by initialized())
    void setPopulationSize(const size_t & populationSize);
    
    //  initialize() must:
    //    -do nothing if initialized == true
    //    -call setParameterDescriptions
    //    -call setPopulationSize()
    //    -load in any derived-class specific data
    //    -set initialized = true on success
    //    -throw an exception on failure
    virtual void initialize(void) = 0;
    
    //  evaluatePopulation() must:
    //    -call initialize() at beginning
    //    -calculate value for any ParameterSet in population whose value is
    //     NaN
    //    -increment numEvaluations for each ParameterSet evaluated
    //    -ignore ParameterSet whose value is not NaN
    //    -succeed even if no ParameterSet remains to be evaluated
    //    -suggested: maintain bestParameters to be a copy of the ParameterSet
    //                with the lowest value by calling setBestParameters()
    virtual void evaluatePopulation(void) = 0;

    //  cleanup()
    //    -do nothing if clean == true
    //    -clean up if necessary
    //    -set clean = true on success
    virtual void cleanup(void) { clean = true; }

    // friend operator, write ParameterPopulation to stream
    friend std::ostream & operator<<(std::ostream & out,
                                     const ParameterPopulation & paramPop);
    // friend operator, read ParameterPopulation from stream
    friend std::istream & operator>>(std::istream & in,
                                     ParameterPopulation & paramPop);

    bool logToDisk;              // set true to save resume and best parameters
                                 // to disk
    bool logToTerminal;          // set true to output updates to terminal
    std::string resumeFileName;  // file to save resume information to disk
    std::string resultsFileName; // file to save best parameters to disk
   
    double tol;
    double fTol;
    size_t maxGenerationNum;
   
  protected:
    // evaluate one generation of the genetic algorithm
    virtual void evaluateGeneration(void);
    // Append in a newly evaluated parameter set into the population,
    // increasing population size by 1
    virtual void appendIn(ParameterSet & newPSet);
    // swap in a newly evaluated parameter set into the population, kicking out
    // the worst parameterSet
    virtual void swapIn(ParameterSet & newPSet);
    // randomly generate a parameter set consistent with parameter descriptions
    void randomPSet(ParameterSet & pSet, RandGen & randGen) const;
    // create a new parameter set via differential crossover from the existing
    // population
    void differentialCross(ParameterSet & pSet, RandGen & randGen) const;
    
    #ifdef USE_EXPERIMENTAL_CROSS
    // experimental crossover
    void experimentalCross(ParameterSet & pSet, RandGen & randGen) const;

    mutable Eigen::MatrixXd quadraticMinDataMatrix;
    mutable Eigen::VectorXd quadraticMinDataVector;    
    void fillQuadraticMinData(const std::vector<size_t> & parentInds) const;
    
    mutable Eigen::MatrixXd quadraticFitMatrix;
    void fillQuadraticFitMatrix(void) const;
    
    mutable Eigen::VectorXd quadraticCombinedCoefs;
    mutable Eigen::MatrixXd quadraticCoefficientsMatrix;
    mutable Eigen::VectorXd quadraticCoefficientsVector;
    void computeQuadraticFitCoefficients(void) const;
    
    mutable Eigen::VectorXd quadBounds;
    mutable Eigen::VectorXd quadExtremum;
    void solveQuadraticFit(void) const;
    
    void fillPSetFromQuadraticFit(ParameterSet & pSet,
                                  const size_t minInd) const;
    
    #endif
    //  sets the value of best parameters, handles any necessary logging
    void setBestParameters(const ParameterSet & newBestParameters);
    // save bestParameters to results file
    void saveBestParameters(void) const;
    //  saveSpecific()
    //    -save derived-class specific state/data
    virtual void saveSpecific(std::ofstream & fileOut) const {(void)fileOut;}
    //  resumeSpecific()
    //    -load in derived-class specific state/data
    virtual void resumeSpecific(std::ifstream & fileIn){(void)fileIn;}

    ParameterDescriptionList parameterDescriptions;  // parameter descriptions
    std::vector<ParameterSet> population;  // population of parameter sets
    ParameterSet bestParameters;  // current best parameter set

    size_t numEvaluations;   // number of parameter sets evaluated
    size_t numGenEvaluations;// number of evaluations in this generation
    size_t generationNum;    // number of generation currently being evaluated
    size_t populationSize;   // maximum size of population
    size_t generationSize;   // number of evaluations per generation
    
    bool initialized;        // true after initialize() is called
    bool clean;              // true after cleanup() is called

    SignalHandler signalHandler; // convert signals to exceptions
    mutable GslRandGen r;    // random number generator
        
  private:
    void saveProgress(void) const;
    void resume(void);
    
    void testOptimal(void);
    void saveHistory(void);
    void summarizeGeneration(const size_t & genNum) const;
    inline void get3ParentInds(size_t & ind1, size_t & ind2, size_t & ind3,
                               RandGen & randGen) const;
    inline void getNParentInds(std::vector<size_t> & parentInds,
                               const size_t & numParents,
                               RandGen & randGen) const;
    inline size_t getParent(const size_t & cutoff, RandGen & randGen) const;
   
    bool resumed;            // true if successfully resumed session
    bool optimal;            // true if optimizing is completed
    double fRange;           // range of values of competetive parameter sets
    double maxLogRange;      // max current range of log-distributed params
    double maxUniformRange;  // max current range of uniform-distributed params
    
    // history of bestParameters for each generation:
    std::vector<GenerationSummary> generationHistory;
    
    Timer totalTime;            // total time evaluating all generations
    Timer generationTime;              // time evaluating current generation
    double maxDiffScale;
};


class ParameterPopulationException : ParameterException
{
  public:
    explicit ParameterPopulationException() :
      ParameterException("ParameterDescriptionException") { }
    explicit ParameterPopulationException(const std::string & str) :
      ParameterException(str) { }
};


#endif
