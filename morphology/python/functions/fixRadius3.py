# correct illegal radius version 3 -- modeled from ted's code

from NeuronGeometry import *  # at least need Geometry
from sys import argv
script, old_filename, new_filename = argv

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
   
def _parseHocGeometryLine(self, line):
    """
    Read a line from hoc file specifying geometry, and update geometryInfo
    appropriately.
    openFilament = name of filament if in a declaration block, otherwise = None
    """
    splitLine = line.split(None) # removes whitespace chars (returns, tabs, etc)
    if not splitLine:
      return
    
    if self._openFilament:
      pass #self._parseDefineFilament(line)
    elif splitLine[0] == 'connect':
      pass #self._addConnection(splitLine)
    elif splitLine[0] == 'create':
      pass #self._createFilaments(splitLine)
    elif splitLine[0] == 'neuron_name':
      pass #self.name = splitLine[-1]
    elif splitLine[0].lower() == "range":
      pass #if len(splitLine) < 7:
        #raise IOError(\
        # 'range should be of form "range minX maxX minY maxY minZ maxZ"')
      #self.minRange = tuple([float(x) for x in splitLine[1:6:2]])
      #self.maxRange = tuple([float(x) for x in splitLine[2:7:2]])
    elif splitLine[0] in self._filamentNames:
      pass #self._openFilament = splitLine[0]
    elif splitLine[0]+'[0]' in self._filamentNames:
      pass #self._openFilament = splitLine[0]+'[0]'
      
      
