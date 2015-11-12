# probabilistic_model.py
# Try to assign points from two unknown streams to the correct stream


import numpy as np
import matplotlib.pyplot as plt
import scipy.linalg as lin


def gen_data(N=100, cols=2, show=False):
  # Generate some random data
  x = np.random.rand(N,cols)
  x[:,1] = 1
  w = np.random.rand(cols,cols)
  #
  y = np.zeros(N)
  n = int(np.random.rand()*N) # Random index
  # All y before n are crossed with w[0,:]; all after are crossed with w[1,:]
  y[:n] = np.dot(x[:n,:], w[0,:]) + np.random.normal(size=n)*0.01
  y[n:] = np.dot(x[n:,:], w[1,:]) + np.random.normal(size=N-n)*0.01
  #
  rx = np.ones( (N,cols) )
  r = np.arange(0,1,0.01) # Time 
  rx[:,0] = r
  # Plot the random data set
  if show:
    plt.plot(x[:,0],y, '.k')
    plt.plot(r, np.dot(rx,w[0,:]), ':b', linewidth=2)
    plt.plot(r, np.dot(rx,w[1,:]), ':r', linewidth=2)
    plt.show()
  return x, y, r, rx



def em(x, y, r, rx, iters=100, lam=0.01):
  """
  Expectation maximization algorithm
  """
  # Mixture weights
  rpi = np.zeros( (2) ) + 0.5
  # Expected mixture weights for each data point
  pi = np.zeros( (len(x),2) ) + 0.5
  # Regression weights
  w1 = np.random.rand(2)
  w2 = np.random.rand(2)
  # Precision term for the probability of the data under the regressor fn
  eta = 100
  # Iterate EM iter number of times
  for it in np.arange(iters):
    if 1:
      plt.plot(r, np.dot(rx, w1), '-r', linewidth=2, alpha=0.3+(0.7*it/iters))
      plt.plot(r, np.dot(rx, w2), '-g', linewidth=2, alpha=0.3+(0.7*it/iters))
    # Compute likelihood for each data point
    err1 = y - np.dot(x,w1)
    err2 = y - np.dot(x,w2)
    prbs = np.zeros( (len(y),2) )
    prbs[:,0] = -0.5*eta*err1**2
    prbs[:,1] = -0.5*eta*err2**2
    # Compute expected mixture weights
    pi = np.tile(rpi, (len(x),1))*np.exp(prbs)
    pi = pi / np.tile(np.sum(pi,1),(2,1)).T
    # Max with respect to the mixture probabilities
    rpi = np.sum(pi,0)
    rpi = rpi / np.sum(rpi)
    # Max with respect to regression weights
    pi1x = np.tile(pi[:,0],(2,1)).T * x
    xp1 = np.dot(pi1x.T, x) + np.eye(2) * lam/eta
    yp1 = np.dot(pi1x.T, y)
    w1 = lin.solve(xp1, yp1)
    #
    pi2x = np.tile(pi[:,1],(2,1)).T * x
    xp2 = np.dot(pi2x.T,x) + np.eye(2) * lam/eta
    yp2 = np.dot(pi[:,1]*y, x)
    w2 = lin.solve(xp2, yp2)
    # Max with respect to (wrt) the precision term
    eta = np.sum(pi) / np.sum(-prbs/eta*pi)
    # Objective function -- unstable as the pi's become concentrated
    #                       on a single component
    obj = np.sum(prbs * pi) - np.sum(pi[pi>1e-50] * np.log(pi[pi>1e-50])) + \
          np.sum(pi * np.log(np.tile(rpi,(len(x),1)))) + \
          np.log(eta)*np.sum(pi)
    print(obj, eta, rpi, w1, w2)
    #
    try:
      if np.isnan(obj): break
      if np.abs(obj - oldobj) < 1e-2: break
    except:
      pass
    # Set old object to current one
    oldobj = obj
  plt.show()
  return w1, w2

    
  
  
  






