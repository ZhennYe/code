# abbrev_readExportedGeometry.py -- pick and choose functions from
#  NeuronGeometry
# This program is based on Ted's more comprehensive programs but 
#  abbreviated so that it runs more easily

import os, sys, re, math
from NeuronGeometry import *


#############################################################################
""" 
HocGeometry calls (outside of itself): 
  
"""
class HocGeometry(Geometry):
  def __init__(self, _fileName=None):
    Geometry.__init__(self)
    self._openSegment = None 
    self._connections = []
    self._segmentNames = []
    self._segments = {}
    self._warnRepeatSegments = True
    
    if _fileName is not None:
      self.setFileName(_fileName)
      self.readGeometry()
  # end of __init__
  
  def readGeometry(self):   # calls: addOneNodeCompartment, 
                            # 
    # get dictionary object of neuron model geometry info by reading file
    
    lineNum = 0
    with open(self.fileName, 'r') as fIN:
      try:
        for line in fIn:
          # loop through each line in the file
          
          # increase line number
          lineNUm = lineNum + 1
          # parse geometry
          self._parseHocGeometryLine(line)
      
      except IOError as err:
        sys.tracebacklimit = 0
        raise IOError('Error reading %s line %d: %s' % \
                     (self.fileName, lineNum, err.message))
    
    if self._openSegment:
      raise IOError('Error reading %s, segment %s open at end of file' % \
                    (self.fileName, self._openSegment))
    
    # connect segments and remove segments and connections, leaving segments
    # and nodes
    self._connectSegments()
    
    # make compartments from hemispheres remaining at the end of unconnected
    # segments
    self._addOneNodeCompartments()
    
    # name segments by segment number, and do sanity check on segment names
    sanityFail = 0
    for segNum in range(len(self.segments)):
      seg = self.segments[segNum];
      seg.name = 'Segment%d' % segNum
      filNum = self.getSegmentIndex(seg)
      if filNum != segNum:
        sanityFail += 1
    if sanityFail > 0:
      print("Warning: segments in .hoc file are out of order!")
      self.segments.sort(key=lambda s:self.getSegmentIndex(seg))
    # end readGeometry
      
      
  def getSomaIndex(self):   # calls: getSegmentIndex, findSoma
    """
    return (segmentIndex, position)
      segmentIndex indexes the .hoc file segment that contains the Soma
      position is a float between 0 and 1 that points to the soma centroid on
        the segment
    """
    # get the Soma
    self.findSoma()
    soma = self._somaSeg
    segmentIndex = self.getSegmentIndex(soma)
    # get the centroid of the Soma
    centroid = soma.centroidPosition(mandateTag='Soma')
    return (segmentIndex, centroid)
  # end getSomaIndex
  
  
  def getTipIndices(self):   # calls: findAxons
    """
    return (segmentInds, positions)
      segmentInds is a list of indices to .hoc file segments that contain
        terminal segments
      positions is a list of floats (0 or 1) that point to the terminal end
        of each terminal segment
    NOTE: This will NOT contain Axon or Soma even if they are terminal segments
    """
    _subGraphs = self.checkConnectivity(removeDisconnected=True)
    
    soma = max(self.segments, key=lambda s: s.maxRadius)
    soma.tags.add('Soma')
    axons = self.findAxons()
    exclude = set(['Soma', 'Axon'])
    
    terminalSegs = [s for s in self.segments if s.isTerminal and \
                    not any(s.tags.intersection(exclude))]
    
    def _getEnd(s):
      if not s.neighbors0:
        return 0.0
      else:
        return 1.0
    
    segmentInds, positions = zip( *[(self.getSegmentIndex(s), _getEnd(s)) \
                                   for s in terminalSegs] )
    return segmentInds, positions
  # end getTipIndices
  
  
  def getAxonIndices(self):   # calls: findAxons
    """
    return (filamentInds, positions)
      filamentInds (segmentInds) is a list of indices to .hoc file filaments that contain
        terminating branches
      positions is a list of floats (0 or 1) that point to the terminal end
        of each terminating branch
    """
    _subGraphs = self.checkConnectivity(removeDisconnected=True)
    axons = self.findAxons()
    
    def _getEnd(b):
      if not b.neighbors0:
        return 0.0
      else:
        return 1.0
    
    if axons:
      segmentInds, positions = \
        zip( *[(self.getFilamentIndex(b), _getEnd(b)) for b in axons] )
    else:
      segmentInds, positions = [], []
    
    return segmentInds, positions
  # end getAxons indices
  
  
  def _parseHocGeometryLine(self, line):   # calls: 
    """
    Read a line from hoc file specifying geometry, and update geometryInfo
    appropriately.
    openFilament = name of filament if in a declaration block, otherwise = None
    """
    splitLine = line.split(None)
    if not splitLine:
      return
    
    if self._openSegment:
      self._parseDefineSegment(line)
    elif splitLine[0] == 'connect':
      self._addConnection(splitLine)
    elif splitLine[0] == 'create':
      self._createFilaments(splitLine)
    elif splitLine[0] == 'neuron_name':
      self.name = splitLine[-1]
    elif splitLine[0].lower() == "range":
      if len(splitLine) < 7:
        raise IOError(\
          'range should be of form "range minX maxX minY maxY minZ maxZ"')
      self.minRange = tuple([float(x) for x in splitLine[1:6:2]])
      self.maxRange = tuple([float(x) for x in splitLine[2:7:2]])
    elif splitLine[0] in self._filamentNames:
      self._openFilament = splitLine[0]
    elif splitLine[0]+'[0]' in self._filamentNames:
      self._openFilament = splitLine[0]+'[0]'
  # end _parseHocGeometryLine
  
  
  def _parseDefineSegment(self, line):
    """
    Parse a line in a filament declaration block. Add node, clear nodes, or
    close block. If multiple nodes are added in one declaration block, connect
    them. Update geometryInfo['nodes'] and geometryInfo['filaments']
    appropriately.
    """
    splitLine = re.split(',|\)|\(', line.strip())
    
    openSegment = self.segments[self._filamentNames.index(self._openFilament)]
      
    if splitLine[0] == '}':
      self._openFilament = None
    elif splitLine[0] == 'pt3dclear':
      openSegment.clear()
    elif splitLine[0] == 'pt3dadd':
      if not (len(splitLine) == 6 or
              (len(splitLine) == 7 and splitLine[-2] == '0')):
        raise IOError('Unexpected form for pt3dadd')
      x,y,z,d = tuple(float(s) for s in splitLine[1:5])
      if d <= 0:
        if d == 0:
          raise ValueError('pt3dadd with diameter = 0.0')
        else:
          raise ValueError('pt3dadd with ciameter < 0.0')
        
      self._addNode(openSegment, x, y, z, 0.5 * d)

      if len(openSegment.nodes) > 1:
        node0 = openSegment.nodes[-2]
        node1 = openSegment.nodes[-1]
        self._addCompartment(openSegment, node0, node1, append=True)
    else:
      raise IOError('Invalid filament command')
  
  
#############################################################################
def _parseArguments(): 
  # this section ensures the proper number of arguments are given
  arguments = sys.argv
  if len(arguments) != 2:
    print('Incorrect number of arguments.')
  geoFile = arguments[1]
  return geoFile


#############################################################################
def abbRead(geoFile):  
  # read in geometry file (parsed already in _parseArguments)
  #  based on demoRead from neuron_readExportedGeometry.py
  geometry = abbHocGeometry(geoFile) # this references an abbreviated class
  
  # This section gets indices to axon, soma and tips, then measures
  #  distances from axon to soma and from axon to random tips
  print(geometry.name)
  try:
    axonInds, axonsTipPos = geometry.getAxonIndices()
    print('Axon tip length = %g' % geometry.segments[axonInds[0]].length)
    print('Axon tip pos = %f' % axonTipPos[0])
  except ValueError:
    print('Could not read axonInds or axonTipPos')
  # can do path distance stuff here if it works


#############################################################################
if __name__ == "__main__":
  # do the geometry
  geoFile = _parseArguments()
  #run a demo of capabilities
  demoRead(geoFile)
  # exit
  sys.exit(0)
