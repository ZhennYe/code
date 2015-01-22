"""
usage: python angles_NeuronGeometry hocFile
  This function calls neuron_PointsExportedGeometry.py and 
    NeuronGeometry.py
"""

import sys
import os
from matplotlib import cm
from matplotlib.ticker import LinearLocator, FormatStrFormatter
import numpy as np
import math
import matplotlib.pyplot as pyplot
from mpl_toolkits.mplot3d import Axes3D
try:
  from NeuronGeometryOld import *
  import neuron_pointsExportedGeometry as points
except ImportError:
  raise ImportError('No file called NeuronGeometry.py and/or \
                     neuron_PointsExportedGeometry.py found.')
  
  

def getAngle(A, B, C):
  """
  send 3 points and return the angle of ABC (angle focused at B)
  A = seg1midpoint, B = seg1endpoint & seg2startpoint, C = seg2midpoint
  """
  def sidelength(X,Y):
   # print X, Y
    return math.sqrt( (X[0]-Y[0])**2 +
                      (X[1]-Y[1])**2 + 
                      (X[2]-Y[2])**2 )
  
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
  for each branchangle (and centroid) 
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

  #for i in xrange(len(midcoords)):
  #  print midcoords[i]
  centAngles = np.array([getAngle(npilCent, midcoords[i], endcoords[i])
                        for i in xrange(len(branchAngles))])
  centAngles = [i*180/math.pi for i in centAngles]
  print('Number of branch angles computed: %d' % len(centAngles))
  print('Mean centripetal angle: %.3f +/- %.3f' 
        % (np.mean(centAngles), np.std(centAngles)))
  # print branchAngles
  
  return centAngles, midcoords, branchAngles, npilCent



def plotBifAngles(centAngles, midcoords, branchAngles, npilCent, 
                  plot=False):
  """
  plot the angle of bifurcation against
  """
  def sidelength(X,Y):
   # print X, Y
    return math.sqrt( (X[0]-Y[0])**2 +
                      (X[1]-Y[1])**2 + 
                      (X[2]-Y[2])**2 )
  
  #centAngles, midcoords, branchAngles, npilCent = getBifAngles(hocFile)
  

  #print midcoords
  distToCent = [sidelength(npilCent, midcoords[i]) 
                for i in xrange(len(midcoords))]
  centAng = [(x,y) for (x,y) in sorted(zip(distToCent, centAngles))]
  XcentAng = [x for (x,y) in centAng]
  YcentAng = [y for (x,y) in centAng]
  coordlist = [x for (y,x) in sorted(zip(distToCent, midcoords))]
  Xs = [x for (x,y,z) in coordlist]
  Ys = [y for (x,y,z) in coordlist]
  Zs = [z for (x,y,z) in coordlist]
  
  brAng = [(x,y) for (x,y) in sorted(zip(distToCent, branchAngles))]
  XbrAng = [x for (x,y) in brAng]
  YbrAng = [y for (x,y) in brAng]
  #bins, patches, Q = pyplot.hist(distToCent, bins=20, linewidth=0,
  #                               alpha=0.8, color='black')
  
  
  # plot distance from neuropil vs centripetal angle
  fig = pyplot.figure()
  ax1 = fig.gca()
  ax1.scatter(XcentAng, YcentAng, c='k', marker='.')
  ax1.set_title('Centripetal angle vs Distance from center')
  ax1.set_xlabel('Distance from center')
  ax1.set_ylabel('Centripetal angle')
  #pyplot.show()
  #print centAng
  
  fig = pyplot.figure()
  ax2 = fig.gca()
  ax2.scatter(XbrAng, YbrAng, c='r', marker='.')
  ax2.set_title('Bifurcation angle vs Distance from center')
  ax2.set_xlabel('Distance from center')
  ax2.set_ylabel('Bifurcation angle')
  
  return



def plotBranchLengths(segLengths, segLocs, npilCent):
  """
  plot the length of branches as a function of distance from the center
  of the neuropil
  """
  mesh = True
  def sidelength(X,Y):
   # print X, Y
    return math.sqrt( (X[0]-Y[0])**2 +
                      (X[1]-Y[1])**2 + 
                      (X[2]-Y[2])**2 )
  
  distToCent = [sidelength(segLocs[i], npilCent)
                for i in xrange(len(segLocs))]
  lengths = [(x,y) for (x,y) in sorted(zip(distToCent, segLengths))]
  Xlengths = [x for (x,y) in lengths]
  Ylengths = [y for (x,y) in lengths]
  
  # create a 3D histogram with 2 bin edges for segLength and distToCent
  # as the X-Y and count as the Z

  fig = pyplot.figure()
  ax3 = fig.add_subplot(111, projection='3d')
  hist, xedges, yedges = np.histogram2d(Xlengths, Ylengths, bins=20)
  elements = (len(xedges) - 1) * (len(yedges) - 1)
  xpos, ypos = np.meshgrid(xedges[:-1]+0.25, yedges[:-1]+0.25)
  
  if np.shape(xpos) == np.shape(ypos) and np.shape(ypos) == np.shape(hist):
    print(np.shape(xpos))

  
  if mesh == False:
    xpos = xpos.flatten()
    ypos = ypos.flatten()
    zpos = np.zeros(elements)
    dx = 0.5 * np.ones_like(zpos)
    dy = dx.copy()
    dz = hist.flatten()
    
    ax3.bar3d(xpos, ypos, zpos, dx, dy, dz, color='b', zsort='average')
    ax3.set_title('Segment length vs distance histogram')
    ax3.set_xlabel('Distance from center')
    ax3.set_ylabel('Segment length')
    ax3.set_zlabel('Count')
  
  else:
    #dz = [float(dz[i])/max(dz) for i in xrange(len(dz))]
    #print(dz)
    surf = ax3.plot_surface(xpos, ypos, hist, rstride=1, cstride=1,
                            cmap=cm.coolwarm, linewidth=0, antialiased=False)
    ax3.set_title('Segment length vs distance histogram')
    ax3.set_xlabel('Distance from center')
    ax3.set_ylabel('Segment length')
    
    ax3.tick_params(axis='z', which='both',bottom='off',top='off',
                    labelbottom='off')
    ax3.view_init(90, 270)
    #ax3.zaxis.set_major_locator(LinearLocator(10))
    #ax3.zaxis.set_major_formatter(FormatStrFormatter('%.02f'))
    fig.colorbar(surf, shrink=0.5, aspect=5)
    savestr = ('%slengthVsDist.png' %hocFile.split('.')[0])
    pyplot.savefig(savestr)
  
  """
  ax3.scatter(Xlengths, Ylengths, c='b', marker='.')
  ax3.set_title('Segment length vs Distance from center')
  ax3.set_xlabel('Distance from center')
  ax3.set_ylabel('Segment length')
  """
  
  return
  




############################### control ################################
def anglesControl(hocFile, options=1):
  """
  control for the program
  """
  branchAngles, midcoords, endcoords, npilCent, segLengths, segLocs = \
    points.demoRead(hocFile, "", False)
  
  if options == 0:
    plotBifAngles(branchAngles, midcoords, endcoords, npilCent, False)
    pyplot.show()
  elif options == 1:
    plotBranchLengths(segLengths, segLocs, npilCent)
    pyplot.show()
  else:
    print('Bad options command given')
  
  return
    




########################################################################
if __name__ == "__main__":
  # print len(sys.argv)
  if len(sys.argv) > 2:
    options = int(sys.argv[2])
    print('options = %d given' % options)
  else:
    options=1
  hocFile = sys.argv[1]
  
  anglesControl(hocFile, options)
