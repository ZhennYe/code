#include "../include/ParameterDescription.h"



using namespace std;



//////////////////////////// ParameterDescription /////////////////////////////



ParameterDescription::ParameterDescription(void) : isStateParameter (false)
{
  parameterType = ParameterDescription::Invalid;
}



ParameterDescription::ParameterDescription(const string & paramName,
                                           const double & defaultVal) :
                                           name(paramName),
                                           units (""),
                                           minAllowed(defaultVal),
                                           maxAllowed(defaultVal),
                                           minRange(defaultVal),
                                           maxRange(defaultVal),
                                           isStateParameter(false)

                                           
{
  checkBounds();
}


ParameterDescription::ParameterDescription(const string & paramName,
                                           const string & paramUnits,
                                           const double & defaultVal) :
                                           name(paramName),
                                           units(paramUnits),
                                           minAllowed(defaultVal),
                                           maxAllowed(defaultVal),
                                           minRange(defaultVal),
                                           maxRange(defaultVal),
                                           isStateParameter(false)
                                           
{
  checkBounds();
}


ParameterDescription::ParameterDescription(const string & paramName,
                                           const double & paramMinRange,
                                           const double & paramMaxRange) :
                                           name(paramName),
                                           units (""),
                                           minAllowed(paramMinRange),
                                           maxAllowed(paramMaxRange),
                                           minRange(paramMinRange),
                                           maxRange(paramMaxRange),
                                           isStateParameter(false)

{
  checkBounds();
}


ParameterDescription::ParameterDescription(const string & paramName,
                                           const string & paramUnits,
                                           const double & paramMinRange,
                                           const double & paramMaxRange) :
                                           name(paramName),
                                           units(paramUnits),
                                           minAllowed(paramMinRange),
                                           maxAllowed(paramMaxRange),
                                           minRange(paramMinRange),
                                           maxRange(paramMaxRange),
                                           isStateParameter(false)
{
  checkBounds();
}


ParameterDescription::ParameterDescription(const string & paramName,
                                           const double & paramMinAllowed,
                                           const double & paramMaxAllowed,
                                           const double & paramMinRange,
                                           const double & paramMaxRange) :
                                           name(paramName),
                                           units (""),
                                           minAllowed(paramMinAllowed),
                                           maxAllowed(paramMaxAllowed),
                                           minRange(paramMinRange),
                                           maxRange(paramMaxRange),
                                           isStateParameter(false)
{
  checkBounds();
}


ParameterDescription::ParameterDescription(const string & paramName,
                                           const string & paramUnits,
                                           const double & paramMinAllowed,
                                           const double & paramMaxAllowed,
                                           const double & paramMinRange,
                                           const double & paramMaxRange) :
                                           name(paramName),
                                           units(paramUnits),
                                           minAllowed(paramMinAllowed),
                                           maxAllowed(paramMaxAllowed),
                                           minRange(paramMinRange),
                                           maxRange(paramMaxRange),
                                           isStateParameter(false)
{
  checkBounds();
}



void
ParameterDescription::checkBounds(void)
{
  if(maxAllowed < maxRange ||
     maxRange < minRange ||
     minRange < minAllowed)
    // inconsistant bounds
    parameterType = ParameterDescription::Invalid;
  else if(minRange == maxRange)
    // If the population initial bounds allow for no variation, this is a
    // constant
    parameterType = ParameterDescription::Constant;
  else if(minAllowed * maxAllowed > 0)
    // if the population initial bounds have the same sign, make it
    //  log-distributed
    parameterType = ParameterDescription::LogDistributed;
  else
    parameterType = ParameterDescription::UniformDistributed;
}



const double &
ParameterDescription::defaultValue(void) const
{
  if(isConstant())
    return minAllowed;
  else
    // throw an exception
    throw ParameterDescriptionException("No default value: parameter " + name +
                                        " is not constant.");
}



bool
ParameterDescription::match(const string & variableName, double & var)
{
  if(variableName == name) {
    matches.push_back(&var);
    if(parameterType == ParameterDescription::Constant)
      var = defaultValue();
    return true;
  }
  else
    return false;
}



bool
ParameterDescription::match(const string & variableName, double & var,
                            bool isState)
{
  if(variableName == name) {
    matches.push_back(&var);
    if(parameterType == ParameterDescription::Constant)
      var = defaultValue();
    isStateParameter = isState;
    return true;
  }
  else
    return false;
}

    
    
void
ParameterDescription::set(const double & paramValue) const
{
  for(const auto paramPtr : matches)
    *paramPtr = paramValue;
}



const ParameterDescription::ParameterType &
ParameterDescription::type(void) const
{
  return parameterType;
}



bool
ParameterDescription::isValid(void) const
{
  return (parameterType != ParameterDescription::Invalid);
}


  
bool
ParameterDescription::isConstant(void) const
{
  return (parameterType == ParameterDescription::Constant);
}



bool
ParameterDescription::isLogDistributed(void) const
{
  return (parameterType == ParameterDescription::LogDistributed);
}



bool
ParameterDescription::isUniformDistributed(void) const
{
  return (parameterType == ParameterDescription::UniformDistributed);
}



double
ParameterDescription::range(void) const
{
  switch(parameterType) {
    case ParameterDescription::Constant:
      return 0;
    case ParameterDescription::LogDistributed:
      return maxRange / minRange;
    case ParameterDescription::UniformDistributed:
      return maxRange - minRange;
    case ParameterDescription::Invalid:
    default:
      throw ParameterDescriptionException("Invalid range for parameter " +
                                          name);
  }
}



void
ParameterDescription::updateRange(const double & value)
{
  switch(parameterType) {
    case ParameterDescription::Constant:
      throw ParameterDescriptionException("Can't update range of constant \"" +
                                          name + "\"");
    case ParameterDescription::LogDistributed:
    case ParameterDescription::UniformDistributed:
      if(value > maxRange)
        maxRange = value;
      else if(value < minRange)
        minRange = value;
        break;
    case ParameterDescription::Invalid:
    default:
      throw ParameterDescriptionException("Invalid range for parameter " +
                                          name);
  }
}



//////////////////////// ParameterDescription Friends /////////////////////////
ostream &
operator<<(ostream & out, const ParameterDescription & desc)
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
  const size_t nameWidth = std::min(desc.name.size(), (size_t)14);
  
  // set output decimal precision
  out.precision(doublePrecision);

  // write out the name
  out.width(nameWidth);
  out << left << desc.name << ' ';

  // set precision to 6
  out.precision(doublePrecision);
  if(desc.isConstant()) {
    out.width(doubleWidth);
    out << right << desc.defaultValue();
  }
  else if(desc.minAllowed == desc.minRange &&
          desc.maxAllowed == desc.maxRange) {
    out.width(doubleWidth);
    out << right << desc.minRange;
    out.width(doubleWidth);
    out << right << desc.maxRange;
  }
  else {
    out.width(doubleWidth);
    out << right << desc.minAllowed;
    out.width(doubleWidth);
    out << right << desc.maxAllowed;
    out.width(doubleWidth);
    out << right << desc.minRange;
    out.width(doubleWidth);
    out << right << desc.maxRange;
  }
  
  // parameter description must end with endl
  out << '\n';
  
  // restore flags
  out.setf(formatFlags);
  // restore width
  out.width(width);
  // restore precision
  out.precision(precision);
  
  return out;
}



istream &
operator>>(istream & in, ParameterDescription & desc)
{
  string line;
  SplitLine splitLine;
  while(splitLine.empty()) {
    if(in.bad())
      throw ParameterDescriptionException(
        "Error reading ParameterDescription from stream");
    
    getline(in, line);
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
  
  switch(splitLine.size()) {
    case 2:  // name constant
      desc.name = splitLine[0];
      desc.minAllowed = desc.maxAllowed = desc.minRange = desc.maxRange =
        stringToDouble(splitLine[1]);
      break;
      
    case 3:  // name minRange maxRange
      desc.name = splitLine[0];
      desc.minAllowed = desc.minRange = stringToDouble(splitLine[1]);
      desc.maxAllowed = desc.maxRange = stringToDouble(splitLine[2]);
      break;
    
    case 5:  // name minAllowed maxAllowed minRange maxRange
      desc.name = splitLine[0];
      desc.minAllowed = stringToDouble(splitLine[1]);
      desc.maxAllowed = stringToDouble(splitLine[2]);
      desc.minRange = stringToDouble(splitLine[3]);
      desc.maxRange = stringToDouble(splitLine[4]);
      break;
      
    default: // error
      cout << "splitLine.size() = " << splitLine.size() << '\n';
      for(size_t dumb = 0; dumb < splitLine.size(); ++dumb)
        cout << splitLine[dumb] << '\n';
      cout.flush();

      throw ParameterDescriptionException(
        "Bad line reading ParameterDescription: " + line);
  }
  desc.checkBounds();
  
  return in;
}




////////////////////////// ParameterDescriptionList ///////////////////////////



bool
ParameterDescriptionList::match(const string & variableName, double & var)
{
  // check if the variable name matches any parameter description.
  // If it does:
  //    add variable pointer to list to be updated with variable value,
  //    and return true
  // otherwise, return false
  for(auto & description : parameterDescriptions)
    if(description.match(variableName, var))
      return true;
  
  return false;
}



bool
ParameterDescriptionList::match(const string & variableName, double & var,
                                bool isState)
{
  // check if the variable name matches any parameter description.
  // If it does:
  //    add variable pointer to list to be updated with variable value,
  //    set isStateParameter
  //    and return true
  // otherwise, return false
  for(auto & description : parameterDescriptions)
    if(description.match(variableName, var, isState))
      return true;
  
  return false;
}



void
ParameterDescriptionList::add(ParameterDescription newParam)
{
  // check to see if this is really a new parameter
  for(const auto & description : parameterDescriptions)
    if(description.name == newParam.name)
      throw ParameterDescriptionException("Tried to add duplicate parameter: "+
                                          newParam.name);
  // add it
  parameterDescriptions.push_back(newParam);
}



void
ParameterDescriptionList::set(const vector<double> & parameters) const
{
  auto paramItr = parameters.cbegin();
  for(const auto & description : parameterDescriptions) {
    description.set(*paramItr);
    ++paramItr;
  }
}



size_t
ParameterDescriptionList::numFitParameters(void) const
{
  size_t numFit = 0;
  for(const auto & description : parameterDescriptions)
    switch(description.type()) {
      case ParameterDescription::Constant:
        break;
      case ParameterDescription::LogDistributed:
        ++numFit;
        break;
      case ParameterDescription::UniformDistributed:
        ++numFit;
        break;
      case ParameterDescription::Invalid:
      default:
        throw ParameterDescriptionException("Invalid range for parameter " +
                                            description.name);
    }
  return numFit;
}



size_t
ParameterDescriptionList::numFitRegularParameters(void) const
{
  size_t numFit = 0;
  for(const auto & description : parameterDescriptions) {
    if(description.isStateParameter) {
      // only count up regular parameters
      continue;
    }
    switch(description.type()) {
      case ParameterDescription::Constant:
        break;
      case ParameterDescription::LogDistributed:
        ++numFit;
        break;
      case ParameterDescription::UniformDistributed:
        ++numFit;
        break;
      case ParameterDescription::Invalid:
      default:
        throw ParameterDescriptionException("Invalid range for parameter " +
                                            description.name);
    }
  }
  return numFit;
}



size_t
ParameterDescriptionList::numFitStateParameters(void) const
{
  size_t numFit = 0;
  for(const auto & description : parameterDescriptions) {
    if(!description.isStateParameter) {
      // only count up state parameters
      continue;
    }
    switch(description.type()) {
      case ParameterDescription::Constant:
        break;
      case ParameterDescription::LogDistributed:
        ++numFit;
        break;
      case ParameterDescription::UniformDistributed:
        ++numFit;
        break;
      case ParameterDescription::Invalid:
      default:
        throw ParameterDescriptionException("Invalid range for parameter " +
                                            description.name);
    }
  }
  return numFit;
}



size_t
ParameterDescriptionList::numMatches(const string & paramName) const
{
  for(const auto & description : parameterDescriptions)
    if(description.name == paramName)
      return description.numMatches();
  
  return 0;
}



double
ParameterDescriptionList::getMaxLogRange(void) const
{
  double maxLogRange = 0.0;
  for(const auto & description : parameterDescriptions)
    if(description.type() == ParameterDescription::LogDistributed)
      maxLogRange = std::max(maxLogRange,
                             description.maxAllowed / description.minAllowed);
  return maxLogRange;
}



double
ParameterDescriptionList::getMaxUniformRange(void) const
{
  double maxUniformRange = 0.0;
  for(const auto & description : parameterDescriptions)
    if(description.type() == ParameterDescription::UniformDistributed)
      maxUniformRange = std::max(maxUniformRange,
                              description.maxAllowed - description.minAllowed);
  return maxUniformRange;
}



void
ParameterDescriptionList::push_back(ParameterDescription newParam)
{
  // check to see if this is really a new parameter
  for(const auto & description : parameterDescriptions)
    if(description.name == newParam.name)
      throw ParameterDescriptionException("Tried to add duplicate parameter: "+
                                          newParam.name);
  
  // add it
  parameterDescriptions.push_back(newParam);
}



////////////////////// ParameterDescriptionList Friends ///////////////////////
ostream &
operator<<(ostream & out, const ParameterDescriptionList & pdList)
{
  // record format flags
  const ios_base::fmtflags formatFlags = out.flags();

  out << left << pdList.size() << " # parameter descriptions\n";

  // output each parameter description  
  for(const auto & description : pdList)
    out << description;

  // parameter description list must end with endline
  out << '\n';

  // restore flags
  out.setf(formatFlags);
  
  return out;
}



istream &
operator>>(istream & in, ParameterDescriptionList & pdList)
{
  // get a non-empty, non-comment line and read in the number of descriptions
  string line, rawLine;
  SplitLine splitLine;
  while(splitLine.empty()) {
    if(in.bad())
      throw ParameterDescriptionException(
        "Error reading ParameterDescriptionList from stream");
    
    getline(in, rawLine);
    line = rawLine;
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
  
  // should be just one element in line (the number of parameter descriptions)
  if(splitLine.size() != 1)
    throw ParameterDescriptionException(
                            "Bad line reading ParameterDescriptionList: "
                            + rawLine);

  // convert number of parameter descriptions to size_t
  size_t numDescriptions = stringToSize(splitLine[0]);
  
  // read in each description and add to list
  for(; numDescriptions > 0; --numDescriptions) {
    ParameterDescription param;
    in >> param;
    pdList.add(param);
  }
 
  return in;
}
