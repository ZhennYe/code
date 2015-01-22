"""
usage: python angles_NeuronGeometry hocFile
  This function calls neuron_PointsExportedGeometry.py and 
    NeuronGeometry.py
"""

import sys
import os
import numpy as np
import math
try:
  from NeuronGeometry import *
  import neuron_PointsExportedGeometry as points
except ImportError:
  raise ImportError('No file called NeuronGeometry.py and/or \
                     neuron_PointsExportedGeometry.py found.')
  


def loadPoints(hocFile):
  """
  load the points 
  """
  starts, mids, ends, segnums, neighbs = points.pointsRead(hocFile, "", False)
  del neighbs['*']
  
  return starts, mids, ends, segnums, neighbs
  


def getAngle(A, B, C):
  """
  send 3 points and return the angle of ABC (angle focused at B)
  A = seg1midpoint, B = seg1endpoint & seg2startpoint, C = seg2midpoint
  """
  def sidelength(X,Y):
   # print X, Y
    return math.sqrt( (X[0][0]-Y[0][0])**2 +
                      (X[0][1]-Y[0][1])**2 + 
                      (X[0][2]-Y[0][2])**2 )
  
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



def getTriplet(seg1, seg2, segnums, starts, mids, ends):
  """
  given a segment and its known neighbor, this returns the point
  triplet needed to compute the bifurcation angle
  # is passing the lists required??
  """
  seg1loc, seg2loc = segnums.index(seg1), \
                                         segnums.index(seg2)
  if starts[seg1loc] == starts[seg2loc]:
    conP = starts[seg1loc]
  elif starts[seg1loc] == ends[seg2loc]:
    conP = starts[seg1loc]
  elif ends[seg1loc] == starts[seg2loc]:
    conP = ends[seg1loc]
  elif ends[seg1loc] == ends[seg2loc]:
    conP = ends[seg1loc]
  else:
    print('Error: no common point found between neighbors %d and %d'
          % (seg1, seg2))
  
  seg1mid, seg2mid = mids[seg1loc], mids[seg2loc]
  return seg1mid, conP, seg2mid





def getBifAngles(hocFile):
  """
  routine for getting bifurcation angles
  returns the angles and the location (x,y,z) of the angle point
  """
  starts, mids, ends, segnums, neighbors = loadPoints(hocFile)
  angles, points = [], []
  
  # for every segment that has neighbors
  for i in neighbors.keys():
    # and for every connection in that neighbor list
    try:
      valueLength = len(neighbors[i])
      for j in xrange(len(neighbors[i])):
        seg1 = neighbors.keys()[i]
        seg2 = neighbors[i][j]
      
        seg1mid, connectionPoint, seg2mid = \
                      getTriplet(seg1, seg2, segnums, starts, mids, ends)
        currentAngle = getAngle(seg1mid, connectionPoint, seg2mid)
        angles.append(currentAngle)
        points.append(connectionPoint)
    except:
      seg1 = neighbors.keys()[i]
      seg2 = neighbors[i]
      seg1mid, connectionPoint, seg2mid = \
                    getTriplet(seg1, seg2, segnums, starts, mids, ends)
      currentAngle = getAngle(seg1mid, connectionPoint, seg2mid)
      angles.append(currentAngle)
      points.append(connectionPoint)
  
  angles, points = np.array(angles), np.array(points)
  angles = [i*180/math.pi for i in angles]
  
  print('Avg. bifurcation angle: %.3f +/- %.3f' 
        % (180-np.mean(angles), np.std(angles)))
  
  return angles, points



def getNeuropilCenter(hocFile):
  """
  return the (x,y,z) 
  """
  
  



def plotBifAngles(hocFile):
  """
  plot the angle of bifurcation against
  """
  return
  
      
  
  






############################### control ################################
def anglesControl(hocFile, options=False):
  """
  control for the program
  """
  getBifAngles(hocFile)






########################################################################
if __name__ == "__main__":
  # print len(sys.argv)
  if len(sys.argv) > 2:
    options = sys.argv[2]
  else:
    options=False
  hocFile = sys.argv[1]
  
  anglesControl(hocFile, options)
