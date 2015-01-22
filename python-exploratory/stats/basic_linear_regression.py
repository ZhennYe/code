# basic_linear_regression.py - given x and y find the regression
#     statistics and the 95% confidence intervals (CI)

import math
import numpy as np


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
  # Covariance sum of squares
  xy_sum = sum([x[i]*y[i] for i in range(len(x))])
  xy_SS = xy_sum - sum(x) * sum(y) / len(x)
  return xy_SS


def SS(x):
  # sum of squares
  # x_SS = sum(map(lambda a: a * a, x))
  x_SS = sum(map(lambda a: a * a, x)) - ((sum(x)**2.)/len(x))
  return x_SS


def basic_linear_regression(x, y, replicates=False):
  if replicates == False:
    # sum(x^2) and sum(xy)
    x_SS = sum(map(lambda a: a * a, x))
    covariance_SS = sum([x[i] * y[i] for i in range(len(x))])
    
    b = (covariance_SS - (sum(x) * sum(y)) / len(x)) / (x_SS - \
                                          ((sum(x)**2) / len(x)))
    a = (sum(y) - b*sum(x))/len(x)
  
  # replicates == True
  else:
    x_sum, y_sum = sum(map(lambda a: a * a, x)), sum(map(lambda a: a*a,y))
    x_mean, y_mean = np.mean(x), np.mean(y)
    x_SS = x_sum - sum(x)**2/len(x)
    xy_sum = sum([x[i]*y[i] for i in range(len(x))])
    xy_SS = xy_sum - sum(x) * sum(y) / len(x)
    b = float(xy_SS) / x_SS
    a = y_mean - b*x_mean
  
  return a, b


def simple_corrcoef(x,y):
  """ 
  correlation coefficient, a.k.a. Pearson product-moment correlation
  coefficient 
  """
  xy_SS = cov_SS(x,y)
  x_SS, y_SS = SS(x), SS(y)
  r = xy_SS / math.sqrt(x_SS * y_SS)
  
  return r


def studentsT(x, y, stats):
  # find student's t statistic for power = 0.05, null H0 = 0 (no correlation)
  n = len(x)
  a, b = basic_linear_regression(x, y)
  # s_b = sqrt(s2_YX ('MS') / x_SS)
  print('MS: %.3f, x_SS: %.3f' %(stats['MS'], stats['x_SS']))
  s_b = math.sqrt( stats['MS'] / stats['x_SS'] )
  # t = (b - 0)/s_b
  print('b: %.3f, s_b: %.3f' %(b, s_b))
  t = (b - 0)/s_b
  stats['t'] = t
  
  print('Students t statistic: %.3f' %t)
  return t, stats


def findCIs(s2_YX, x, x_SS, x_value):
  # find the +/- confidence interval for a given x_value of interest
  n = len(x)
  x_mean = np.mean(x)
  
  s_yhat = math.sqrt( s2_YX * ((1./n) + ((x_value - x_mean)**2./x_SS)) )
  return s_yhat
  

def regression_with_CI(x, y, replicates):
  a, b = basic_linear_regression(x,y, replicates)
  
  # find regression and total sum of squares (SS)
  x_SS = sum(map(lambda a: a * a, x)) - ((sum(x)**2.)/len(x))
  yhat = [(a+x[i]*b) for i in range(len(x))]
  y_SS = sum(map(lambda a: a * a, y)) - ((sum(y)**2.)/len(y))
  covariance_SS = sum([x[i] * y[i] for i in range(len(x))]) - \
                  ((sum(x)*sum(y))/len(x))
  reg_SS = float(covariance_SS)**2./x_SS
  
  # find relevant statistics
  r_squared = float(reg_SS)/y_SS
  residual = float(y_SS) - reg_SS             # residual
  MS = s2_YX = residual/(len(x)-2)     # mean square
  s_YX = math.sqrt(MS)                 # standard error
  stats = {'r_squared': r_squared, 'residual': residual, 'MS': MS,
           's_YX': s_YX, 'x_SS': x_SS, 'y_SS': y_SS, 'reg_SS': reg_SS,
           'covariance_SS': covariance_SS, 'a': a, 'b': b}
  
  # find confidence intervals
  CI = [findCIs(MS, x, x_SS, x[i]) for i in range(len(x))]
  upper = [yhat[i] + abs(CI[i]) for i in range(len(yhat))]
  lower = [yhat[i] - abs(CI[i]) for i in range(len(yhat))]
  
  return yhat, upper, lower, stats
  

def predictSamples(m, x_val, x, y):
  """
  if you wanted to know the mean and CI of m samples taken at a
  value x_val
  """
  n = len(x)
  x_mean = np.mean(x)
  yhat, upper, lower, stats = regression_with_CI(x, y)
  # mean at x_val:
  y_val = stats['a'] + stats['b'] * x_val
  # standard error of measurement at x_val for m samples:
  s_m = math.sqrt( stats['MS']*(1./m + 1./n + (x_val - x_mean)**2 / \
                               stats['x_SS']) )
  t, stats = studentsT(x, y, stats)
  critval = returnCritValue(n-2)
  print('Mean for %i samples at %.3f: %.3f +/- %.3f' 
        %(m, x_val, y_val, critval*s_m))
  return


def plotStuff(x ,y, replicates=False):
  # make some plots, I dunno
  y_hat, upper, lower, stats = regression_with_CI(x, y, replicates)
  import pylab as py
  fig1 = py.figure()
  ax1 = fig1.add_subplot(111)
  # raw data
  if replicates==False:
    ax1.plot(x, y, 'k', linewidth=3, alpha=0.5)
  else:
    ax1.plot(x, y, 'ko', alpha=0.5)
  # regressed data
  ax1.plot(x,y_hat, 'b', linewidth=2, alpha=1)
  ax1.plot(x,upper, 'b', linewidth=5, alpha=0.2)
  ax1.plot(x,lower, 'b', linewidth=5, alpha=0.2)
  # py.legend( (y, y_hat), ('Raw Data','Regressed') )
  # print('r^2: %.5f ' % stats['r_squared'])
  print(stats)
  py.show()
  
  return


######################## control ########################

def replicateSample():
  print('Running with replicate sample data from _Biostatisical Analysis_ by Zar')
  X=[30,30,30,40,40,40,40,50,50,50,60,60,60,60,60,70,70,70,70,70]
  Y=[108,110,106,
     125,120,118,119,
     132,137,134,
     148,151,146,147,144,
     162,156,164,158,159]
  a, b = basic_linear_regression(X, Y, replicates=True)
  plotStuff(X,Y,replicates=True)


def runSample():
  print('Running with sample data from _Biostatistical Analysis_ by Zar')
  x = [3,4,5,6,8,9,10,11,12,14,15,16,17]
  y = [1.4,1.5,2.2,2.4,3.1,3.2,3.2,3.9,4.1,4.7,4.5,5.2,5.0]
  plotStuff(x, y)


if __name__ == '__main__':
  replicateSample()
  



