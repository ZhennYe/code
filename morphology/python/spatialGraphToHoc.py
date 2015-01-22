# spatialGraphToHoc.py - create a hoc file from amira spatial graph file
# usage: python amiraFileName hocFileName -options
# 1. load amira file into class 
# 2. apply desired operations (remove isolated sections ('-iso'),
#                              remove loops ('-loops'),
#                              remove ribbing ('-ribs'))
#

import sys, os
import math
import numpy as np



class Loops:
  



class SptGr:
  
  def __init__(self, amiraFileName, hocFileName='outputFile'):
    self.inFile = amiraFileName
    self.outFile = hocFileName
    self.properties = {}
    self.endpoints = []
    self.getEndpoints = False
    self.nodes = []
    self.getNodes = False
    self.sections = {} # numbered sections with nodes
    self.connections = {} # also tells which "end" a neighbor is 
                          # connected at
    self.getConnections = False
    self.rads = []
    self.getRads = False
    self.lengths = []
    self.getLengths = False
    self.neighbors = {} # tells which 'end' a neighbor is at
    self.islands = {}
    self.keepIsland = {}
    self.loopsOn = False
    self.getLoops = False
    self.loopSecs = []
    self.loops = {}
    
    # run through class methods
    # default is to just remove islands
    self.readAmiraFile()
    self.sanitizeLists()
    
  
  
  
  def readAmiraFile(self):
    # read lines from spatial graph file (may need sub-functions)
    
    lineNum = 0
    print('Loading file %s ...' % self.inFile)
    
    with open(self.inFile, 'r') as fIn:
      # try
      
      for line in fIn:
        lineNum = lineNum + 1
        splitLine = line.split(None)
        
        if splitLine[0] == 'Parameters':
          self.loopsOn = True
        elif splitLine[0] == 'define':
          self.properties[splitLine[1]] = int(splitLine[2])
        elif splitLine[0] == '@1':
          self.getEndpoints = True
        elif splitLine[0] == '@2':
          self.getEndpoints = False
          self.getConnections = True
        elif splitLine[0] == '@3':
          self.getConnections = False
          self.getLengths = True
        elif splitLine[0] == '@4' and self.loopsOn == True:
          self.getLoops = True
        # @4 and @5 depend on whether there are loops present
        elif (splitLIne[0] == '@4' and self.loopsOn == False) or \
             (splitLIne[0] == '@5' and self.loopsOn == True):
          self.getLoops = False
          self.getLengths = False
          self.getNodes = True
        elif (splitLine[0] == '@5' and self.loopsOn == False) or \
             (splitLine[0] == '@6' and self.loopsOn == True):
          self.getNodes = False
          self.getRads = True
        
        elif self.getEndpoints == True:
          self.endpoints.append([float(i) for i in splitLine])
        elif self.getConnections == True:
          self.connections.append([int(i) for i in splitLine])
        elif self.getLengths == True:
          self.lengths.append([int(splitLine)])
        elif self.getLoops == True:
          self.loopSecs.append(int(splitLine))
        elif self.getNodes == True:
          self.nodes.append([float(i) for i in splitLine])
        elif self.getRads = True:
          self.rads.append([float(splitLine)])
          
        
    print(' done.')
    
    return self
  
        
  
  def sanitizeLists(self):
    """
    remove empty [] elements from the lists; these arise from
    newline characters that are included due to the \n between
    the last element in the sptgr and the next section ('@...')
    """
    print('Sanitizing lists ...')
    # find and dispose of all empty indices in self.lengths
    mt_inds = [i for i,x in enumerate(self.lengths) if x == [] ]
    for i in xrange(len(mt_inds)):
      self.lengths.pop(mt_inds[i - i])
    # turn eliminate "second" dimension of self.lengths [[x],[y]] -> [x,y]
    self.lengths = [self.lengths[i][0] 
                    for i in xrange(len(self.lengths))]
      
    # find and dispose of all empty indices in self.nodes
    mt_inds = [i for i,x in enumerate(self.nodes) if x == [] ]
    for i in xrange(len(mt_inds)):
      self.nodes.pop(mt_inds[i - i])
      
    # find and dispose of all empty indices in self.endpoints
    mt_inds = [i for i,x in enumerate(self.endpoints) if x == [] ]
    for i in xrange(len(mt_inds)):
      self.endpoints.pop(mt_inds[i - i])
      
    # find and dispose of all empty indices in self.connections
    mt_inds = [i for i,x in enumerate(self.connections) if x == [] ]
    for i in xrange(len(mt_inds)):
      self.connections.pop(mt_inds[i - i])
      
    # find and dispose of all empty indices in self.rads
    mt_inds = [i for i,x in enumerate(self.rads) if x == [] ]
    for i in xrange(len(mt_inds)):
      self.rads.pop(mt_inds[i - i])
    self.rads = [self.rads[i][0] for i in xrange(len(self.rads))]
    print(' done.')
    
    return self



  def createSections(self):
    # use lengths and nodes to create node-detailed sections
    if len(self.sections) > 0:
      print('Warning: sections already exist in self.sections!')
    if [] in self.lengths:
      self.sanitizeLists()
    
    print('Creating sections with specific nodes ... ')
    for sec in xrange(len(self.lengths)):
      startnode = int( np.sum(self.lengths[:sec]) )
      endnode = int( startnode + self.lengths[sec] )
      self.sections[sec] = self.nodes[startnode:endnode]
    print(' done.')
    
    return self
        
    

  def findNeighbors(self):
    # find neighbors from connections list and overlappying node tuples
    # structure is self.neighbors[seg] = [neighborseg[end], ...]
    def whereConnect(self, seg0, seg1):
      # find where the two segments are connected (arbitrary based on
      # location of node in the list
      seg00, seg01 = self.sections[seg0][0], self.sections[seg0][-1]
      seg10, seg11 = self.sections[seg1][0], self.sections[seg1][-1]
      if seg00 in [seg10,seg11]:
        if seg00 == seg10:
          return [0,0]
        else:
          return [0,1]
      if seg01 in [seg10,seg11]:
        if seg01 == seg10:
          return [1,0]
        else:
          return [1,1]
      else:
        return False
    
    if len(self.neighbors) > 0:
      print('Warning: self.neighbors already populated with %i entries' 
            % len(self.neighbors) )
    
    # first entry in list is probably real and not an island
    for cxn in len(self.connections):
      seg0, seg1 = self.connections[cxn][0], self.connections[cxn][1]
      connects = self.whereConnect(seg0, seg1)
      if connects is not False:
        # if an entry does not exist for a section, create it
        if seg0 not in self.neighbors.keys():
          self.neighbors[seg0] = [seg1,[connects[0]]]
        else:
          self.neighbors[seg0].append([seg1,connects[0]])
        if seg1 not in self.neighbors.keys():
          self.neighbors[seg1] = [seg0, connects[1]]
        else:
          self.neighbors[seg1].append([seg0,connects[1]])
      else:
        print('No valid connection betweel segs %i and %i.'
              % (seg0, seg1) )
    
    return self



  def returnNeighbors(self, segnum):
    # call with self a chosen segment number, will return all neighbors
    inds0 = [i for i, x in enumerate(self.connections) if x[0]==segnum]
    inds1 = [i for i, x in enumerate(self.connections) if x[1]==segnum]
    neighborsegs = [self.connections[i][0] for i in inds0] + \
                   [self.connections[i][1] for i in inds1] 
    return neighborsegs
    
  
  
  def uniqueArray(preArr, refList):
    # refList should be a list of segments that have already been ass'n'ed
    m,n = np.shape(preArr)
    badInds = []
    if n != 2:
      print('npArray of connections must be N x 2.')
    else:https://www.youtube.com/watch?v=0QvJ3dXqmvw
      count = 0
      for x,y in preArr:
        if x in refList:
          badInds.append(count)
        if y in refList:
          badInds.append(count)
        count = count + 1
      # once all common indices are found, take unique() and remove them
      badInds = np.unique(badInds)
      postArr = np.delete(preArr, (badInds), axis=0)
      
      return postArr
  
  
  
  
  def findIslands(self):
    """
    by default finds all islands, used to group first (then can be 
    eliminated later
    """
    if len(self.islands) != 0:
      print('Warning: self.islands already contains %i islands!' \
            % len(self.islands))
    concopy = np.array(self.connections[:])
    curseg = concopy[0,0]
    todo, finished, segList = [], [], []
    go = True
    
    print('Finding islands ...')
    while go:
      newsegs = self.returnNeighbors(curseg)
      # for each segment attached to curseg
      for seg in newsegs:
        # if a neighboring segment isn't already in the seg list, add it
        if seg not in segList:https://www.youtube.com/watch?v=0QvJ3dXqmvw
          segList.append(seg)
        if seg not in finished:
          todo.append(seg)
        # add curseg to completed segments
        finished.append(curseg)
        
      # remove segments from todo if they're already done
      for seg in todo:
        if seg in finished:
          todo.remove(seg)
      
      # repeat analysis with next todo segment; remove it from todo
      if len(todo) >= 1:
        curseg = todo[0]
        todo.remove(curseg)
        if curseg not in segList:
          segList.append(curseg)
      # unless there are no segments left in todo
      else:
        if len(finished) != len(segList):
          print('Warning: finished seglist has %i segments while \
                segList has %i segments' % (len(finished), len(segList)))
        # create new island
        self.islands[len(islands)] = segList
        # remove used segments from connectionscopy
        commonInds = uniqueArray(concopy, finished)
        concopy = np.delete(concopy, commonInds, axis=0)
        todo, finished, segList = [], [], []
        curseg = concopy[0,0]
        # if all concopy segments are assigned, stop while loop
        if len(concopy) == 0:
          go = False
    # exit while loop
    print(' finished.')
    
    return self

https://www.youtube.com/watch?v=0QvJ3dXqmvw

  def removeIslands(self, params=False):
    """
    Calls findIslands to get the segment list that each island contains.
    Finds the size of each island and automatically removes very small
    (<10% of largest size) 
    Other options determine: 
    """
    islandsize = {}
    # get the size of each island
    for isl in self.islands.keys():
      size = 0
      for seg in self.islands[isl]:
        size = size + len(self.sections[seg])
      islandsize[isl] = size
    
    # find the largest island
    maxisl = 0
    for isl in islandsize.keys():
      if islandsize[isl] > maxisl:
        maxisl = islandsize[isl]
        maxkey = isl
    for seg in self.islands[maxkey]:
      self.keepIsland[seg] = self.sections[seg]
    
    return self
  
  
  
  def repopulateIsland(self):
    """
    Create a new Spatial graph/ hoc file from just the biggest island.
    Can preserve loops.
    """
    if not self.keepIsland:
      print('Island analysis not run before repopulateIsland call.')
    else:
      if len(self.keepIsland) < 1:
        print('keepIsland does not exist')
    print('Creating new Amira File from largest island only.')
    segList = self.keepIsland.keys()
    connectList = []
    for con in self.connections:
      if con[0] in segList or con[1] in segList:
        connectList.append(con)
    vertexList = []
    for seg in segList:
      vertexList.append([self.sections[seg][0]])
      vertexList.append([self.sections[seg][-1]])
    ptList, radList = [], []
    for seg in segList:
      for pt in self.sections[seg]:
        ptList.append([self.sections[seg][pt]])
      for rd in self.secRads[seg]:
        radList.append([self.secRads[seg][rd]])
    numedgepts = [len(self.sections[seg]) for seg in segList]
    
    return vertexList, connectList, numedgepts, ptList, radList
  
  
  
  """
  ### 
  Used Amira to remove all but the single largest connected graph.
  ###
  def writeIslandFile(self):
    # Creates an amira spatial graph file from the largest identified 
    # island. Calls (1) findIslands, (2) removeIslands, (3) repopulate-
    # Islands.
    with open(self.outFile, 'w') as fOut:
      def writeDataLine(dat):
        fOut.write
  """
    
  
  
  def checkEndpoints(self):
    """
    Make sure that the class of [0] and [-1] elements of skelpoints
    is the same as the endpoints list
    """
    print('Checking endpoints...')
    allendpoints = []
    for seg in self.sections.keys():
      allendpoints.append([self.sections[seg][0]])
      allendpoints.append([self.sections[seg][-1]])
    # keep only unique endpoints
    allendpoints = np.array(allendpoints)
    ends = np.ascontiguousarray(allendpoints).view(np.dtype((np.void, \
           allendpoints.dtype.itemsize * allendpoints.shape[1])))
    _, idx = np.unique(ends, return_index=True)
    unique_endpoints = np.copy(allendpoints[idx])
    
    if len(self.endpoints) == len(unique_endpoints):
      selfendpoints = self.endpoints[self.endpoints[:,0].argsort()]
      uniqueendpoints = unique_endpoints[unique_endpoints[:,0].argsort()]
      boolary = selfendpoints == uniqueendpoints
      if False in boolary:
        print('Not all endpoint tuples are equal. Running elementwise.')
        for pt in self.endpoints:
          if pt not in unique_endpoints:
            print('unique_endpoints missing pt (%.5f, %.5f, %.5f)' \
                  % (pt[0],pt[1],pt[2]))
      else:
        print('All endpoints present in both arrays.')
    else:
      print('Unequal numbers of endpoints. Running elementwise.')
      for pt in self.endpoints:
        if pt not in unique_endpoints:
          print('unique_endpoints missing pt (%.5f, %.5f, %.5f)' \
                % (pt[0],pt[1],pt[2]))
    # no errors means seg[0] and [-1] can be used for endpoints
    print('Done checking endpoints.')
    
    return self
  
  
  
  def sectionRads(self):
    # assign radiuses to the sections
    if len(self.secRads) > 0:
      print('Radiuses already assigned')
    print('Creating radiuses ...')
    for sec in xrange(len(self.lengths)):
      startnode = int( np.sum(self.lengths[:sec]) )
      endnode = int( startnode + self.lengths[sec] )
      self.secRads[sec] = self.rads[startnode:endnode]
    
    return self
  

    
  def processLoops(self):
    """
    Figure out which segments contribute to each loop.
    """
    if len(self.loop) > 0:
      print('Warning: self.loops already has members')
    
    for sec in self.loopSecs:
      if self.loopSecs[sec] not in self.loops.keys():
        self.loops[self.loopSecs[sec]] = [sec]
      else:
        self.loops[self.loopSecs[sec]].append(sec)
    
    return self
  
  
  
  def insertBreak(self, secNum):
    """
    Inserts a break in the given section and edits the section list,
    neighbor data and radius list
     
    """
    maxSec = max(self.sections.keys())
    try:
      minrad = min(self.secRads[secNum])
    except:
      minrad = np.min(self.secRads[secNum])
      
    for pt in self.secRads[secNum]:
      if self.secRads[secNum][pt] == minrad:
        ptInd = pt
    #edit section node list
    self.sections[maxSec] = self.sections[secNum][ptInd+1:]
    self.section[secNum] = self.sections[secNum][:ptInd]
    # edit section radius list
    self.secRads[maxSec] = self.secRads[secNum][ptInd+1:]
    self.secRads[secNum] = self.secRads[secNum][:ptInd]
    
    # adjust the neighbors data
    # structure is self.neighbors[seg] = [[seg, end], ...]
    for neb in self.neighbors[secNum]:
      if self.neighbors[secNum][neb][1] == 1:
        if maxSec not in self.neighbors.keys():
          self.neighbors[maxSec] = [[neb, 1]]
          self.neighbors[secNum][neb] = []
        else:
          self.neighbors[maxSec].append([neb, 1])
    for k in self.neighbors.keys():
      for seg in self.neighbors[k]:
        if seg[0] == secNum and seg[1] == 1:
          if maxSec not in self.neighbors.keys():
            self.neighbors[maxSec] = [[k,1]]
          else:
            self.neighbors[maxSec].append([k,1])
          self.neighbors[k][seg] = []
    
    return self
  
  
  
  def findThinSection(self, secs):
    """
    Pass the sections included in a loop and this returns the 
    particular section with the smallest cross-section
    """
    # smallRad = float('Inf') # set initial radius to inf
    # start with just the smallest rad for the first section
    try:
      smallRad = min(self.secRads[secs[0]])
    except:
      smallRad = np.min(self.secRads[secs[0]])
    smallSec = secs[0]
    
    for sec in secs:
      try:
        newMin = min(self.secRads[sec])
      except:
        newMin = np.min(self.secRads[sec])
      if newMin < smallRad:
        smallRad, smallSec = newMin, sec
    # should have found the smallest radius
    
    return self, smallSec
  
  
  
  def breakLoops(self, options=False):
    """
    Break the loops in a skeleton based on the thickness estimated
    radius of each point. Loops should be identified in Amira and
    defined at the beginning of the spatialgraph file.
    """
    if len(self.loops) == 0:
      print('No loops left to break')
    
    # loop through loops and edit sections
    for lp in self.loops.keys():
      targetSec = self.findThinSection(self.loops[lp])
      self.insertBreak(targetSec)
      # remove edited loop from self.loops
      del(self.loops[lp])
    
    return self
  
  
  
  ####################
  def findLoops(self):
    """
    Look for loops; calls class Loops
    """
        
        
  
  
  def writeAmiraFile(self):
    """
    Create a spatial graph-like file for reading back into Amira
    to scan again for loops and such
    
    """
    
    
  
  
  
  def writeHoc(self):
    """
    Write the hoc file from the data collected
    """
    
    
    







def spatialGraphControl(amiraFileName, hocFileName, options=False:
  # control the flow of the program
  # create new spatial graph class
  
  # execute options
  if options != False:
    pass
  
  # decide what to return
  




######################################
if __name__ == '__main__':
  arguments = sys.argv
  amiraFileName = arguments[1]
  if len(arguments) > 2:
    hocFileName = arguments[2]
  else:
    hocFileName = 'newhocfile.hoc'
  if len(arguments) > 3:
    options = arguments[3:]
  else:
    options = False
  spatialGraphControl(amiraFileName, hocFileName, options)
