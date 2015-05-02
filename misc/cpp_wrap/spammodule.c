// sample module for extending python
// needed to run with 
// gcc -I/usr/include/python2.7 -lpython2.7 /home/alex/code/misc/cpp_wrap/spammodule.c 
// just to get anything to work at all

#include <Python.h>


static PyObject *
spam_system(PyObject *self, PyObject *args)
{
  const char *command;
  int sts;
  
  if (!PyArg_ParseTuple(args, "s", &command))
    return NULL;
  sts = system(command);
  return PyLong_FromLong(sts);
}

