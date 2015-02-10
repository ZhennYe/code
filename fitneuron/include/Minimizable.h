#ifndef _MINIMIZABLE_H_
#define _MINIMIZABLE_H_



#include "ParameterDescription.h"



// define a base class that can be minimized
class Minimizable
{
  public:
    virtual ~Minimizable() {}
    virtual void initialize(void) {}
    inline const ParameterDescriptionList & getParameterDescriptions(void)
      const { return parameterDescriptionList; }
  
  protected:
    ParameterDescriptionList parameterDescriptionList;
};

#endif
