# sample_fit.py -- run a sample polyfit based on sum of squares
#                  without over-fitting; max 10 exponents
# usage: python sample_fit.py -options

import pylab as py
import numpy as np
import math


def createData(eq='exp'):
  """
  Creates 10,000 sample datapoints with randomly generated 
  coefficients. eq options are 'exp' and 'sin'
  """


p, residuals, rank, singular_values, rcond = py.polyfit(x,y0,5,full=True)
