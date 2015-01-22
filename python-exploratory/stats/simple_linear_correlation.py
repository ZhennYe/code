# simple_linear_correlation.py

import numpy as np
import math


def returnCritValue(df, alpha=0.05):
  ns = range(1, 50)
  c = [12.706, 4.303, 3.182, 2.776, 2.571, 
       2.447, 2.365, 2.306, 2.262, 2.228,
       2.201, 2.179, 2.160, 2.145, 2.131, 
       2.120, 2.110, 2.101, 2.093, 2.086,
       2.080, 2.074, 2.069, 2.064, 2.060,
       2.056, 2.052, 2.048, 2.045, 2.042,
       2.040, 2.037, 2.035, 2.032, 2.030,
       2.028, 2.026, 2.024, 2.023, 2.021,
       2.020, 2.018,2.017, 2.015, 2.014,
       2.013, 2.012, 2.011, 2.010, 2.009]
  if df > 50 or df < 0:
    critval = 1.98
  else:
    critval = c[df]
  
  return critval


def cov_SS(x,y):
  """
  Covariance sum of squares
  """
  xy_sum = sum([x[i]*y[i] for i in xrange(len(x))])
  xy_SS = xy_sum - sum(x) * sum(y) / len(x)
  return xy_SS


def SS(x):
  """
  sum of squares
  x_SS = sum(map(lambda a: a * a, x))
  """
  x_SS = sum(map(lambda a: a * a, x)) - ((sum(x)**2.)/len(x))
  return x_SS


def simple_corrcoef(x,y):
  """ 
  correlation coefficient, a.k.a. Pearson product-moment correlation
  coefficient 
  """
  xy_SS = cov_SS(x,y)
  x_SS, y_SS = SS(x), SS(y)
  r = xy_SS / math.sqrt(x_SS * y_SS)
  if r > 1.:
    r = 1
  print('r is %.3f' %r)
  return r


def r_std_err(x,y):
  """
  standard error of correlation coefficient
  """
  r = simple_corrcoef(x,y)
  # print(1-r**2)
  s_r = math.sqrt( (1 - r**2)/
                              (len(x)-2) )
  return s_r


def studentsT(x,y):
  """
  Student's t statistic (also returns two-tailed F value)
  """
  s_r = r_std_err(x,y)
  r = simple_corrcoef(x,y)
  t = r/s_r
  F = (1+abs(r))/(1-abs(r))
  return t, F


def test_correlation(x,y):
  """
  Test whether a correlation is significant with student's t statistic.
  """
  t, _ = studentsT(x,y)
  critval = returnCritValue(len(x))
  significant = (abs(t) >= critval)
  print('Y is significantly correlated with X? %s (t = %.3f vs %.3f)' \
        %(significant, t, critval))
  return #significant



