#!/usr/bin/python



_usageStr=\
"""usage: neuron_readExportedGeometry.py geoFile
  get dictionary object describing neuron model geometry info by reading file
"""


## make sure import is current NeuronGeometry
import os, sys, re, math
from NeuronGeometry_edited import *


"""
class PathDistanceFinder
Finds network distances from one segment/branch at specified position
Can report distance to another segment/branch at specified position via
  .distanceTo()
Note _getNetworkDistance() is used by Scholl analysis, but that's due to
  the requirement of reporting a range of distances for a whole
  segment/branch, rather than distance to a specific position
"""
class HocPathDistanceFinder(PathDistanceFinder):
  def __init__(self, geometry, segment, pos=0.5):
    print("HocPathDistanceFinder is deprectated. Use PathDistanceFinder")
    super(HocPathDistanceFinder, self).__init__(geometry, segment, pos)


class HocGeometry(Geometry):
  def __init__(self, _fileName=None):
    Geometry.__init__(self)
    self._openFilament = None
    self._connections = []
    self._filamentNames = []
    self._filaments = {}
    self._warnRepeatFilaments = True
    
    if _fileName is not None:
      self.setFileName(_fileName)
      self.readGeometry()
      
  
  def readGeometry(self):
    """
    get dictionary object describing neuron model geometry info by reading file
    """
     
    lineNum = 0
    with open(self.fileName, 'r') as fIn:
      # read the geometry file
      try:
        for line in fIn:
          # loop through each line in the file
          
          # inc the line number
          lineNum = lineNum + 1
          # parse the line in geometry file, adding info to geometryInfo
          self._parseHocGeometryLine(line)
    
      except IOError as err:
        sys.tracebacklimit = 0
        raise IOError('Error reading %s line %d: %s' % \
                      (self.fileName, lineNum, err.message))
    
    if self._openFilament:
      raise IOError('Error reading %s, filament %s open at end of file' %
                    (self.fileName, self._openFilament))
    
    # connect filaments and remove filaments and connections, leaving segments
    # and nodes
    self._connectFilaments()
    
    # make compartments from hemispheres remaining at the end of unconnected
    # segments
    self._addOneNodeCompartments()
    
    # name segments by segment number, and do sanity check on filament names
    sanityFail = 0
    for segNum in range(len(self.segments)):
      seg = self.segments[segNum];
      seg.name = 'Segment%d' % segNum
      filNum = self.getFilamentIndex(seg)
      if filNum != segNum:
        sanityFail += 1
    if sanityFail > 0:
      print("Warning: filaments in .hoc file are out of order!")
      self.segments.sort(key=lambda s:self.getFilamentIndex(seg))
      

  def getSomaIndex(self):
    """
    return (filamentIndex, position)
      filamentIndex indexes the .hoc file filament that contains the Soma
      position is a float between 0 and 1 that points to the soma centroid on
        the filament
    """
    # get the Soma
    self.findSoma()
    soma = self._somaSeg
    
    filamentIndex = self.getFilamentIndex(soma)
    
    # get the centroid of the Soma
    centroid = soma.centroidPosition(mandateTag='Soma')

    return (filamentIndex, centroid)

  
  def getTipIndices(self):
    """
    return (filamentInds, positions)
      filamentInds is a list of indices to .hoc file filaments that contain
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
    
    filamentInds, positions = zip( *[(self.getFilamentIndex(s), _getEnd(s)) \
                                   for s in terminalSegs] )
    return filamentInds, positions

  
  def getAxonIndices(self):
    """
    return (filamentInds, positions)
      filamentInds is a list of indices to .hoc file filaments that contain
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
    
    filamentInds, positions = zip( *[(self.getFilamentIndex(b), _getEnd(b)) \
                                   for b in axons] )
    return filamentInds, positions


  def _parseHocGeometryLine(self, line):
    """
    Read a line from hoc file specifying geometry, and update geometryInfo
    appropriately.
    openFilament = name of filament if in a declaration block, otherwise = None
    """
    splitLine = line.split(None)
    if not splitLine:
      return
    
    if self._openFilament:
      self._parseDefineFilament(line)
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
  
  def _parseDefineFilament(self, line):
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


  def _addConnection(self, splitLine):
    """
    Return dict describing connection between two filaments
    """
    (name1, location1) = re.split('\(|\)', splitLine[1])[0:2]
    (name2, location2) = re.split('\(|\)', splitLine[2])[0:2]
    
    connection = { \
      'filament1' : name1, \
      'location1' : float(location1), \
      'filament2' : name2, \
      'location2' : float(location2) \
    }
    self._connections.append(connection)


  def _createFilaments(self, splitLine):
    """
    Add requested number of filaments to geometry, as segments
    """
    if '[' and ']' in splitLine[1]:
      # hoc produced by Imaris, requests variable number of filaments
      # create baseName[numFilaments]
      baseName, numFilamentsStr = re.split('\[|\]', splitLine[1])[0:2]
      numFilaments = int(numFilamentsStr)
    else:
      # hoc produce by Amira, requests 1 filament
      # create baseName
      baseName, numFilaments = splitLine[1], 1
    
    for n in range(numFilaments):
      name = '%s[%d]' % (baseName, n)
      if name in self._filamentNames:
        raise IOError('%s already created' % name)
      newSeg = self._addSegment(name)
      self._filamentNames.append(name)
      filamentIndex = int(name.split('[')[1].split(']')[0])
      self._filaments[filamentIndex] = newSeg


  def _connectFilaments(self):
    """
    Loop through requested filament connections.
    For each connection connect two filaments together by joining the nodes at
      their ends. Note that this removes a node for each connection
    """
    
    def _getSegmentFromFilament(_filament):
      if not ('[' in _filament and ']' in _filament):
        _filament += '[0]'
      _segment = self.segments[self._filamentNames.index(_filament)]
      return _segment      
    
    while self._connections:
      _connection = self._connections.pop()
      
      # get the filaments and locations
      _location0 = _connection['location2']
      _filament0 = _connection['filament2']
      _segment0 = _getSegmentFromFilament(_filament0)
      print(_segment0.nodes)

      _location1 = _connection['location1']
      _filament1 = _connection['filament1']
      _segment1 = _getSegmentFromFilament(_filament1)
      print(_segment1.nodes)
      
      self._connectSegments(_segment0, _location0, _segment1, _location1)
    
    self.nodes = [n for n in self.nodes if n not in self._removeNodes]
    self._removeNodes = set()
  
  def getFilamentIndex(self, seg):
    """
    Return index to filament from original .hoc file
    """
    n = int(len(seg.compartments) / 2)
    c = seg.compartments[n]
    filamentIndex = None
    for tag in c.tags:
      try:
        newFilamentIndex = int(tag.split('[')[1].split(']')[0])
        if filamentIndex is not None and self._warnRepeatFilaments:
          print('Warning, one segment has multiple filaments: '
                + ' filament%d and filament%d' %
                (filamentIndex, newFilamentIndex))
          self._warnRepeatFilaments = False
        filamentIndex = newFilamentIndex
      except:
        pass
    return filamentIndex
  
  def getFilament(self, index):
    """
    return a segment based upon filament number
    """
    return self._filaments[index]
      


###############################################################################
def _parseArguments():
  arguments = sys.argv
  
  if len(arguments) != 2:
    print(_usageStr)
    raise TypeError('Incorrect number of arguments.')
  
  geoFile = arguments[1]
  return geoFile


def suggestProps(geometry, tau_m=215.2, tau_1=29.0, R_0=2.1, R_in=9.7):
  # R in MOhm, Tau in ms
  Cm = 1.0 # uF/cm^2, gospel
  Rm = 1.0e-3 * tau_m / Cm  # MOhm cm^2
  g1 = 1.0e-6 / Rm # S/cm^2
  RTau1 = 1.0e-3 * tau_1 / Cm
  g2 = 1.0e-6 / RTau1
  
  somaArea = geometry._somaSeg.surfaceArea * 1.0e-2
  cellArea = geometry.surfaceArea * 1.0e-2
  
  g3 = 1.0e-6 / R_0 / somaArea
  g4 = 1.0e-6 / R_0 / cellArea
  g5 = 1.0e-6 / R_in / somaArea
  g6 = 1.0e-6 / R_in / cellArea
  
  print(g1, g2, g3, g4, g5, g6)
  
  """
  L = math.pi / math.sqrt(tau_m / tau_1 - 1) #unitless ratio of length / lambda
  RInf = R_in * math.tanh(L)
  print('Rm = %g MOhm cm^2' % Rm)
  print('L = %g' % L)
  print('RInf = %g MOhm' % RInf)
  # get d (equivalent diameter in cm) somehow...
  # maybe guess it's the smallest diameter in the primary neurite?
  somaIndex, somaPos = geometry.getSomaIndex()
  d = 2 * geometry.segments[somaIndex].minRadius * 1.0e-4 # cm
  
  Ri = (RInf * math.pi / 2 * d**1.5)**2 / Rm  # MOhm cm
  Ra = 1.0e6 * Ri # Ohm cm
  print('cm = %g uF/cm^2, gLeak = %g S/cm^2, Ra = %g Ohm cm'
        % (Cm, g, Ra))
  """
  

###############################################################################
def demoRead(geoFile):
  ### Read in geometry file and pre-compute various quantities
  geometry = HocGeometry(geoFile)
  
  ### This section gets indices to the Axon, Soma, and neurite tips,
  ### then measures distance from Axon to Soma, and from Axon to a random
  ### neurite tip.
  axonInds, axonTipPos = geometry.getAxonIndices()
  print('Axon tip length = %g' % geometry.segments[axonInds[0]].length)
  print('Axon tip pos = %f' % axonTipPos[0])
  pDF = PathDistanceFinder(geometry, axonInds[0], axonTipPos[0])
  somaInd, somaPos = geometry.getSomaIndex()
  print('Path distance from Axon tip to Soma = %g' %
        pDF.distanceTo(somaInd, somaPos))
  
  import random
  random.seed
  
  neuriteTipInd, neuriteTipPos = geometry.getTipIndices()
  whichTip = random.randint(0, len(neuriteTipInd) - 1)
  print('Path distance from Axon tip to random neurite tip = %g' %
        pDF.distanceTo(neuriteTipInd[whichTip], neuriteTipPos[whichTip]))
  
  if os.access('steady_voltages.pickle', os.R_OK):
    import cPickle
    from math import sqrt, log
    from matplotlib import pyplot

    pDF = PathDistanceFinder(geometry, somaInd)
    dSeg = [pDF.distanceTo(s) for s in geometry.segments]
    with open('steady_voltages.pickle', 'r') as fIn:
      vSteady = cPickle.loads(fIn.read())
    eLengths = pDF.getElectrotonicLengths(vSteady)
    logELengths = [log(eL) for eL in eLengths]
    dist = [pDF.distanceTo(s) for s in range(len(vSteady))]
    diam = [sqrt(s.avgRadius) for s in geometry.segments]

### plots

    pyplot.figure()
    pyplot.plot(dist, vSteady, 'r.')
    pyplot.figure()
    pyplot.plot(dist, logELengths, 'b.')
    pyplot.figure()
    pyplot.plot(diam, logELengths, 'b.')
    pyplot.show()

  
  suggestProps(geometry)
  
  ### Display summary info
  geometry.displaySummary()


###############################################################################
if __name__ == "__main__":
  # get the geometry file
  geoFile = _parseArguments()
  # run a demo of capabilities
  demoRead(geoFile)
  #exit
  sys.exit(0)
