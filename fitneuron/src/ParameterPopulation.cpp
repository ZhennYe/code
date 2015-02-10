#include "../include/ParameterPopulation.h"
#include "../include/io_functions.h"
#include "../include/constants.h"

using namespace std;



////////////////////////////// Helper functions ///////////////////////////////



inline bool
floatsDiffer(const double & x, const double & y)
{
  return (abs(x - y) / (abs(x + y + 1.0)) > 1.0e-9);
}



inline double
randomParamValue(const ParameterDescription & param, RandGen & r)
{
  // return a random value consistant with the ranges and type of the parameter
  //  description, using random number generator r
  switch(param.type()) {
    case ParameterDescription::Constant:
      return param.defaultValue();
    case ParameterDescription::LogDistributed:
      return param.minRange * pow(param.maxRange / param.minRange,
                                  r.openUniform());
    case ParameterDescription::UniformDistributed:
      return param.minRange + (param.maxRange - param.minRange) *
                              r.openUniform();
    case ParameterDescription::Invalid:
    default:
      throw ParameterPopulationException("Invalid parameter ranges for " +
                                         param.name);
      return NaN;
  }
}



inline bool
compatible(const ParameterDescription & pd1,
           const ParameterDescription & pd2)
{
  // check if two parameter descriptions are compatible (i.e. one could
  // plausibly be from start-up, and the other from resuming
    
  // the names must be the same
  if(pd1.name != pd2.name) {
    cout << pd1.name << " != " << pd2.name << '\n';
    cout.flush();
    return false;
  }
  
  // the types must be the same
  if(pd1.type() != pd2.type()) {
    cout << "incompatible desc types\n"; cout.flush();
    return false;
  }
  
  if(pd1.type() == ParameterDescription::Constant) {
    if(floatsDiffer(pd1.defaultValue(), pd2.defaultValue())) {
        cout << "incompatible const\n"; cout.flush();
        cout << pd1 << '\n' << pd2 << '\n'; cout.flush();
      return false;
    }
  }
  else {
    if(floatsDiffer(pd1.minAllowed, pd2.minAllowed) ||
       floatsDiffer(pd1.maxAllowed, pd2.maxAllowed)) {
      // the min and max allowed values must be the same
      cout << "incompatible ranges\n"; cout.flush();
      cout << pd1 << '\n' << pd2 << '\n'; cout.flush();
      return false;
    }
  }
  
  // the ranges don't have to be the same, since that changes during fitting
  return true;
}



inline bool
compatible(const ParameterDescriptionList & pdlResume,
           const ParameterDescriptionList & pdlStartup)
{
  // check if two parameter description lists are compatible (i.e. one could
  // plausibly be from start-up, and the other from resuming
  
  // Check if they have the same length. The resume description may be longer
  // due to added state descriptions
  if(pdlResume.size() < pdlStartup.size()) {
    cout << "incompatible pdl sizes\n"; cout.flush();
    return false;
  }
    
  // loop through parameter descriptions and check if any of them are
  // incompatible. Loop over the startup descriptions
  ParameterDescriptionList::const_iterator pdItr1 = pdlStartup.begin();
  const ParameterDescriptionList::const_iterator end1 = pdlStartup.end();
  ParameterDescriptionList::const_iterator pdItr2 = pdlResume.begin();
  for(; pdItr1 != end1; ++pdItr1, ++pdItr2)
    if(!compatible(*pdItr1, *pdItr2)) {
      cout << "incompatible pds\n"; cout.flush();
      return false;
    }
  
  // All parameter descriptions compatible, so the lists are compatible
  return true;
}



inline void
jumpPastComment(istream & in)
{
  string dummy;
  while(in.good()) {
    switch(in.peek()) {
      case ' ':
      case '\t':
      case '\n':
        // ignore white-space
        in.ignore(1);
        break;
      
      case '#':
        // comment, remove rest of line
        getline(in, dummy);
        break;
      
      default:
        // non-comment, non-white-space
        return;
    }
  }
}



inline size_t
getSize(istream & in)
{
  // get a non-empty, non-comment line and read in a size_t
  
  string line, rawLine;
  SplitLine splitLine;
  while(splitLine.empty()) {
    if(in.bad())
      throw ParameterPopulationException("Error reading size from stream");
    
    getline(in, line);
    rawLine = line;
    in.peek();  //peek helps trip eof flag
    
      // look for comment
    size_t commentInd = line.find('#');
    if(commentInd != std::string::npos) {
      // remove everything after comment
      line.erase(commentInd);
    }
    
    if(line.empty())
      // empty line or just space
      continue;

    // split line up into words
    splitLine = split(line);
    if(splitLine.empty())
      // just empty space on line
      continue;
  }
  
  // should be just one element in line (the size_t)
  if(splitLine.size() != 1)
    throw ParameterPopulationException("Bad line reading ParameterPopulation: "
                                       + rawLine);

  // convert line to size_t
  return stringToSize(splitLine[0]);
}



inline size_t
numWordsInNextLine(istream & in)
{
  // compute the number of words left in the next line, but leave in unchanged
  
  // get the starting position of in stream
  streampos startPos = in.tellg();
  
  // read in the next line
  string line;
  getline(in, line);
  // split it into words
  SplitLine splitLine = split(line);
  // compute the number of words
  size_t numWords = splitLine.size();
  
  // rewind to starting position
  in.seekg(startPos, ios::beg);
  
  return numWords;
}



double
maxDistance(const ParameterSet & pS1, const ParameterSet & pS2,
            const ParameterDescriptionList & pDList)
{
  double maxDist = 0.0;
  for(size_t n = 0; n < pDList.size(); ++n) {
    switch(pDList[n].type()) {
      case ParameterDescription::UniformDistributed:
        maxDist = std::max( maxDist, abs(pS1.parameters[n] -
                                         pS2.parameters[n]) );
        break;
      
      case ParameterDescription::LogDistributed:
        maxDist = std::max( maxDist, abs(log(pS1.parameters[n] /
                                             pS2.parameters[n])) );
        break;
      
      default:
        ;
    }
  }
  return maxDist;
}


template <class T>
inline T
square(const T & x)
{
  return x*x;
}


inline double
pDistance(const ParameterSet & p1, const ParameterSet & p2,
          const ParameterDescriptionList & parameterDescriptions)
{
  double d = 0.0;
  auto p1Itr = p1.parameters.cbegin();
  auto p2Itr = p2.parameters.cbegin();
  for(const auto & desc : parameterDescriptions) {
    switch(desc.type()) {
      case ParameterDescription::LogDistributed:
        d += square(log(*p1Itr / *p2Itr));
        break;
      case ParameterDescription::UniformDistributed:
        d += square(*p1Itr - *p2Itr);
        break;
      default:
        ;
    }
    ++p1Itr;
    ++p2Itr;
  }
  return d;
}




///////////////////// GenerationSummary member functions //////////////////////
GenerationSummary::GenerationSummary(const ParameterSet & paramSet,
                                     const Timer & genTime,
                                     const Timer & totTime,
                                     const double & f,
                                     const double & logRange,
                                     const double & uniformRange) :
  fRange (f), maxLogRange (logRange), maxUniformRange (uniformRange)
{
  // assign bestParameters
  bestParameters = paramSet;
  
  // assign generationTime by:
  stringstream ssGen;
  //   -writing genTime's elapsed time to a stringstream
  ssGen << genTime;
  //   -saving as a string
  generationTime = ssGen.str();
  
  // similarly assing totalTime:
  stringstream ssTot;
  //   -writing genTime's elapsed time to a stringstream
  ssTot << totTime;
  //   -saving as a string
  totalTime = ssTot.str();
}



///////////////////// GenerationSummary friend functions //////////////////////
ostream &
operator<<(ostream & out, const GenerationSummary & genSum)
{ 
    // record format flags
  const ios_base::fmtflags formatFlags = out.flags();
  // record width;
  const streamsize width = out.width();
  // record precision
  const streamsize precision = out.precision();
  
  // calculate a few widths to ensure all the significant info in the double
  //  is output
  const streamsize doublePrecision = numeric_limits<double>::digits10 + 1;
  const streamsize expWidth = (streamsize)ceil(
    log10((double)numeric_limits<double>::max_exponent10));
  const streamsize doubleWidth = doublePrecision + expWidth + 4;
  
  // output the summary
  //   -output the two time strings
  out.width(24);
  out << left << genSum.generationTime;
  out.width(24);
  out << left << genSum.totalTime;
  
  //   -output the range descriptions
  //      -set output decimal precision
  out.precision(doublePrecision);
  // set to default-style output
  out.unsetf(ios_base::floatfield);
  
  //      -output fRange, maxLogRange, maxUniformRange
  out.width(doubleWidth);
  out << left << genSum.fRange;
  out.width(doubleWidth);
  out << left << genSum.maxLogRange;
  out.width(doubleWidth);
  out << left << genSum.maxUniformRange;
  
  //   -output the best parameter set
  out << genSum.bestParameters;

  // restore flags
  out.setf(formatFlags);
  // restore width
  out.width(width);
  // restore precision
  out.precision(precision);
  
  return out;
}



istream &
operator>>(istream & in, GenerationSummary & genSum)
{
  // skip whitespace
  in >> ws;
  // get the generation time (which ends in 's')
  getline(in, genSum.generationTime, 's');
  // the 's' is discarded by getline, so append it to the end
  genSum.generationTime += 's';

  // skip whitespace
  in >> ws;
  // get the total time (which ends in 's')
  getline(in, genSum.totalTime, 's');
  // the 's' is discarded by getline, so append it to the end
  genSum.totalTime += 's';
  
  // older versions of ParameterPopulation don't output fRange, maxLogRange,
  // maxUniformRange. To distinguish this, count the number of words left on
  // the line
  const size_t numWords = numWordsInNextLine(in);
  const size_t paramSize = 1 + genSum.bestParameters.parameters.size();
  if(numWords == paramSize) {
    // didn't output the range values
    genSum.fRange = NaN;
    genSum.maxLogRange = NaN;
    genSum.maxUniformRange = NaN;
  }
  else if(numWords == paramSize + 3) {
    // output the range values
    genSum.fRange = getDouble(in);
    genSum.maxLogRange = getDouble(in);
    genSum.maxUniformRange = getDouble(in);
  }
  else {
    // line is corrupted
    throw ParameterPopulationException("Invalid parameter history line");
  }

  // get the best parameters for this generation  
  in >> genSum.bestParameters;
  
  return in;
}



///////////////// ParameterPopulation public member functions /////////////////



ParameterPopulation::ParameterPopulation(void)
{
  // initializing public values, these can be changed as convenient:
  logToDisk = true;
  logToTerminal = true;
  resumeFileName = "resume.txt";
  resultsFileName = "results.txt";

  tol = 1.0e-3;
  fTol = 1.0e-4;
  maxGenerationNum = 10000;
  maxDiffScale = 0.5;

  // initializing protected / public values, don't edit these:
  generationNum = 1;
  numEvaluations = 0;
  numGenEvaluations = 0;
  generationSize = 0;
  bestParameters.value = Inf;
  
  initialized = false;
  clean = false;
  resumed = false;
  optimal = false;
}



ParameterPopulation::~ParameterPopulation(void)
{
  cleanup();
}



void ParameterPopulation::geneticAlgorithm(void)
{
  // optimize population with genetic algorithm
  
  // start the overall timer
  totalTime.start();
  // initialize (load in parameter descriptions, derived-class data, etc)
  initialize();
  
  // this can't work with population size < 3
  if(populationSize < 3)
    throw ParameterPopulationException("Population size must be >= 3");
  
  if(generationSize == 0)
    generationSize = populationSize;
  
 maxGenerationNum = 1000 * parameterDescriptions.numFitParameters();
  
  try {
    // try to resume
    resume();
    if(resumed) {
      // resume a previous session
      
      if(numGenEvaluations >= populationSize) {
        // stopped after previous generation was complete, so start a new
        // generation
        numGenEvaluations = 0;
        if(generationNum == generationHistory.size())
          ++generationNum;
        // start generation timer
        generationTime.start();
      }
      // don't need to start generationTime generation is in progress so,
      // it's already started
    }
    else {
      // start a new session
      
      optimal = false;
      numEvaluations = 0;
      numGenEvaluations = 0;
      // start generation timer
      generationTime.start();
    }
    
    // make sure we have the parameter descriptions
    if(parameterDescriptions.size() == 0)
      throw ParameterPopulationException("No parameter descriptions loaded");
    
    // make sure the population size has been set
    if(populationSize == 0)
      throw ParameterPopulationException("Population size not set");
    
    // evaluate the first generation of THIS session
    evaluateGeneration();
    
    // continue until optimal (or interrupted)  
    while(!optimal) {
      // start the timer for this generation
      generationTime.start();
      ++generationNum;
      
      // evaluate the population, indicate progress
      evaluateGeneration();
    }
  }
  catch(exception & e) {
    // best to quit without saving, because the state may be messed up
    // (also if signal is caught during evaluateGeneration(), progress will be
    // saved)
    cout << e.what() << '\n';
    cout.flush();
    // try to clean up before exit
    cleanup();
    // rethrow the exception
    exit(0);
    throw;
  }
}



const size_t &
ParameterPopulation::getGenerationNum(void) const
{
  return generationNum;
}



const size_t &
ParameterPopulation::getPopulationSize(void) const
{
  return populationSize;
}


const size_t &
ParameterPopulation::getNumEvaluations(void) const
{
  return numEvaluations;
}



const size_t &
ParameterPopulation::getNumGenEvaluations(void) const
{
  return numGenEvaluations;
}



const ParameterSet &
ParameterPopulation::getBestParameters(void) const
{
  return bestParameters;
}



void
ParameterPopulation::setParameterDescriptions(
                            const ParameterDescriptionList & paramDescriptions)
{
  // copy in the new list of parameter descriptions
  parameterDescriptions = paramDescriptions;
  
  // resize bestParameters
  bestParameters.parameters.resize(parameterDescriptions.size());
}



void
ParameterPopulation::setPopulationSize(const size_t & newPopulationSize)
{
  // set maximum population size, and reserve memory
  populationSize = newPopulationSize;
  
  if(population.size() <= populationSize)
    population.reserve(populationSize);
  else
    population.resize(populationSize);
}



//////////////////// ParameterPopulation friend functions /////////////////////



ostream &
operator<<(ostream & out, const ParameterPopulation & paramPop)
{
  // write out the list of parameter descriptions
  out << paramPop.parameterDescriptions;
  
  // write out the best parameters
  out << "# Current best parameters:\n";
  out << paramPop.bestParameters << '\n';

  // write out the timers
  out << "# Total time:\n";
  out << paramPop.totalTime << '\n';
  out << "# Current generation time:\n";
  out << paramPop.generationTime << '\n' << '\n';

  // write out the number of evaluations
  out << paramPop.numEvaluations << " # number of parameter sets evaluated\n";
  // write out the number of evaluations in this generation
  out << paramPop.numGenEvaluations
      << " # number of evaluations this generation\n";
  // write out the generation number
  out << paramPop.generationNum << " # current generation number\n\n";
  
  // write out the size of the generation history
  out << paramPop.generationHistory.size() << " # generation history\n";
  
  // write out the history of generation summaries
  for(const auto & genHist : paramPop.generationHistory)
    out << genHist;
  out << '\n';
  
  // write out the current population size
  out << paramPop.population.size() << " # population\n";
  
  // write out the members of the population
  for(const auto & pSet : paramPop.population)
    out << pSet;
  
  return out;
}



istream &
operator>>(istream & in, ParameterPopulation & paramPop)
{
  // read in the list of parameter descriptions
  ParameterDescriptionList resumeDescriptions;
  in >> resumeDescriptions;
  if(paramPop.parameterDescriptions.empty())
    // no existing parameter descriptions, so set them to the resume
    // descriptions
    paramPop.setParameterDescriptions(resumeDescriptions);
  if( compatible(resumeDescriptions, paramPop.parameterDescriptions) )
    // have parameter descriptions read in from start up that are compatible
    // with the resume descriptions. The resume descriptions are a refinement
    // due to fitting, so use them
    paramPop.parameterDescriptions = resumeDescriptions;
  else
    // inconsistent parameter descriptions
    throw ParameterPopulationException("Parameter descriptions in resume file "
                                       "are inconsistent with start up");
  
  // read in the best parameters
  jumpPastComment(in);
  in >> paramPop.bestParameters;

  // read in the timers
  jumpPastComment(in);
  in >> paramPop.totalTime;
  jumpPastComment(in);
  in >> paramPop.generationTime;

  // read in the number of evaluations
  paramPop.numEvaluations = getSize(in);
  jumpPastComment(in);
  // read in the number of evaluations this generation
  paramPop.numGenEvaluations = getSize(in);
  jumpPastComment(in);
  // read in the generation number
  paramPop.generationNum = getSize(in);

  // read in the size of the the generation history
  jumpPastComment(in);
  size_t len = getSize(in);
  paramPop.generationHistory.resize(len);
  
  // read in the history of best parameters
  jumpPastComment(in);
  const size_t numParams = paramPop.bestParameters.parameters.size();
  for(auto & genHist : paramPop.generationHistory) {
    genHist.bestParameters.parameters.resize(numParams);
    in >> genHist;
  }
  
  // read in the population size
  len = getSize(in);
  if(paramPop.populationSize == 0)
    // population not already resized
    paramPop.setPopulationSize(len);
  
  const size_t numRead = std::min(len, paramPop.populationSize);
  
  paramPop.population.resize(numRead);
  
  // read in the members of the population
  jumpPastComment(in);
  for(auto & pSet : paramPop.population) {
    pSet.parameters.resize(numParams);
    in >> pSet;
  }
  
  return in;
}



/////////////// ParameterPopulation protected member functions ////////////////



void
ParameterPopulation::evaluateGeneration(void)
{
  // Once-per-generation evaluation function:
  //   -evaluate the parameter population
  //   -maintain bestParameters and history,
  //   -test for optimality
  //   -save progress
  //   -spit out progress indicator
  
  try {
    signalHandler.check();  // throw exception if signal caught
    
    // evaluate the population (derived-class specific function)    
    evaluatePopulation();
        
    // sort the population to be in order from best to worst
    signalHandler.check();  // throw exception if signal caught
  
    // test to see if this generation has achieved optimality
    //   (set this->optimal = true)
    testOptimal();
    
    // save history of this generation
    saveHistory();
    
    // save progress
    signalHandler.check();  // throw exception if signal caught
    saveProgress();
  }
  catch(SignalException & sigExcept) {
    // signal caught, save progress before quitting
    saveProgress();
    // try to clean up
    cleanup();
    // re-throw the exception
    throw;
  }
  catch(exception & e) {
    // some other exception, just try to clean up
    cleanup();
    // re-throw the exception
    throw;
  }
  
  // print out progress indicator
  summarizeGeneration(generationNum);
}



void
ParameterPopulation::randomPSet(ParameterSet & pSet, RandGen & randGen) const
{
  // randomly generate a parameter set consistent with parameter descriptions
  
  // set value to NaN (flag that it needs to be evaluated)
  pSet.value = NaN;
  pSet.diffScale = maxDiffScale;
  
  // loop through different parameters within pSet, assign random values
  auto paramDescriptionItr = parameterDescriptions.cbegin();
  for(double & paramVal : pSet.parameters) {
    // assign the parameter a random value within range
    paramVal = randomParamValue(*paramDescriptionItr, randGen);
    ++paramDescriptionItr;
  }
}



void
ParameterPopulation::appendIn(ParameterSet & newPSet)
{
  // Append in a newly evaluated parameter set into the population, keeping the
  // population ordered in increasing value.
  // Increase the population size by 1.
  
  if(population.empty()) {
    // this is the first parameter set
    population.push_back(newPSet);
    setBestParameters(newPSet);
    return;
  }
  // Add copy of parameter set to population
  population.push_back(newPSet);

  // make iterator to newly added pSet
  auto oldPSetItr1 = population.rbegin();
  auto oldPSetItr2 = oldPSetItr1;
  // keep swapping until the parameter sets are in value order
  for(++oldPSetItr2; oldPSetItr2 != population.rend(); ++oldPSetItr2) {
    if(oldPSetItr1->value >= oldPSetItr2->value)
      break;
    // swap parameter sets
    oldPSetItr2->swap(*oldPSetItr1);
    ++oldPSetItr1;
  }
  
  // Swapped all the way to the front, the new set is the best ParameterSet
  if(oldPSetItr2 == population.rend())
    setBestParameters(*oldPSetItr1);
}



void
ParameterPopulation::swapIn(ParameterSet & newPSet)
{
  // Swap in a newly evaluated parameter set into the population, keeping the
  // population ordered in increasing value.
  // Kicking out the worst parameterSet, keeping the population size constant.
  
  // Form iterator to worst existing member of the population
  auto oldPSetItr1 = population.rbegin();
  if(newPSet.value >= oldPSetItr1->value)
    // newPSet isn't good enough to join the population
    return;
  
  // swap the worst parameter set out, and the new one in
  std::swap(newPSet, *oldPSetItr1);
  //newPSet.swap(*oldPSetItr1);
  
  // keep swapping until the parameter sets are in value order
  auto oldPSetItr2 = oldPSetItr1;
  for(++oldPSetItr2; oldPSetItr2 != population.rend(); ++oldPSetItr2) {
    if(oldPSetItr1->value >= oldPSetItr2->value)
      break;
    // swap parameter sets
    std::swap(*oldPSetItr2, *oldPSetItr1);
    //oldPSetItr2->swap(*oldPSetItr1);
    ++oldPSetItr1;
  }
  
  // Swapped all the way to the front, the new set is the best ParameterSet
  if(oldPSetItr2 == population.rend())
    setBestParameters(*oldPSetItr1);
}



void
ParameterPopulation::differentialCross(ParameterSet & pSet, RandGen & randGen)
  const
{
  // produce a new member of the population by using a differential crossover
  //   -three parents chosen from breeding population
  //   -parent1 more fit than parent2
  //      -an improvement direction vector obtained from parent1 - parent2
  //      -this vector is scaled by a random uniform number "diffScale"
  //   -parent3 is a starting location updated by improvement vector
  //      -for uniform parameters update as:
  //         child = parent3 + diffScale * (parent1 - parent2)
  //      -for logarithmic parameters update as:
  //         child = parent3 * pow(parent1 / parent2, diffScale)
  
  // assign the new pSet an undefined value
  pSet.value = NaN;
  
  
  // get the three parents from the breeding population (weighted
  // preferentially to better fitness), chosen so that ind1 < ind2 (swap if
  // necessary), implying that the fitness of parent1 is better than the
  // fitness of parent2
  //   -pick ind2 using cutoff-1 and ind3 using cutoff-2 because we're
  //    excluding the possibility of any of the indices matching
  
  // Randomly draw indices to three members of the population
  size_t ind1, ind2, ind3;
  get3ParentInds(ind1, ind2, ind3, randGen);
  
  // get progenitors from the population, via the indices
  const ParameterSet & parent1 = population[ind1];
  const ParameterSet & parent2 = population[ind2];
  const ParameterSet & parent3 = population[ind3];
  
  // throw a uniform random number to scale the differential cross
  static const double minDiffScale = 0.01;
  /*
  static const double maxDiffScale2 = log(5.0) /
    log((double)parameterDescriptions.numFitParameters() + 1.0);
  */
  //static const double maxDiffScale2 = 12.0 /
  //  ((double)parameterDescriptions.numFitRegularParameters() + 2.0);
  static const double maxDiffScale2 = 0.5;
  
  const size_t compInd = ind3 == 0 ? 1 :
    (size_t) round( 0.1 * (double) ind3 );
  const double & compVal = population[compInd].value;
  const double valueFact = 
    abs( (parent3.value - compVal) / (parent2.value - parent1.value) );
  const double diffScale = std::min(std::max(
    pow( 2.0, -1.0 + 2.0 * randGen.openUniform() ) * valueFact,
    minDiffScale
  ), maxDiffScale2);
  
  
  //static const double diffRange = maxDiffScale2 - minDiffScale;
  //const double diffScale = minDiffScale + diffRange * randGen.openUniform();
  
  pSet.diffScale = diffScale;
  
  // loop over each parameter, computing new value via differential crossover
  auto p1Itr = parent1.parameters.cbegin();
  auto p2Itr = parent2.parameters.cbegin();
  auto p3Itr = parent3.parameters.cbegin();
  auto paramDescriptionItr = parameterDescriptions.cbegin();
  for(auto & newParamVal : pSet.parameters) {
    switch(paramDescriptionItr->type()) {
      case ParameterDescription::Constant:
        // this parameter is constant, set to only allowed value
        newParamVal = paramDescriptionItr->maxAllowed;
        break;
      
      case ParameterDescription::LogDistributed:
        // use logarithmic differential crossover
        newParamVal = *p3Itr * pow(*p1Itr / *p2Itr, diffScale);
        
        // ensure newParamVal is in bounds
        if(newParamVal < paramDescriptionItr->minAllowed)
          newParamVal = paramDescriptionItr->minAllowed;
        else if(!(newParamVal <= paramDescriptionItr->maxAllowed))
          newParamVal = paramDescriptionItr->maxAllowed;
        break;
      
      case ParameterDescription::UniformDistributed:
        // use linear differential crossover
        newParamVal = *p3Itr + diffScale * (*p1Itr - *p2Itr);
        
        // ensure newParamVal is in bounds
        if(newParamVal < paramDescriptionItr->minAllowed)
          newParamVal = paramDescriptionItr->minAllowed;
        else if(!(newParamVal <= paramDescriptionItr->maxAllowed))
          newParamVal = paramDescriptionItr->maxAllowed;
        break;

      case ParameterDescription::Invalid:
      default:
        throw ParameterPopulationException("Invalid parameter \"" +
                                           paramDescriptionItr->name + "\"");
    }
    
    ++p1Itr;
    ++p2Itr;
    ++p3Itr;
    ++paramDescriptionItr;
  }
}


#ifdef USE_EXPERIMENTAL_CROSS
#include <Eigen/Eigenvalues>
using namespace Eigen;
void
ParameterPopulation::experimentalCross(ParameterSet & pSet, RandGen & randGen)
  const
{
  // try to implement stochastic quadratic optimizer
  
  const size_t numFitParameters = parameterDescriptions.numFitParameters();
  // How many parameter sets needed to exactly specify quadratic fit?
  // numQuad = numSecondDerivs + numFirstDerivs + 1
  //         = numHessian + numFitParameters + 1
  //         = numFitP * (NumFitP + 1) / 2 + numFitP + 1
  const size_t numQuad = (numFitParameters + 1) * (numFitParameters + 2) / 2;
  const size_t numExtra = 2; // max(2, 0.1 * numFitParameters)
  const size_t numParents = numQuad + numExtra;
  //const size_t numParents = min(numQuad + numExtra, (size_t)10);

  // Form list of numParents randomly-chosen parameter indices
  // redo this to use set?
  vector<size_t> parentInds (numParents);
  getNParentInds(parentInds, numParents, randGen);
  
  // 1. Find the best parent, then
  // 2. Fill out a matrix of parameter differences between the remaining parents
  //    and the best parent
  // 3. Fill out a vector of value differences between the remaining parents and
  //    the best parent
  // quadraticMinDataMatric = paramVecs - bestParamVec
  // quadraticMinDataVector = paramVals - bestParamVal
  quadraticMinDataMatrix.resize(numParents-1, numFitParameters);
  quadraticMinDataVector.resize(numParents-1);  
  fillQuadraticMinData(parentInds);
  
  // Fill out a series of equations describing a quadratic fit to the
  //   quadraticMinData
  // Form a (numParents-1) x (numQuad-1) matrix (quadraticFitMatrix) where:
  //   -each row corresponds to a parameter difference from quadraticMinData
  //   -the first numFitParams columns are the data in quadraticMinDataMatrix
  //   -the next numHessian columns are quadratic combinations of this data
  //      e.g. minDataMat(column1) * minDataMat(column7)
  // Then quadraticFitMatrix * quadraticCombinedCoefs = quadraticMinDataVector
  //   is a forumula specifying coefficients for a quadratic fit to the parents
  quadraticFitMatrix.resize(numParents-1, numQuad-1);
  fillQuadraticFitMatrix();
  
  // Solve those equations to compute the coefficients of the quadratic fit
  // 1. Solve for quadraticCombinedCoefs:
  //    quadraticFitMatrix * quadraticCombinedCoefs = quadraticMinDataVector
  // 2. Break quadratic combined coefs into a matrix, so that for an arbitrary
  //    deviation from the bestParent dP, the change in value dV is:
  //    dV = dP' * quadCoefsMat * dP - 2 * quadCoefsVec * dP
  quadraticCombinedCoefs.resize(numQuad-1);
  quadraticCoefficientsMatrix.resize(numFitParameters, numFitParameters);
  quadraticCoefficientsVector.resize(numFitParameters);
  computeQuadraticFitCoefficients();
  
  // find reglarized "minimum" from quadraticFitCoefficients
  // Since dV = dP' * quadCoefsMat * dP - 2 * quadCoefsVec * dP,
  //  the unregularized extremum/saddlepoint is found by taking dV/dP = 0:
  //  minDP = quadCoefsMat^-1 * quadCoefsVec
  // Function is more complex because we need to regularize and guarantee
  // "minimum" (or at least decrease in value)
  quadExtremum.resize(numFitParameters);
  solveQuadraticFit();
  
  // quadExtremum now holds information about how to alter best parent pSet
  //   to get to minimum. Fill out a new pSet from that info
  fillPSetFromQuadraticFit(pSet, parentInds[0]);
}


inline void
ParameterPopulation::fillQuadraticMinData(const vector<size_t> & parentInds)
  const
{
  // Fill out a matrix of differences between randomly chosen parameters and
  //   the best pSet among them
  
  // the best (lowest value) parentInd is at the beginning of the vector
  const size_t minInd = parentInds[0];
  
  // 2. Fill out a matrix of parameter differences between the remaining parents
  //    and the best parent
  // 3. Fill out a vector of value differences between the remaining parents and
  //    the best parent
  // quadraticMinDataMatric = paramVecs - bestParamVec
  // quadraticMinDataVector = paramVals - bestParamVal
  const auto & minPSet = population[minInd];
  size_t row = 0;
  for(const size_t & ind : parentInds) {
    if(ind == minInd) {
      continue;
    }
    const auto & diffPSet = population[ind];
    size_t col = 0;
    for(size_t paramNum = 0; paramNum < minPSet.parameters.size(); ++paramNum){
      if(parameterDescriptions[paramNum].isConstant()) {
        continue;
      }
      else if(parameterDescriptions[paramNum].isLogDistributed()) {
        quadraticMinDataMatrix(row,col) =
          log(diffPSet.parameters[paramNum] / minPSet.parameters[paramNum]);
      }
      else {
        quadraticMinDataMatrix(row,col) =
          diffPSet.parameters[paramNum] - minPSet.parameters[paramNum];
      }
      ++col;
    }
    quadraticMinDataVector(row) = diffPSet.value - minPSet.value;
    ++row;
  }
}


inline void
ParameterPopulation::fillQuadraticFitMatrix(void) const
{
  // Fill out a series of equations describing a quadratic fit to the
  //   quadraticMinData
  
  // Form a (numParents-1) x (numQuad-1) matrix (quadraticFitMatrix) where:
  //   -each row corresponds to a parameter difference from quadraticMinData
  //   -the first numFitParams columns are the data in quadraticMinDataMatrix
  //   -the next numHessian columns are quadratic combinations of this data
  //      e.g. minDataMat(column1) * minDataMat(column7)
  // Then quadraticFitMatrix * quadCoefs = quadraticMinDataVector is a forumula
  //   specifying coefficients for a quadratic fit to the parents
  const size_t numFitParameters = parameterDescriptions.numFitParameters();
  for(size_t row = 0; row < (size_t)quadraticFitMatrix.rows(); ++row) {
    size_t col = 0;
    for(size_t dataCol = 0; dataCol < numFitParameters; ++dataCol, ++col) {
      quadraticFitMatrix(row, col) = quadraticMinDataMatrix(row, dataCol);
    }
    for(size_t dataCol1 = 0; dataCol1 < numFitParameters; ++dataCol1) {
      for(size_t dataCol2 = dataCol1;
          dataCol2 < numFitParameters;
          ++dataCol2, ++col) {
        quadraticFitMatrix(row, col) = quadraticMinDataMatrix(row, dataCol1)
                                     * quadraticMinDataMatrix(row, dataCol2);
      }
    }
  }
}


inline void
ParameterPopulation::computeQuadraticFitCoefficients(void) const
{
  // Solve equations to compute the coefficients of the quadratic fit
  // 1. Solve for quadraticCombinedCoefs:
  //    quadraticFitMatrix * quadraticCombinedCoefs = quadraticMinDataVector
  quadraticCombinedCoefs
    = quadraticFitMatrix.jacobiSvd(ComputeThinU | ComputeThinV).solve(quadraticMinDataVector);

  // 2. Break quadratic combined coefs into a matrix, so that for an arbitrary
  //    deviation from the bestParent dP, the change in value dV is:
  //    dV = dP' * quadCoefsMat * dP - 2 * quadCoefsVec * dP
  size_t vecInd;
  const size_t numFitParameters = parameterDescriptions.numFitParameters();
  for(vecInd = 0; vecInd < numFitParameters; ++vecInd) {
    // factor of -0.5 because we want dV ~ -2 * quadCoefsVec * dP, but
    //  dV ~ quadCombinedCoefs * dV
    quadraticCoefficientsVector[vecInd] = -0.5 * quadraticCombinedCoefs[vecInd];
  }
  for(size_t row = 0; row < numFitParameters; ++row) {
    quadraticCoefficientsMatrix(row,row) = quadraticCombinedCoefs[vecInd];
    ++vecInd;
    for(size_t col = row + 1; col < numFitParameters; ++col, ++vecInd) {
      // factor of +0.5 because dV ~ dP' * quadCoefsMat * dP, but that
      // counts each off-diagonal product twice (top and bottom triangle),
      // whereas quadraticCombinedCoefs counts each product once
      quadraticCoefficientsMatrix(row,col)
        = 0.5 * quadraticCombinedCoefs[vecInd];
      quadraticCoefficientsMatrix(col,row)
        = quadraticCoefficientsMatrix(row,col);
    }
  }
}

inline void
ParameterPopulation::solveQuadraticFit(void) const
{
  // find reglarized "minimum" from quadraticFitCoefficients
  // Since dV = dP' * quadCoefsMat * dP - 2 * quadCoefsVec * dP,
  //  the unregularized extremum/saddlepoint is found by taking dV/dP = 0:
  //  minDP = quadCoefsMat^-1 * quadCoefsVec
  // This function is more complicated because we need to regularize and
  // guarantee "minimum" (or at least decrease in value)
  
  // quadraticFitCoefficientsMatrix is real, symmetric, so can diagonalize:
  SelfAdjointEigenSolver<MatrixXd> es (quadraticCoefficientsMatrix);
  // now:
  // quadraticFitCoefficientsMatrix == es.eigenvectors()
  //                                 * es.eigenvalues().asDiagonal()
  //                                 * es.eigenvectors.transpose()
  // so the unregularized extremum is at:
  // ex = es.eigenvectors() * es.eigenvalues().asDiagonal().inverse()
  //      * es.eigenvectors.transpose() * quadraticFitCoefficientsVector;
  
  // To regularize and deal with local maxima/saddlepoints, work in the space
  // where quadraticFitCoefficientsMatrix is diagonal!
  // 1. Transform quadraticFitCoefficientsVector and quadraticMinDataMatrix into
  //    eigenspace of quadraticFitCoefficients
  quadraticCoefficientsVector
    = es.eigenvectors().transpose() * quadraticCoefficientsVector;
  quadraticMinDataMatrix = quadraticMinDataMatrix * es.eigenvectors();
  
  // 2. Find the smallest hyper-rectangle bounding that space evenly around
  //    the best parent. Then scale it up to allow modest expansion of space
  const double quadScaleFactor = 2.0;
  quadBounds = quadScaleFactor
    * quadraticMinDataMatrix.cwiseAbs().colwise().maxCoeff();
  
  // 3. Find "minimum" by dividing quadraticFitCoefficientsVector by
  //    abs(es.eigenvalues), while bounding to hyper-rectangle
  const size_t numFitParameters = parameterDescriptions.numFitParameters();
  for(size_t ind = 0; ind < numFitParameters; ++ind) {
    const double vec_n = quadraticCoefficientsVector[ind];
    
    if(vec_n > 0) {
      quadExtremum[ind] = min(vec_n / abs(es.eigenvalues()[ind]),
                              quadBounds[ind]);
    }
    else if(vec_n < 0) {
      quadExtremum[ind] = max(vec_n / abs(es.eigenvalues()[ind]),
                              -quadBounds[ind]);
    }
    else {
      // vec_n is exactly 0.0
      quadExtremum[ind] = 0.0;
    }
  }
  
  // 4. Transform solution back to original space
  quadExtremum = es.eigenvectors() * quadExtremum;
}


void
ParameterPopulation::fillPSetFromQuadraticFit(ParameterSet & pSet,
                                              const size_t minInd) const
{
  // quadExtremum now holds information about how to alter best parent pSet
  //   to get to minimum. Fill out a new pSet from that info

  // assign the new pSet an undefined value
  pSet.value = NaN;

  // modify minPSet according to the deltas in quadExtremum
  const auto & minPSet = population[minInd];

  size_t col = 0;
  auto paramDescItr = parameterDescriptions.cbegin();
  for(size_t paramNum = 0;
      paramNum < pSet.parameters.size();
      ++paramNum, ++paramDescItr) {
    double & newParamVal = pSet.parameters[paramNum];
    if(paramDescItr->isConstant()) {
      newParamVal = minPSet.parameters[paramNum];
      continue;
    }
    else if(paramDescItr->isLogDistributed()) {
      newParamVal = minPSet.parameters[paramNum] * exp(quadExtremum[col]);
    }
    else {
      newParamVal = minPSet.parameters[paramNum] + quadExtremum[col];
    }

    ++col;
    
    // ensure newParamVal is in bounds
    if(newParamVal < paramDescItr->minAllowed) {
      newParamVal = paramDescItr->minAllowed;
    }
    else if(!(newParamVal <= paramDescItr->maxAllowed)) {
      newParamVal = paramDescItr->maxAllowed;
    }
  }
}

#endif

void
ParameterPopulation::setBestParameters(const ParameterSet & newBestParameters)
{
  // set the best parameters and save them
  
  bestParameters = newBestParameters;
  saveBestParameters();
}



void
ParameterPopulation::saveBestParameters(void) const
{
  // save bestParameters to results file
  
  if(!initialized || !logToDisk)
    // don't bother trying if not initialized, or not logging to disk
    return;
  
  // these better be true
  assert(!bestParameters.parameters.empty());
  assert(!parameterDescriptions.empty());
  
    // open results file for writing
  ofstream out (resultsFileName.c_str());
  if(out.fail())
    throw ParameterPopulationException("Can't open " + resultsFileName +
                                       " for writing");

  try {
    // calculate a few widths to ensure all the significant info in the double
    //  is output
    const streamsize doublePrecision = numeric_limits<double>::digits10 + 1;
    const streamsize expWidth = (streamsize)ceil(
      log10((double)numeric_limits<double>::max_exponent10));
    const streamsize doubleWidth = doublePrecision + expWidth + 4;
    
    // set output decimal precision
    out.precision(doublePrecision);
    
    // get length of longest parameter description name
    ParameterDescriptionList::const_iterator descItr =
      parameterDescriptions.begin();
    streamsize nameWidth = 5;
    for(; descItr != parameterDescriptions.end(); ++descItr)
      if(descItr->name.size() > (size_t)nameWidth)
        nameWidth = descItr->name.size();
    nameWidth += 1;

    // write out the function value
    out.width(nameWidth);
    out << left << "value";
    out.width(doubleWidth);
    out << right << bestParameters.value << '\n';
    
    // write out the parameters
    vector<double>::const_iterator bestItr = bestParameters.parameters.begin();
    descItr = parameterDescriptions.begin();
    for(; descItr != parameterDescriptions.end(); ++descItr, ++bestItr) {
      out.width(nameWidth);
      out << left << descItr->name;
      out.width(doubleWidth);
      out << right << *bestItr << '\n';
    }
    
    out.close();
  }
  catch(exception & e){
    // close the file
    out.close();
    // re-throw error
    throw;
  }
}



//////////////// ParameterPopulation private member functions /////////////////



void
ParameterPopulation::saveProgress(void) const
{
  // save state of ParameterPopulation to resume file
  
  if(!initialized || !logToDisk)
    // don't bother trying if not initialized, or not logging to disk
    return;
  
  // these better be true
  assert(!population.empty());
  assert(!bestParameters.parameters.empty());
  assert(!parameterDescriptions.empty());
  
  // create a temporary resume file to avoid destroying old resume information
  // if shutdown occurs during saving
  const string tempResumeFileName = resumeFileName + ".temp";
  
  // move the old resume information to the temporary file
  rename(resumeFileName.c_str(), tempResumeFileName.c_str());
  
  // open resume file for writing
  ofstream fileOut (resumeFileName.c_str());
  if(fileOut.fail())
    throw ParameterPopulationException("Can't open " + resumeFileName +
                                       " for writing");
  try {
    // write out *this
    fileOut << *this;
    
    // derived-class specific save routine
    saveSpecific(fileOut);
    
    // close resume file
    fileOut.close();
  }
  catch(exception & e) {
    // close the file
    fileOut.close();
    
    // remove the resume file
    remove(resumeFileName.c_str());
    
    // copy the old version back
    rename(tempResumeFileName.c_str(), resumeFileName.c_str());
     
    // re-throw error
    throw;
  }
  
  // remove temporary file
  remove(tempResumeFileName.c_str());

  if(logToTerminal) {
    cout << "Progress saved to " << resumeFileName << '\n';
    cout.flush();
  }
}



void
ParameterPopulation::resume(void)
{
  // check to see if program was killed during saveProgress()
  // (can happen with kill -9, or pulled plug)
  const string tempResumeFileName = resumeFileName + ".temp";
  if(fileExists(tempResumeFileName)) {
    if(logToTerminal) {
      cout << "Cleaning up corrupted resume file...";
      cout.flush();
    }
    
    // remove the (presumably corrupt) permanent resume file
    remove(resumeFileName.c_str());
    // restore temporary resume file
    rename(tempResumeFileName.c_str(), resumeFileName.c_str());
    
    if(logToTerminal) {
      cout << "done.\n";
      cout.flush();
    }
  }
  
  ifstream fileIn (resumeFileName.c_str());
  if(fileIn.fail()) {
    // if resume file can't be opened, don't resume
    resumed = false;
    return;
  }
  
  if(logToTerminal) {
    cout << "Resuming previous session from " << resumeFileName << " ... ";
    cout.flush();
  }

  try {
    // read in *this
    fileIn >> *this;
    
    // derived-class specific resume routine
    resumeSpecific(fileIn);
    
    // close resume file
    fileIn.close();
  }
  catch(exception & e) {
    // close resume file
    fileIn.close();
    // re-throw error
    throw;
  }

  if(logToTerminal) {
    cout << "success.\n";
    cout.flush();
  }
  resumed = true;
  
  // test if population is optimal
  testOptimal();
  
  // re-display the progress from the previous session(s)
  for(size_t genNum = 1; genNum <= generationHistory.size(); ++genNum)
    summarizeGeneration(genNum);
  return;
}



void
ParameterPopulation::testOptimal(void)
{
  // Max current range of log-distributed params
  maxLogRange = 1.0;
  // Max current range of uniform-distributed params
  maxUniformRange = 0.0;
  
  // initialize the min and max range of each parameter description
  auto paramItr = population.front().parameters.cbegin();
  for(auto & paramDescription : parameterDescriptions) {
    if(paramDescription.isConstant()) {
      ++paramItr;
      continue;
    }
    
    // initialize this parameter description's min/max to the best parameter
    // set's value
    paramDescription.minRange = *paramItr;
    paramDescription.maxRange = paramDescription.minRange;
    
    ++paramItr;
  }
  
  // keep track if the population has been fully evaluted
  bool populationEvaluated = (population.size() >= populationSize);
  
  maxDiffScale = 0.0;
  // loop over parameter sets
  for(const auto & pSet : population) {
    // for each parameter set:
    //   1. check if it's been evaluated
    //   2. update ranges of each parameter description
    
    if(isNaN(pSet.value))
      populationEvaluated = false;
    else {
      if(pSet.diffScale > maxDiffScale) {
        maxDiffScale = pSet.diffScale;
      }
    }
    
    paramItr = pSet.parameters.cbegin();
    for(auto & paramDescription : parameterDescriptions) {
      if(!paramDescription.isConstant())
        paramDescription.updateRange(*paramItr);
      ++paramItr;
    }
  }
  
  // update the overall maxLogRange and maxUniformRange
  for(auto & paramDescription : parameterDescriptions) {
    if(paramDescription.isLogDistributed()) {
      // parameter is log-distributed, update maxLogRange
      if(paramDescription.range() > maxLogRange)
        maxLogRange = paramDescription.range();
    }
    else {
      // parameter is uniform-distributed, update maxUniformRange
      if(paramDescription.range() > maxUniformRange)
        maxUniformRange = paramDescription.range();
    }
  }
  
  // range of values of competetive parameter sets
  fRange = population.back().value - population.front().value;
  
  // This is the test to see if the function is optimized
  
  if(parameterDescriptions.numFitParameters() == 0) {
    optimal = true;
  }
  else if(populationEvaluated && fRange < fTol && maxUniformRange < tol &&
           (maxLogRange - 1.0) < tol) {
    optimal = true;
  }
  else if(generationNum > maxGenerationNum) {
    optimal = true;
  }
  else {
    optimal = false; // provisional, may change in a bit
    /*
    const size_t lookBack = 1000;
    if(generationNum > lookBack) {
      const double smallTol = fTol * 0.1;
      
      const double deltaValue =
        generationHistory[generationNum - lookBack].bestParameters.value -
        bestParameters.value;
      
      if(generationNum > lookBack && fRange < smallTol && deltaValue < smallTol){
          optimal = true;
      }
    }
    */
  }
}



void
ParameterPopulation::saveHistory(void)
{
  // save current generation's history
  // NOTE: this function may be called more than once per generation, so can't
  // simply call generationHistory.push_back(...)

  // resize generationHistory
  generationHistory.resize(generationNum, 
    GenerationSummary(bestParameters, generationTime, totalTime,
                      fRange, maxLogRange, maxUniformRange) );
}



void
ParameterPopulation::summarizeGeneration(const size_t & genNum) const
{
  // print a summary of the requested generation
  assert(genNum > 0);
  
  // get a pointer to the requested generation summary
  GenerationSummary const* const histPtr = &generationHistory[genNum - 1];
  
  // print out a the details of the summary
  if(logToTerminal) {
    cout << "####### Generation " << genNum << " #######\n";
    cout << "Best value = " << histPtr->bestParameters.value << '\n';
    cout << "Evaluation time:  " << histPtr->generationTime << '\n';
    cout << "Total elapsed time:  " << histPtr->totalTime << '\n';
    if(optimal && genNum == generationNum)
      cout << "Finished optimizing.\n";    
    cout.flush();
  }
}



inline void
ParameterPopulation::get3ParentInds(size_t & ind1, size_t & ind2,
                                    size_t & ind3, RandGen & randGen) const
{
  // Randomly draw indices to three members of the population
  
  const size_t popSize = population.size();
  ind1 = getParent(popSize, randGen);
  ind2 = getParent(popSize - 1, randGen);
  ind3 = getParent(popSize - 2, randGen);
  if(ind2 >= ind1) {
    ++ind2;
    if(ind3 >= ind1)
      ++ind3;
    if(ind3 >= ind2)
      ++ind3;
  }
  else {
    if(ind3 >= ind2)
      ++ind3;
    if(ind3 >= ind1)
      ++ind3;
  }
}



inline void
ParameterPopulation::getNParentInds(vector<size_t> & parentInds,
                                    const size_t & numParents,
                                    RandGen & randGen) const
{
  // Randomly draw indices to numParents members of the population.
  // Ensure that the first parentInd is the best one
  
  // create list of all possible parents
  static vector<size_t> allInds;  // change to not be vector?
  allInds.resize(population.size());
  size_t n = 0;
  for(size_t & ind : allInds) {
    ind = n;
    ++n;
  }
  
  // resize the vector of indices appropriately
  parentInds.resize(numParents);
  
  for(size_t & parentInd : parentInds) {
    // randomly select a valid member of the population
    const size_t randomInd = randGen.unsignedInt(allInds.size());
    auto allItr = allInds.begin() + randomInd;
    // set the current parent to the selected member
    parentInd = *allItr;
    // If it's better (lower index) than the first parentInd, swap them
    if(parentInd < parentInds[0]) {
      swap(parentInd, parentInds[0]);
    }
    // remove this population member from further consideration
    allInds.erase(allItr);
  }
}



inline size_t
ParameterPopulation::getParent(const size_t & cutoff, RandGen & randGen) const
{
  // return a parent index from the breeding population [0, cutoff - 1]
  
  return (size_t)randGen.unsignedInt(cutoff);
}
