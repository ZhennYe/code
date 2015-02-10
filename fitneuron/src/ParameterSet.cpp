#include "../include/ParameterSet.h"
#include "../include/io_functions.h"
#include "../include/constants.h"

#include <limits>
#include <algorithm>


using namespace std;


/////////////////////// ParameterSet member functions ////////////////////////

void
ParameterSet::read(const string & fileName, const size_t & numParams)
{
  // read a set of parameters from specified file

  // open the parameters file
  std::ifstream fIn (fileName.c_str());
  
  try {
    // read parameters from file
    this->read(fIn, numParams);
  }
  catch(exception & e) {
    // caught an exception, close fIn, add some detail and rethrow exception
    fIn.close();
    const string errorStr = "Error reading base parameters file: " +
                            fileName + "\n\t" + e.what();
    throw IOException(errorStr);
  }
  
  // close file
  fIn.close();
}



void
ParameterSet::read(istream & in, const size_t & numParams)
{
  // read a set of parameters from the stream in
  size_t lineNum = 0;
  value = NaN;
  parameters.resize(numParams);
  vector<double>::iterator paramItr = parameters.begin();
  while(in.good()) {
    // strip comments and get the next non-empty line
    const string line = getNextLine(in, lineNum, '#');
    if(line.empty())
      break;
    
    // split the line up into words
    SplitLine splitLine = split(line);
    if(splitLine.size() != 2) {
      if(splitLine.size() == numParams) {
        for(size_t n = 0; n < numParams; ++n) {
          parameters[n] = stringToDouble(splitLine[n]);
        }
        return;
      }
      throw IOException("Error in line" + numToString(lineNum) +
                        ": Line not of form \"paramName paramValue\"");
    }
    // convert the paramName (first word) to lower case
    string paramName = splitLine[0];
    transform(paramName.begin(), paramName.end(), paramName.begin(),::tolower);
    
    if(paramName == "value")
      value = stringToDouble(splitLine[1]);
    else {
      if(paramItr == parameters.end())
        throw IOException("Error in line" + numToString(lineNum) +
                          ": Too many parameters");
      *paramItr = stringToDouble(splitLine[1]);
      ++paramItr;
    }
  }
  
  if(paramItr != parameters.end()) {
    size_t numAssigned = (size_t)(paramItr - parameters.begin());
    throw IOException("Not all parameters assigned (" +
                      numToString(numAssigned) + "/" + numToString(numParams) +
                      ")");
  }
}



bool
ParameterSet::operator<(const ParameterSet & compareParam) const
{
  return (value < compareParam.value);
}



/////////////////////// ParameterSet friend functions ////////////////////////



ostream &
operator<<(ostream & out, const ParameterSet & parameterSet)
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
  
  // set to default-style output
  out.unsetf(ios_base::floatfield);
  
  // set output decimal precision
  out.precision(doublePrecision);
  
  // output the ParameterSet value
  out.width(doubleWidth);
  out << left << parameterSet.value;
  
  // output the parameters
  for(const double & param : parameterSet.parameters) {
    out.width(doubleWidth);
    out << right << param;
  }
  
  // parameter set must end with endline
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
operator>>(istream & in, ParameterSet & parameterSet)
{
  vector<double>::iterator paramItr;
  vector<double>::const_iterator end;
  if(parameterSet.parameters.empty()) {
    // get the next line of input
    string line;
    getline(in, line);
    // split it up into words (each is a double)
    SplitLine splitLine = split(line);
    // first word is the ParameterSet's value
    parameterSet.value = stringToDouble(splitLine[0]);
    // resize the parameters
    parameterSet.parameters.resize(splitLine.size() - 1);
    // read in the rest of the words as parameters
    paramItr = parameterSet.parameters.begin();
    end = parameterSet.parameters.end();
    for(size_t n = 1; paramItr != end; ++paramItr, ++n) {
      *paramItr = stringToDouble(splitLine[n]);
      if(isNaN(*paramItr))
        throw ParameterSetException("NaN parameter value");
    }
  }
  else {
    string word;
    parameterSet.value = getDouble(in);
    paramItr = parameterSet.parameters.begin();
    end = parameterSet.parameters.end();
    for(; paramItr != end; ++paramItr) {
      *paramItr = getDouble(in);
      if(isNaN(*paramItr))
        throw ParameterSetException("NaN parameter value");
    }
  }
  
  return in;
}



