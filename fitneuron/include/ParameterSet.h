#ifndef ParameterSet_h
#define ParameterSet_h


#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>


// sortable class that organizes parameter information
struct ParameterSet
{
  // read a set of parameters from specified file
  void read(const std::string & fileName, const size_t & numParams);
  // read a set of parameters from the stream in
  void read(std::istream & in, const size_t & numParams);
  // define an ordering on parameter sets (based on this->value)
  inline bool operator<(const ParameterSet & compareParam) const;
  // define an operator to return the nth parameter:
  inline double & operator[](const size_t & index) {
    return parameters[index];
  }
  inline const double & operator[](const size_t & index) const {
    return parameters[index];
  }
  // swap two parameter sets
  inline void swap(ParameterSet & swapSet) {
    double temp = value;
    value = swapSet.value;
    swapSet.value = temp;
    
    temp = diffScale;
    diffScale = swapSet.diffScale;
    swapSet.diffScale = temp;
    parameters.swap(swapSet.parameters);
  }
  // friend operator, write ParameterSet to stream
  friend std::ostream & operator<<(std::ostream & out,
                                   const ParameterSet & parameterSet);
  // friend operator, read ParameterSet from stream
  //   assumes either:
  //    -ParameterSet has already been resized OR
  //    -ParameterSet is alone on a line of input
  friend std::istream & operator>>(std::istream & in,
                                   ParameterSet & parameterSet);

  std::vector<double> parameters;  // array that stores the parameters
  double value;              // the "value" of the parameters, when evaluated
  double diffScale;
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

struct ParameterSetException : ParameterException
{
  explicit ParameterSetException() :
    ParameterException("ParameterDescriptionException") { }
  explicit ParameterSetException(const std::string & str) :
    ParameterException(str) { }
};

#endif
