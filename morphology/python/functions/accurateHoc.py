# accurateHoc.py -- this toolbox creates a very accurate hoc file
# usage: python accurateHoc.py hocFile tiffFolder outFile
# Input: 1. hocFile - hoc-format, really only used for the skeleton
#        2. tiffStack - the directory containing a tiffstack in B&W
#        3. outFile - filename for the completed hoc file
# Output: 1. outFile - a hoc file
# Dependencies: 
#        1. NeuronGeometry.py and neuron_readExportedGeometry.py, and
#           neuron_readExportedGeometry.demoread must return geometry
#        2. imageMatrix.py

import math
import numpy as np
from imageMatrix import *
from neuron_readExportedGeometry import *



##################### Loading functions ##################################

def load_everything(hocFile, tiffFolder)
  # load coords from given directory
  vol_coords = get_voxel_locations(folder, fname, [1,1,1], False)
  geometry = demoRead(hocFile)
  return geometry, vol_coords


def skel_coords(geometry):
  # return all skelcoords
  skelcoords = []
  for s in geometry.segments:
    for n in s.nodes:
      skelcoords.append([n.x, n.y, n.z])
  return skelcoords



######################## Small functions ################################

def dist(pt0, pt1):
  if len(pt0) == len(pt1):
    return math.sqrt( (pt0[0]-pt1[0])**2 + 
                      (pt0[1]-pt1[1])**2 +
                      (pt0[2]-pt1[2])**2 )
  else:
    print('Dimension mismatch:')
    print(pt0, pt1)


def closest_point(points, pt):
  if len(points[0]) != len(pt):
    print('Points mismatch: points has %i columns, should be %i'
          %(len(points[0]), len(pt)))
  else:
    minp, indp = np.inf, None
    for p in points:
      curr = dist(p, pt)
      if curr < minp:
        minp = curr
        indp = points.index(p)
    return indp, minp





# create used and unused lists of volcoords; create cascade of volpoints
def assign_vol_coords(hocFile, tiffFolder):
  geometry, vol_coords = load_everything(hocFil, tiffFolder)
  skelcoords = skel_coords(geometry)
  unused_vol = vol_voords.copy()
  used_vol = []
  raw_vols = {'*':None} # here the volumes of each skelpoint will be logged
  skel_vols = {'*':None} # here we'll log the actual coords (reproduce 
                        # each associated volume)
  # create skel_vols and raw_vols
  for pt in skelcoords:
    raw_vols[str(pt)] = 0
    skel_vols[str(pt)] = [pt]
  
  while len(unused_vol) > 0:
    










