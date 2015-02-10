#ifndef _PARAMETER_DESCRIPTION_H_
#define _PARAMETER_DESCRIPTION_H_



#include "io_functions.h"

#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <limits>
#include <cmath>



class ParameterDescription
 {
  public:
    ParameterDescription(void);
    ParameterDescription(const std::string & name,
                         const double & defaultVal);
    ParameterDescription(const std::string & name, const std::string & units,
                         const double & defaultVal);
    ParameterDescription(const std::string & name,
                         const double & minRange,
                         const double & maxRange);
    ParameterDescription(const std::string & name, const std::string & units, 
                         const double & minRange,
                         const double & maxRange);
    ParameterDescription(const std::string & name,
                         const double & minAllowed,
                         const double & maxAllowed,
                         const double & minRange,
                         const double & maxRange);
    ParameterDescription(const std::string & name, const std::string & units,
                         const double & minAllowed,
                         const double & maxAllowed,
                         const double & minRange,
                         const double & maxRange);
                         
    // set the value of parameterType based on range and allowed values
    void checkBounds(void);    
    // return the default Value of this ParameterDescription
    const double & defaultValue(void) const;
    // match a variable to a this parameter by name
    //   if matched:
    //     add pointer-to-variable to matches
    //     if the parameter is constant, set var = defaultValue()
    //     return true
    //   otherwise return false
    bool match(const std::string & variableName, double & var);
    // match a variable to a this parameter by name, and set isStateParameter
    bool match(const std::string & variableName, double & var, bool isState);
    // set all the matching variables to the value paramValue
    void set(const double & paramValue) const;
    // return the number of matches
    inline size_t numMatches(void) const {
      return matches.size();
    }

    // define an enum describing the preferred initial distribution of this
    // parameter in a randomly-generated initial population
    enum ParameterType {Invalid, Constant, LogDistributed, UniformDistributed};
    // preturn parameterType
    const ParameterType & type(void) const;
    
    // return boolean: valid/constant/logDistributed/uniformDistributed:
    bool isValid(void) const;
    bool isConstant(void) const;
    bool isLogDistributed(void) const;
    bool isUniformDistributed(void) const;
    
    // return the range of the parameter (max - min for uniform, max/min for
    // log-distributed)
    double range(void) const;
    // update minRange or maxRange based on a parameter value
    void updateRange(const double & value);
    
    // friend operator, write ParameterDescription to stream
    friend std::ostream & operator<<(std::ostream & out,
                                     const ParameterDescription & desc);
    // friend operator, read ParameterDescription from stream
    friend std::istream & operator>>(std::istream & in,
                                     ParameterDescription & desc);
    
    std::string name;
    std::string units;
    double minAllowed;
    double maxAllowed;
    double minRange;
    double maxRange;
    bool isStateParameter;
    
  private:
    ParameterType parameterType;
    std::vector<double*> matches;
};



class ParameterDescriptionList
{
  public:
    // member functions unique to parameter-like behavior:
    //  match a variable to a parameter by name, returning true if successful
    bool match(const std::string & variableName, double & var);
    // match, and set isStateParameter if successful
    bool match(const std::string & variableName, double & var, bool isState);
    //  add a new parameter description to the list
    void add(ParameterDescription newParam);
    //  set variables to values specified by a vector of parameters
    void set(const std::vector<double> & parameters) const;
    //  return the number of parameters that aren't const
    size_t numFitParameters(void) const;
    //  return the number of regular parameters that aren't const
    size_t numFitRegularParameters(void) const;
    //  return the number of state parameters that aren't const
    size_t numFitStateParameters(void) const;
    // return the number of matches for a given parameter name
    size_t numMatches(const std::string & paramName) const;
    // return the maximum allowed range of log-distributed parameters
    double getMaxLogRange(void) const;
    // return the maximum allowed range of uniform-distributed parameters
    double getMaxUniformRange(void) const;
    
    // typedef iterators
    typedef std::vector<ParameterDescription>::iterator iterator;
    typedef std::vector<ParameterDescription>::const_iterator const_iterator;
    
    // vector-like interface
    size_t size(void) const {
      return parameterDescriptions.size();
    }
    bool empty(void) const {
      return parameterDescriptions.empty();
    }
    ParameterDescription & front(void) {
      return parameterDescriptions.front();
    }
    const ParameterDescription & front(void) const {
      return parameterDescriptions.front();
    }
    ParameterDescription & back(void) {
      return parameterDescriptions.back();
    }
    const ParameterDescription & back(void) const {
      return parameterDescriptions.back();
    }
    void push_back(ParameterDescription newParam);
    inline ParameterDescriptionList::iterator begin(void) {
      return parameterDescriptions.begin();
    }
    inline ParameterDescriptionList::const_iterator begin(void) const {
      return parameterDescriptions.begin();
    }
    inline ParameterDescriptionList::const_iterator cbegin(void) const {
      return parameterDescriptions.begin();
    }
    inline ParameterDescriptionList::iterator end(void) {
      return parameterDescriptions.end();
    }
    inline ParameterDescriptionList::const_iterator end(void) const {
      return parameterDescriptions.end();
    }
    inline ParameterDescriptionList::const_iterator cend(void) const {
      return parameterDescriptions.end();
    }
    ParameterDescription & operator[](const size_t & ind) {
      return parameterDescriptions[ind];
    }
    const ParameterDescription & operator[](const size_t & ind) const {
      return parameterDescriptions[ind];
    }

    // friend operator, write ParameterDescriptionList to stream
    friend std::ostream & operator<<(std::ostream & out,
                                     const ParameterDescriptionList & pdList);
    // friend operator, read ParameterDescriptionList from stream
    friend std::istream & operator>>(std::istream & in,
                                     ParameterDescriptionList & pdList);

  protected:
    std::vector<ParameterDescription> parameterDescriptions;
};


#ifndef ParameterException_Struct
  #define ParameterException_Struct
  struct ParameterException : std::runtime_error
  {
    explicit ParameterException() :
      std::runtime_error("ParameterPopulationException") {}
    explicit ParameterException(const std::string & str) :
      std::runtime_error(str) {}
  };
#endif

struct ParameterDescriptionException : ParameterException
{
  explicit ParameterDescriptionException() :
    ParameterException("ParameterDescriptionException") { }
  explicit ParameterDescriptionException(const std::string & str) :
    ParameterException(str) { }
};



#endif
