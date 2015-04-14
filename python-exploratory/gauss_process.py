import numpy as np
import matplotlib.pyplot as plt
from sklearn.gaussian_process import GaussianProcess

np.random.seed(1)


def f(x):
  # this is the target function
  return x * np.sin(x)


### noiseless case
def noiseless():
  X = np.atleast_2d([1.,3.,5.,6.,7.,8.,]).T
  # observations
  y = f(X).ravel() # reshape to 1-D
  # mesh the input space to cover all points to predict f(x) and the MSE
  x = np.atleast_2d(np.linspace(0,10,1000)).T # and flatten
  # instantiate gauss process
  gp = GaussianProcess(corr='cubic', theta0=1e-2, thetaL=1e-4,
                       thetaU=1e-1, random_start=100)
  # fit to data using maximum likelihood estimation of params
  gp.fit(X,y)
  # make predictions on meshed x-axis, also return MSE
  y_pred, MSE = gp.predict(x, eval_MSE=True)
  sigma = np.sqrt(MSE)
  
  # plot
  fig = plt.figure()
  plt.plot(x, f(x), 'r:', label=u'$f(x) = x\,\sin(x)$')
  plt.plot(X, y, 'r.', markersize=10, label=u'Observations')
  plt.plot(x, y_pred, 'b-', label=u'Prediction')
  # fill the space between the +/-MSE
  plt.fill(np.concatenate([x, x[::-1]]), # reverse order of x
           np.concatenate([y_pred - 1.9600 * sigma,
                          (y_pred + 1.9600 * sigma)[::-1]]),
           alpha=0.5, fc='b', ec='None', label='95% confidence interval')
           # shade, fill color, edge color
  plt.title('Noiseless case')
  plt.xlabel('$x$')
  plt.ylabel('$f(x)$')
  plt.ylim(-10, 20)
  plt.legend(loc='upper left')
  return



### noisy case
def noisy():
  X = np.linspace(0.1,9.9, 20)
  X = np.atleast_2d(X).T
  # observations and noise
  y = f(X).ravel()
  dy = 0.5 + 1.0 * np.random.random(y.shape)
  noise = np.random.normal(0, dy)
  y += noise
  # mesh the input space for evaluation of the function & its prediction and MSE
  x = np.atleast_2d(np.linspace(0,10,1000)).T
  # instantiate a gauss process
  gp = GaussianProcess(corr='squared_exponential', theta0=1e-1,
                       thetaL=1e-3, thetaU=1, nugget= (dy/y)**2,
                       random_start=100)
                       # nugget specifies std of noise, Tikhonov regularization
                       # allows robust recovery of underlying function
                       # from noisy data
  # fit to GP using maximum likelihood estimation of params
  gp.fit(X,y)
  # make predictions on meshed x-axis
  y_pred, MSE = gp.predict(x, eval_MSE=True)
  sigma = np.sqrt(MSE)
  
  # plot
  fig = plt.figure()
  plt.plot(x, f(x), 'r:', label=u'$f(x) = x\,\sin(x)$')
  plt.errorbar(X.ravel(), y, dy, fmt='r.', markersize=10, 
               label=u'Observations')
  plt.plot(x, y_pred, 'b-', label=u'Prediction')
  plt.fill(np.concatenate([x, x[::-1]]), # reverse order of x
           np.concatenate([y_pred - 1.9600 * sigma,
                          (y_pred + 1.9600 * sigma)[::-1]]),
           alpha=0.5, fc='b', ec='None', label='95% confidence interval')
           # shade, fill color, edge color
  plt.title('Noisy case')
  plt.xlabel('$x$')
  plt.ylabel('$f(x)$')
  plt.ylim(-10, 20)
  plt.legend(loc='upper left')
  return



def demo():
  noiseless()
  noisy()
  plt.show()







