# directed_walk.py - 2-D only
# usage python directed_walk.py

import numpy as np
import math



## Helper functions

def weight_to_one(nOptions, nRands=1000, returnDistribution=False):
  """
  Creates a weighted random distribution toward 1 so that 1 is most
  likely to be chosen and 0 is least likely. The rands are divided into
  the number of options and the index of the option chosen is returned 
  (which will usually be a small index since that is closest to 1).
  """
  distSample = np.random.randn(nRands)
  distSample = np.abs( 1 - np.abs(distSample / np.max(distSample)) )
  sample = np.random.choice(distSample, 1, replace=True)
  chosenInd = nOptions - int(math.ceil(sample * nOptions))
  chosenValue = sample
  
  if returnDistribution == False:
    return chosenInd, chosenValue
    
  elif returnDistribution == True:
    # the distribution below can be returned if desired
    distArray, binedges = np.histogram(distSample)
    distDistribution = zip(distArray, binedges)
    return chosenInd, chosenValue, distDistribution



def get_angle(prevPt, curPt, nextPt):
  """
  Find the angle between 3 points
  """
  A, B, C = prevPt, curPt, nextPt
  def sidelength(X,Y):
   # print X, Y
    return math.sqrt( (X[0]-Y[0])**2 +
                      (X[1]-Y[1])**2 ) # +
                      # (X[2]-Y[2])**2 )

  a, b, c = sidelength(B,C), sidelength(A,C), sidelength(A,B)
  cosB = ((a**2 - b**2 + c**2) / (2*a*c))
  cosA = ((-a**2 + b**2 + c**2) / (2*c*b))
  cosC = ((a**2 + b**2 - c**2) / (2*b*a))

  angB, angA, angC = math.acos(cosB), math.acos(cosA), math.acos(cosC)
  if (angA+angB+angC)-180 < 5:
    return angB, #ang*(180/math.pi)
  else:
    print('angles do not add up to 180')
    return angB, #ang*(180/math.pi)

def init_pointList():
  pointList = np.array([[0,0]])
  
  


def choose_point(currentPt, activePoints):
  """
  create a weighted distribution scaled to the closest active points
  """
  # create list of distances from each point, 
  #   retain original activatePoints order
  distances = [np.linalg.norm(currentPt-activePoints[i]) 
               for i in xrange(len(activePoints))]
  # get simple indices of where each active point is
  distanceInds = range(len(distances))
  # sort the distances from indexed points by closest to farthest
  sortDistances = [(y,x) for (y,x) in 
                   sorted(zip(distances, distanceInds))]

  # create a random distribution [0,1] skewed toward 1 (the closer pts)
  # distSample = np.random.randn(1000)
  # distSample = np.abs( 1 - np.abs(distSample / np.max(distSample)) )
  # pick a number, use it to find the desired point
  # sample = np.random.choice(distSample, 1, replace=True)
  # find which point it corresponds to and the distance of the step
  # chosenPointDistInd = math.ceil(sample * len(activatePoints)
  # sortDist is [(dist (closest 1st), activePointInd), ... ]
  chosenInd, chosenValue = weight_to_one(len(activePoints))
  chosenPoint = activePoints[sortDistances[chosenInd][1]]
  
  return chosenPoint, chosenValue



def choose_angle(pointList, activePoints):
  """
  Given the possible points, weight the closest point highest and 
  select the angle of approach randomly with that weighting
  """
  # get closest point
  chosenPoint, chosenDistance = choose_point(pointList[-1], activePoints)
  
  ## create distribution of angles
  # set prevPt and curPt
  if len(pointList) == 1:
    prevPt = np.array([0.01,0.01])
    curPt = np.array([0.,0.])
  else:
    curPt = pointList[-1]
    prevPt = pointList[-2]
  
  # get the current active point index
  #for i in xrange(len(activePoints)):
  #  if activePoints[i] == curPt:
  #    chosenInd = i
  
  # get the current angle and get an probabilistic angle from dist.
  angle = get_angle(prevPt, curPt, chosenPoint) # returns radians
  
  
  
  
  
  

  









