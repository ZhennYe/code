# check_distribution.py -- determines the state of a distribution
#   and whether it needs to be transformed

import numpy as np
import math
  

def std(x):
  """
  Find standard deviation (and variance) of a distribution X. 
  Greek little sigma is std. std^2 is variance (sigma^2).
  """
  x2_sum = sum(map(lambda a: a * a, x))
  x_sum = sum(x)
  n = len(x)
  var = (x2_sum - (x_sum**2/n))/(n-1)
  x_std = math.sqrt(var)
  return x_std


def expected_height(x,y,i):
  """
  Given a presumed normal distribution, this finds the expected height/
  value of Y(i) at a given X(i); i is an element of X.
  """
  y_std = std(y)
  x_mean = np.mean(x)
  Yi = 1/(y_std*math.sqrt(2*math.pi)) * \
          math.exp(-(i-x_mean)**2/2*y_std**2)
  return Yi
  

def skew(x,y):
  """
  Examines the skew / symmetry of a distribution. Defined by 
  kth moment about the mean.
  """
  def nth_sum(x,n):
    # get the nth sum of a distribution
    n_sum = sum(map(lambda a: (a - np.mean(x))**n, x))
    return n_sum
  n = len(y)
  y_var = std(y)**2
  sum2, sum3, sum4 = nth_sum(y,2), nth_sum(y,3), nth_sum(y,4)
  k3 = (len(y) * sum3)/((n-1)*(n-2))
  g1 = k3/y_var**3
  if g1 < -0.5:
    print('Dist. is skewed to left (g1 = %.3f)' %g1)
  elif g1 > 0.5:
    print('Dist. is skewed to right (g1 = %.3f)' %g1)
  else:
    print('Dist. is relatively normal (g1 = %.3f)' %g1)
  
  # kurtosis - peaked-ness/tailed-ness, dispersion around mean +/- 1 std
  kurt = ( sum4 * n * (n+1) * (n-1) - 3*(sum2)**2 ) / \
         ((n-2)*(n-3))
  g2 = kurt/y_var**4
  if g2 < -0.5:
    print('Distribution is platykurtic (has more values than expected \n\
          within 1 std): g2 = %.3f' %g2)
  elif g2 > 0.5:
    print('Distribution is leptokurtic (has fewer values than expected \n\
          within 1 std): g2 = %.3f' %g2)
  else:
    print('Distribution is mesokurtic (has expected number of values \n\
          within 1 std): g2 = %.3f' %g2)
  return k3, kurt, g1, g2



