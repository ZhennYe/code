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
from imageMatrix import *
from spiral import *



class Hoc(geoFile, tiffFolder, voxel=[0.176,0.176,0.38]):
  def __init__(self, geoFile, tiffFolder=None):
    if tiffFolder == None:
      self.tiffFolder = '/home/alex/data/morphology/848/848_081/fake_filament/'
    self.voxel = voxel
    self.geofile = geoFile
    self.varr = None
    self.geometry, self.varr = load_geometry(geoFile, tiffFolder)
    self.skelpoints, self.skel_vectors, self.skel_names = [], [], []
    self.skel_coords() # load skelcoords and skel vectors
    self.segments = {'*':None} # segments and nodes will go here
    self.hoc = [] # tuple of [seg_name,[x,y,z],rad]
    
    
    
  ##################### Loading functions ##################################
  
  def load_geometry(self)
    # load voxelized image from directory, discard coordinates
    _, self.varr = get_voxel_locations(self.tiffFolder, 'f', self.voxel, False)
    self.geometry = demoRead(hocFile)
    return self


  def skel_coords(self):
    # return all skelcoords
    for s in self.geometry.segments:
      for n in range(len(s.nodes)-1):
        pt0 = [s.nodes[n].x, s.nodes[n].y, s.nodes[n].z]
        pt1 = [s.nodes[n+1].x, s.nodes[n+1].y, s.nodes[n+1].z]
        self.skelpoints.append(pt0)
        self.skel_vectors.append(get_vector(pt0,pt1))
        self.skel_names.append(s.name)
        # this method skips the initial connecting plane, which
        # would be really messy anyway
    return self



  ######################## Small functions ################################

  def dist(pt0, pt1):
    if len(pt0) == len(pt1):
      return math.sqrt( (pt0[0]-pt1[0])**2 + 
                        (pt0[1]-pt1[1])**2 +
                        (pt0[2]-pt1[2])**2 )
    else:
      print('Dimension mismatch:')
      print(pt0, pt1)







  ####################### Money functions ##################################

  def get_cross_sections(self):
    """
    Do like everything.
    """
    def add_node(n,s, cross_dims):
      if self.skel_names[n] not in self.segments.keys():
        # if it doesn't exist, then the first node doesn't exist either
        self.segments[self.skel_names[n]] = {'0': {'coord': self.skelpoints[n],
                                                   'area': s.area*cross_dims[0]*cross_dims[1],
                                                   'surface': s.surface['xs']*cross_dims[0] +\
                                                              s.surface['ys']*cross_dims[1],
                                                   'arr': reproduce_matrx(s.live_pts)}}
        self.segments[self.skel_names[n]] = {'0': {'rad': \
        self.segments[self.skel_names[n]]['0']['surface']/2*np.pi}}
      else:
        new = len(self.segments[self.skel_names[n]])
        self.segments[self.skel_names[n]][str(new)]={'coord': self.skelpoints[n],
                                                     'area': s.area*cross_dims[0]*cross_dims[1],
                                                     'surface': s.surface['xs']*cross_dims[0] +\
                                                                s.surface['ys']*cross_dims[1],
                                                     'arr': reproduce_matrix(s.live_pts)}
        self.segments[self.skel_names[n]] = {str(new): {'rad': \
        self.segments[self.skel_names[n]][str(new)]['surface']/2*np.pi}}
      return self
      
      
    # create cross-sections to populate nodes & segments
    for n in range(len(self.skelpoints)):
      plancoords, numpts = gen_plane(self.skel_vectors[n],self.voxel) # imageMatrix
      cross_dims = plane_XY(plancoords, numpts) # imageMatrix
      vcoords = scale_plan(plancoords, self.skelpoints[n], self.voxel) # imageMatrix
      sarr = return_cross_sec_array(vcoords, self.varr, numpts) # imageMatrix
      s = Spiral(sarr) # spiral
      # populate segments and nodes dicts
      self.add_node(n,s,cross_dims)
      self.hoc.append([self.skel_names[n], self.skelpoints[n],
                       self.segments[self.skel_names[n]][str(len(self.segments[self.skel_names[n]]))]['rad']])
    # done
    return self
      
  
  def get_connections(self):
    conns = []
    lineNum = 0
    
    with open(self.geofile, 'r') as fIn:
      for line in fIn:
        lineNum = lineNum + 1
        splitLine = line.split(None)
        if type(splitLine) is list:
          if splitLine[0] == 'connect':
            # save everything 
          conns.append(line)
    # should have all the connections now as a list in conns
    return conns
        
  
  

  def writeHoc(self, newName):
    """
    Create a new hoc file, but mantain all of the previous connections
    info since that's a pain to reproduce.
    """
    
    
  

"""
class Seg:
  def __init__(self, 
"""







