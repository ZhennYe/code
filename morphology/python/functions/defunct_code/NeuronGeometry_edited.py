#!/usr/bin/python



_usageStr=\
"""usage: neuron_readExportedGeometry.py geoFile
  get dictionary object describing neuron model geometry info by reading file
"""



import os
import scipy
from scipy import special
from collections import deque
import matplotlib.pyplot as pyplot
from math import log




"""
Geometry class public methods: (self is always first argument)
 setFileName(_fileName)
 numCompartments()
 readGeometry()  pure virtual
 displaySummary()
 findBranches()
 checkConnectivity()
 schollAnalysis()
"""

def cumsum(_values, _start=0.0):
  """
  Return generator to running cumulative sum of values
  """
  for _v in _values:
    _start += _v
    yield _start


"""
class PathDistanceFinder
Finds network distances from one segment/branch at specified position
Can report distance to another segment/branch at specified position via
  .distanceTo()
Can compute electrotonic lengths from a list of voltages via
  .getElectrotonicLengths()
"""
class PathDistanceFinder(object):
  def __init__(self, geometry, segment, pos=0.5):
    self.geometry = geometry
    if type(segment) == int:
      segment = geometry.segments[segment]
      self.network = geometry.segments
    elif segment in geometry.segments:
      self.network = geometry.segments
    elif segment in geometry.branches:
      self.network = geometry.branches
    else:
      raise TypeError('Start segment must be an index to geometry.segments, or'
                      ' an object from geometry.segments or geometry.branches')
    
    self.startSegment = segment
    self.startPos = pos
    self.distances, self.branchOrders = self._computeDistances()
    
  
  def distanceTo(self, segment, pos=0.5):
    if type(segment) == int:
      segment = self.network[segment]
    
    if segment == self.startSegment:
      return segment.length * abs(pos - self.startPos)
    else:
      try:
        pathD = self.distances[segment]
        return pathD[0] + pathD[1] * (pos - 0.5)
      except KeyError:
        raise KeyError('Segment object is not reachable from the network')
  
  
  def getElectrotonicLengths(self, steadyVoltages):
    return [self._getElectrotonicLength(ind, seg, steadyVoltages)
            for ind, seg in enumerate(self.network)]
  
  
  def _getElectrotonicLength(self, index, segment, steadyVoltages):
    # compute the electro
    dSeg = self.distanceTo(segment)
    vSeg = steadyVoltages[index]
    segOrder = self.branchOrders[segment]

    # use only one other neighbor, pick the closest one that is of a different
    # branch order
    
    neighbor = min((s for s in segment.neighborsGeneral.keys()
                    if self.branchOrders[s] != segOrder),
                   key=lambda n: abs(self.distanceTo(n) - dSeg))
    dNeighbor = self.distanceTo(neighbor)
    vNeighbor = steadyVoltages[self.network.index(neighbor)]
    eLength = (dNeighbor - dSeg) / log(vSeg / vNeighbor)
    if eLength < 0:
      nIndex = self.network.index(neighbor)
      nOrder = self.branchOrders[neighbor]
      print('eLength=%g: ind=%d, nInd=%d, d=%g, nd=%g, v=%g, nV=%g, '
            % (eLength, index, nIndex, dSeg, dNeighbor, vSeg, vNeighbor)
            + 'order=%d, nOrder=%d' % (segOrder, nOrder))
    return eLength
      
  
  def _computeDistances(self):
    # use Dijkstra's algorithm to find path distance from start to rest of
    # neuron.
    # Keep track of effect of startPos (starting position in startSegment)
    # Also keep track of effect of pos of each final segment
    distances = { self.startSegment : (0.0, 0.0) }
    branchOrders = { self.startSegment : 0 }
    openSegs = set()
    for neighbor, (startPos, neighborPos, node) in \
        self.startSegment.neighborsGeneral.items():
      if startPos == 0:
        # neighbor connects at startSegment '0' side
        pathD = ( self.startSegment.length * self.startPos
                 + neighbor.length * 0.5)
      else:
        # neighbor connects at startSegment '1' side
        pathD = ( self.startSegment.length * (1.0 - self.startPos)
                 + neighbor.length * 0.5)
      if neighborPos == 0:
        # startSegment connects at neighbor '0' side
        neighborScale = neighbor.length
      else:
        # startSegment connects at neighbor '1' side
        neighborScale = -neighbor.length
      
      distances[neighbor] = (pathD, neighborScale)
      openSegs.add(neighbor)
      branchOrders[neighbor] = 1
    
    while openSegs:
      currentSeg = openSegs.pop()
      currentD = distances[currentSeg][0]
      currentL = currentSeg.length
      for neighbor, (currentPos, neighborPos, node) in \
          currentSeg.neighborsGeneral.items():
        pathD = currentD + (currentL + neighbor.length) / 2
        if neighbor not in distances or distances[neighbor][0] > pathD:
          if neighbor in distances:
            nIndex = self.network.index(neighbor)
            oldIndex = self.network.index(currentSeg)
            print('Warning, loop detected with connection from index '
                  '%d to index %d' % (oldIndex, nIndex))
          if neighborPos == 0:
            # currentSeg connects at neighbor '0' side
            distances[neighbor] = (pathD, neighbor.length)
          else:
            # currentSeg connects at neighbor '1' side
            distances[neighbor] = (pathD, -neighbor.length)
          openSegs.add(neighbor)
          branchOrders[neighbor] = branchOrders[currentSeg] + 1
    
    return distances, branchOrders


class Geometry:
  def __init__(self, _fileName = None):
    # who knows, do something?
    self.path = None
    self.fileName = None
    self.name = None
    
    # store the geometry info here
    self.nodes = []
    self.segments = []
    self.branches = []
    self.compartments = []
    self.branchOrders = None
    self.tags = {'*' : 0}
    
    # helper sets for efficient deleting
    self._removeNodes = set()
    self._removeSegments = set()
    
    self._soma = None
    self._somaSeg = None
    self._axons = None
    
    self.surfaceArea = 0.0   # mm^2
    self.volume = 0.0        # mm^3

    self.minRange = [float('nan'), float('nan'), float('nan')]
    self.maxRange = [float('nan'), float('nan'), float('nan')]
    
    if _fileName is not None:
      self.setFileName(_fileName)
      self.readGeometry()
  
  
  def setFileName(self, _fileName):
    self.path = os.path.dirname(os.path.abspath(_fileName))
    self.fileName = _fileName
    self.name = os.path.basename(_fileName).split('.')[0]
  
  def numCompartments(self):
    return len(self.compartments)
  
  
  def readGeometry(self):
    raise RuntimeError( \
      'Geometry must be a subclass that knows how to read files')  
  
  
  def displaySummary(self):
    """
    Display summary statistics of neuron geometry
    """
    print("total number of nodes: %d" % len(self.nodes))
    print("total number of compartments: %d" % len(self.compartments))
    print("total number of segments: %d" % len(self.segments))

    _subGraphs = self.checkConnectivity()
    _numSubgraphs = len(_subGraphs)
    if _numSubgraphs == 1:
      print("number of subgraphs: %d" % 1)
    else:
      _numComps = [sum([len(s.compartments) for s in g]) for g in _subGraphs]
      print("number of subgraphs: %d %s" % (_numSubgraphs, str(_numComps)))
      _subGraphs = self.checkConnectivity(removeDisconnected=True)
      print("  ...removed all but largest subgraph")

    print("number of connected nodes: %d" % len(self.nodes))
    print("number of connected compartments: %d" % len(self.compartments))
    print("number of connected segments: %d" % len(self.segments))
    
    if not self.branches:
      self.findBranches()
    print("number of branches: %d" % len(self.branches))
    soma = self.findSoma()[0]
    somaArea = sum(c.surfaceArea for c in soma.compartments \
                   if 'Soma' in c.tags)
    print('Soma Area = %g mm^2' % somaArea)
    print('Found %d axon%s' % (len(self._axons), 's'*(len(self._axons)!=1)))
    print("volume: %g mm^3" % self.volume)
    print("surface area: %s mm^2" % self.surfaceArea)
    self.calcBranchOrder(doPlot=False)
    self.schollAnalysis(straightenNeurites=True)
    self.mergeBranchesByOrder()
    self.mergeBranchesByOrder()
    pyplot.show()            # plot
    
  
  
  def findBranches(self):
    """
    Break up geometry into segments defined by branch points, starting at the
    soma
    """
    (_somaBranch, _somaNeighbors0, _somaNeighbors1) = self.findSoma()
    _somaSegs = {_c.segment for _c in _somaBranch.compartments}
    
    self.branches = [_somaBranch];
    _openBranches = [(_somaBranch, 0, _somaNeighbors0), \
                     (_somaBranch, 1, _somaNeighbors1)]
    _openSegs = set(self.segments).difference(_somaSegs)
    
    while _openBranches:
      # check an open branch to see if it has any neighbors branching off
      (_check, _side, _segNeighbors) = _openBranches.pop()
      
      # find neighbors on specified _side of _check
      _neighbors = {_check: _side}
      _checkNode = _check.nodes[-_side]
      for s in _segNeighbors:
        if s not in _openSegs:
          continue
        # for each neighboring segment, find the branch it's in
        (_branch, _neighbors0, _neighbors1) = self._getBranch(s)
        
        # add that branch to geometry
        self.branches.append(_branch)
        
        # remove the segments in _branch from _openSegs
        _openSegs.difference_update(\
          {_c.segment for _c in _branch.compartments})
        
        # add _branch to dic of common neighbors, at appropriate side
        # and add to openBranches with appropriate neighbors
        if _branch.nodes[-1] == _checkNode:
          if _branch.nodes[0] == _checkNode:
            print('found a loop! branch %s' %_branch.name)
            # _branch is a loop with both ends connected to _check
            raise RuntimeError("Can't handle loops right now")
            _neighbors[_branch] = 2
          else:
            # side 1 of _branch connects to _check
            assert _branch.nodes[-1] == _checkNode, "Node mismatch"
            _neighbors[_branch] = 1
            # side 0 is still open
            _openBranches.append((_branch, 0, _neighbors0))
        else:
          # side 0 of _branch connects to _check
          # print('line 302 %s' %_branch.name)
          assert _branch.nodes[0] == _checkNode, "Node mismatch"
          _neighbors[_branch] = 0
          # side 1 is still open
          _openBranches.append((_branch, 1, _neighbors1))
      
      # update the neighbors of _check and all the new branches
      while _neighbors:
        n1, n1Side = _neighbors.popitem()
        n1.neighbors.update(_neighbors.keys())
        # do weird nSide tests so that loops (side == 2) update correctly
        if n1Side != 1:
          n1.neighbors0.update(_neighbors.keys())
        if n1Side != 0:
          n1.neighbors1.update(_neighbors.keys())
        for n2, n2Side in _neighbors.iteritems():
          n2.neighbors.add(n1)
          if n2Side != 1:
            n2.neighbors0.add(n1)
          if n2Side != 0:
            n2.neighbors1.add(n1)
          n1.neighborsGeneral[n2] = (n1Side, n2Side, _checkNode)
          n2.neighborsGeneral[n1] = (n2Side, n1Side, _checkNode)
      
    for b in self.branches:
      assert b.neighbors == set(b.neighborsGeneral.keys()), \
        "neighborsGeneral does not match neighbors"
      for n in b.neighborsGeneral:
        assert b in n.neighborsGeneral, "Neighbors not symmetric"
        
  
  def _plotBranchStat(self, branchStat, yLabel, title, \
                      fontSize=22, barWidth=0.25):
    ### plot collected statistic along with number of branches
    ### branchStat should be a dictionary with:
    ###  -each key is a branch order (an integer)
    ###  -each item is a list of y-values (the list for all branches with
    ###   that branch order)
    order = branchStat.keys()
    order.sort()
    y = [branchStat[o] for o in order]
    x = list(range(len(order)))
    orderStr = [str(o) for o in order]
    numBranches = [len(y_n) for y_n in y]
    
    # make new figure
    fig = pyplot.figure()
    # plot number of branches as bar plot
    ax1 = pyplot.gca()
    pyplot.bar(x, numBranches, width=barWidth, color='g')
    pyplot.ylabel('# branches', fontsize=fontSize)
    pyplot.xlabel('Branch Order', fontsize=fontSize)
    pyplot.xticks(x, orderStr)
    
    # plot y statistics as a box and whisker plot
    positions = [x_n - barWidth/2.0 for x_n in x]
    ax2 = pyplot.twinx()
    pyplot.boxplot(y, positions=positions, widths=barWidth)
    pyplot.title(title, fontsize=fontSize)
    pyplot.ylabel(yLabel, fontsize=fontSize)
    pyplot.xlabel('Branch Order', fontsize=fontSize)
    pyplot.xticks(x, orderStr)
    
    # set the numBranches y-axis and labels on right, main on left
    ax1.yaxis.tick_right()
    ax1.yaxis.set_label_position('right')
    ax2.yaxis.tick_left()
    ax2.yaxis.set_label_position('left')
    return fig


  def _plotBranchOrderStatistics(self):
    ### Visualize various statistics dependent upon branch order

    # collect data
    branchRadius = {}
    branchLength = {}
    for branch in self.branches:
      order = self.branchOrders[branch][0]
      if order not in branchRadius:
        branchRadius[order] = [branch.maxRadius]
        branchLength[order] = [branch.length]
      else:
        branchRadius[order].append(branch.avgRadius)
        branchLength[order].append(branch.length)

    self._plotBranchStat(branchRadius, \
                         'Radius (um)', 'Branch Order vs. Radius')
    self._plotBranchStat(branchLength, \
                         'Length (um)', 'Branch Order vs Length')
    
  
  def calcBranchOrder(self, doPlot=True):
    self.calcReverseBranchOrder(doPlot=doPlot)
  
  
  def calcForewardBranchOrder(self, doPlot=True, printAxonInfo=False):
    """
    Find the principle neurites (assign them order 0) and compute branch order 
    1 + number of nodes between a given branch and the nearest principle
    neurite
    """
    # branchOrders is a dictionary object
    #  -each branch is a key in branchOrders
    #  -the value is a tuple (branch order, path from soma to branch)
    #     (int, [list of branches, in visitation order])
    branchOrders = {branch : (float('inf'), []) for branch in self.branches}
    
    soma = self.findSoma()[0]
    branchOrders[soma] = (0, [soma])
    
    openPaths = deque([branchOrders[soma]])
    while openPaths:
      order, path = openPaths.pop()
      order += 1
      branch = path[-1]
      
      pathNeighbors = [n for n in branch.neighborsGeneral if n not in path]
      for n in pathNeighbors:
        oldOrder = branchOrders[n][0]
        if order < oldOrder:
          # new path is the shortest so far (in terms of branches visited)
          newPath = [b for b in path]
          newPath.append(n)
          branchOrders[n] = (order, newPath)
          openPaths.append(branchOrders[n])
    
    self.branchOrders = branchOrders
    for branch in self.branches:
      branch.branchOrder = branchOrders[branch][0]
    
    if doPlot:
      self._plotBranchOrderStatistics()
    
    if printAxonInfo:
      axons = self.findAxons()
      for axon in axons:
        print("Branch order of axon is %d" % branchOrders[axon][0])
  
  
  def calcReverseBranchOrder(self, doPlot=True):
    """
    Find the principle neurites (assign them order 0) and compute branch order 
    1 + number of nodes between a given branch and the nearest principle
    neurite
    """
    
    # branchOrders is a dictionary object
    #  -each branch is a key in branchOrders
    #  -the value is a tuple (branch order, path from soma to branch)
    #     (int, [list of branches, in visitation order])
    if not self.branches:
      self.findBranches()
    branchOrders = {branch : (0, [branch]) for branch in self.branches}
    visited = {branch : False for branch in self.branches}
    openPaths = deque()
    for branch in self.branches:
      if branch.isTerminal and 'Soma' not in branch.tags:
        openPaths.append(branch)
        visited[branch] = True
    
    while openPaths:
      branch = openPaths.pop()
      
      newOpen = [n for n in branch.neighborsGeneral if not visited[n]]
      if len(newOpen) == 0:
        # this path is finished
        continue
      elif len(newOpen) > 1:
        # this path must wait to be resolved
        openPaths.appendleft(branch)
        continue
      
      # this path can continue
      newOpen = newOpen[0]
      visited[newOpen] = True
      oldOpen = [n for n in newOpen.neighborsGeneral if visited[n]]
      maxOrder = -1
      newPath = []
      for n in oldOpen:
        oldOrder, oldPath = branchOrders[n]
        if oldOrder > maxOrder:
          maxOrder = oldOrder
          newPath = oldPath
      
      newOrder = maxOrder + 1
      newPath.append(newOpen)
      branchOrders[newOpen] = (newOrder, newPath)
      openPaths.appendleft(newOpen)
    
    self.branchOrders = branchOrders
    for branch in self.branches:
      branch.branchOrder = branchOrders[branch][0]
    
    if doPlot:
      self._plotBranchOrderStatistics()
    
    soma = self.findSoma()[0]
    print('Soma branch order: %s' % str(soma.branchOrder))
  
  
  def checkConnectivity(self, removeDisconnected=False, checkObjects=None):
    """
    Compute the number of connected subgraphs and the number of compartments in
    each one. Return the list of _numSubgraphCompartments
    """
    
    if checkObjects is None:
      checkObjects = self.segments
    
    def _getConnectedObjects(_connected, _checkObjs, _subGraph):
      """
      Choose a compartment from connected, pop it, and add all its neighbors
      (that haven't yet been checked) to connected
      """
      _neighbors = _connected.pop().neighborsGeneral
      for _neighbor in _neighbors:
        if _neighbor in _checkObjs:
          _connected.add(_neighbor)
          _subGraph.add(_neighbor)
          _checkObjs.remove(_neighbor)
      
    
    _subGraphs = []
    _checkObjs = {obj for obj in checkObjects}
    while _checkObjs:
      # start checking new subgraph
      _connected = set()
      _connected.add(_checkObjs.pop())
      _subGraph = {_o for _o in _connected}
      
      while _connected:
        _getConnectedObjects(_connected, _checkObjs, _subGraph)
      
      _subGraphs.append(_subGraph)
    
    if isinstance(checkObjects[0], Segment):
      _checkType = 'Segment'
      _subGraphs.sort(key=lambda x: -sum([len(y.compartments) for y in x]))
    elif isinstance(checkObjects[0], Compartment):
      _checkType = 'Compartment'
      _subGraphs.sort(key=lambda x: -len(x))
    else:
      raise RuntimeError("Can't sort type: %s" % str(type(checkObjects[0])))
    
    if removeDisconnected and len(_subGraphs) > 1:
      print("removing disconnected subgraphs")
      _badSegs = set()
      _badComps = set()
      _badNodes = set()
      if _checkType == 'Segment':
        for n in range(len(_subGraphs) - 1):
          _subGraph = _subGraphs.pop(1)
          
          # find unwanted objects in the subgraph
          _badSegs.update(_subGraph)
          
          for _seg in _subGraph:
            _badComps.update(_seg.compartments)
            _badNodes.update(_seg.nodes)
      else:
        for n in range(len(_subGraphs) - 1):
          _subGraph = _subGraphs.pop(1)
          
          # find unwanted objects in the subgraph
          _badComps.update(_subGraph)
          
          for _comp in _subGraph:
            _badNodes.update(_comp.nodes)
            _badSegs.add(_comp.segment)

      self.segments[:] = \
        [_seg for _seg in self.segments if _seg not in _badSegs]
      self.compartments[:] = \
        [_comp for _comp in self.compartments if _comp not in _badComps]
      self.nodes[:] = [_node for _node in self.nodes if _node not in _badNodes]
    
    return _subGraphs


  def _plotSchollGraph(self, distances):
    """
    Plot the number of neurites at a given distance
    """
    
    # neuriteDistance starts at zero, and has two data points for each
    # distance: one with the previous (running) number of compartments, and one
    # with the change added in (running +1 or -1)
    runningNum = 0
    neuriteDistance = [0.0]
    numIntersections = [0]
    lastNeuriteDistance = 0.0
    for d in distances:
      if d[0] > lastNeuriteDistance:
        neuriteDistance.append(lastNeuriteDistance)
        numIntersections.append(runningNum)
        neuriteDistance.append(d[0])
        numIntersections.append(runningNum)
        lastNeuriteDistance = d[0]
      runningNum += d[1]
    neuriteDistance.append(lastNeuriteDistance)
    numIntersections.append(runningNum)
    
    fig = pyplot.figure()
    pyplot.plot(neuriteDistance, numIntersections, 'k-')
    pyplot.title('Scholl Analysis', fontsize=22)
    pyplot.xlabel('Distance from soma', fontsize=22)
    pyplot.ylabel('Number of compartments', fontsize=22)
    ax = pyplot.gca()
    for tick in ax.xaxis.get_major_ticks():
      tick.label1.set_fontsize(16)
    for tick in ax.yaxis.get_major_ticks():
      tick.label1.set_fontsize(16)
      
  
  def schollAnalysis(self, straightenNeurites=True):
    """
    Find the number of neurites that intersect a sphere of a given radius
    """
    # get the centroid of the soma, weighting each compartment's contribution
    # by volume
    soma = self.findSoma()[0]
    
    # define how distance from centroid to compartment is measured
    if straightenNeurites:
      # get distance traveled along neurites (e.g. as though neuron was
      # straightened out
      
      centroid = soma.centroidPosition(mandateTag='Soma')
      print("Soma centroid position at %g" % centroid)
      
      # compute distance from soma to each branch
      somaPaths = PathDistanceFinder(self, soma, centroid)
      
      # store results in an array
      distances = []
      for b in self.branches:
        d0, d1 = somaPaths.distanceTo(b, 0.0), somaPaths.distanceTo(b, 1.0)
        if d1 < d0:
          d0, d1 = d1, d0
        
        distances.append((d0, 1))
        distances.append((d1, -1))
      
    else:
      # get euclidean distance from soma centroid to each compartment
      # (must be done compartment by compartment, because branches curve)

      centroid = soma.centroid(mandateTag='Soma')
      print("Soma centroid at %s" % str(centroid))
      
      # define distance from centroid to compartment
      def _centroidDist(c):
        def _tupleDist(_t1, _t2):
          return scipy.sqrt( (_t1[0] - _t2[0])**2 + \
                            (_t1[1] - _t2[1])**2 + \
                            (_t1[2] - _t2[2])**2 )

        d0 = _tupleDist(centroid, (c.x0, c.y0, c.z0))
        d1 = _tupleDist(centroid, (c.x1, c.y1, c.z1))
        if d0 <= d1:
          return d0, d1
        else:
          return d1, d0
      
      # compute the distance from the soma centroid to each compartment that's
      # not in the soma
      distances = []
      for c in self.compartments:
        if 'Soma' in c.tags:
          continue
        d0, d1 = _centroidDist(c)
        distances.append((d0, 1))  
        distances.append((d1, -1))
    
    # sort the distances by increasing distance
    distances.sort(key=lambda x: x[0])
    
    self._plotSchollGraph(distances)
  
    
  def _addSegment(self, name, segList=None):
    """
    Add a new segment to the model
    """
    if name in self.tags:
      raise IOError('Tried to create segment with existing name/tag')

    newSeg = Segment(self)
    newSeg.name = name
    if name not in self.tags:
      self.tags[name] = 0
    
    
    if segList is None:
      segList = self.segments
    segList.append(newSeg)
    return newSeg
  
  
  def _addNode(self, segment, _x, _y, _z, _r1, _r2=None, _r3=None,
               _theta = 0.0, _phi=0.0):
    """
    Define and add a new node to the model within the specified segment
    """
    newNode = Node(_x, _y, _z, _r1, _r2, _r3, _theta, _phi)
    newNode.segments.append(segment)
    newNode.tags.update(segment.tags)
    newNode.tags.add(segment.name)
    self.nodes.append(newNode)
    segment.nodes.append(newNode)
    return newNode
  
  
  def _addCompartment(self, segment, _node0, _node1=None, append=False):
    """
    Define and add compartment to geometry within specified segment
    """
    if type(_node0) is int:
      # passed an index, get the node object
      _node0 = self.nodes[_node0]

    if _node1 is None:
      # define one-node compartment and add it to the segment
      newComp = OneNodeCompartment(_node0)
      _node0.compartments.append(newComp)
      if _node0 == segment.nodes[0]:
        segment.compartments.insert(0, newComp)
      else:
        assert(_node0 == segment.nodes[-1])
        segment.compartments.append(newComp)
    else:
      # define two-node compartment and add it to the segment
      if type(_node1) is int:
        _node1 = self.nodes[_node1]
      newComp = TwoNodeCompartment(_node0, _node1)
      _node0.compartments.append(newComp)
      _node1.compartments.append(newComp)
      if append or _node1 == segment.nodes[-1]:
        segment.compartments.append(newComp)
      else:
        _compInd = segment.nodes.index(_node0)
        if len(segment.compartments) and \
           isinstance(segment.compartments[0], OneNodeCompartment):
          _compInd += 1
        segment.compartments.insert(_compInd, newComp)
    
    # add segment information to compartment
    newComp.tags.update(segment.tags)
    newComp.tags.add(segment.name)
    newComp.segment = segment
    
    # add compartment to geometry
    self.compartments.append(newComp)
    
    # update tag counts
    self.tags['*'] += 1
    for tag in newComp.tags:
      self.tags[tag] += 1
    
    # update geometry totals
    self.surfaceArea += newComp.surfaceArea
    self.volume += newComp.volume
    return newComp
  
  
  def _connectSegments(self, _segment0, _location0, _segment1, _location1):
    """
    Connect two segments in the geometry, managing the connecting info in the
    segments and their nodes and compartments
    """
    def _samePos(_n1, _n2):
	  return _n1.x == _n2.x and _n1.y == _n2.y and _n1.z == _n2.z
    def _fixR(_n1, _n2):
	  if _n1.r1 != _n2.r1:
		r = max(_n1.r1, _n2.r2)
		_n1.r1, _n1.r2, _n1.r3, _n2.r1, _n2.r2, _n2.r3 = r, r, r, r, r,r
    # get appropriate node on _segment0, and it's existing neighbors
#    if _location0 == 0.0:
    _node00 = _segment0.nodes[0]
    _neighbors00 = _segment0.neighbors0
#    elif _location0 == 1.0:
    _node01 = _segment0.nodes[-1]
    _neighbors01 = _segment0.neighbors1
#    else:
#      raise IOError('Can only connect filaments at end')
        
    # get appropriate node on _segment1, and it's existing neighbors
#    if _location1 == 0.0:
    _node10 = _segment1.nodes[0]
    _neighbors10 = _segment1.neighbors0
#    elif _location1 == 1.0:
    _node11 = _segment1.nodes[-1]
    _neighbors11 = _segment1.neighbors1
#    else:
#      raise IOError('Can only connect filaments at end')
      
    # force connections for all known connected nodes
    if _samePos(_node00, _node10):
      _fixR(_node00, _node10)
      _node0 = _node00
      _node1 = _node10
      _neighbors0 = _neighbors00
      _neighbors1 = _neighbors10
    elif _samePos(_node00, _node11):
      _fixR(_node00, _node11)
      _node0 = _node00
      _node1 = _node11
      _neighbors0 = _neighbors00
      _neighbors1 = _neighbors11
    elif _samePos(_node01, _node10):
      _fixR(_node01, _node10)
      _node0 = _node01
      _node1 = _node10
      _neighbors0 = _neighbors01
      _neighbors1 = _neighbors10
    elif _samePos(_node01, _node11):
      _fixR(_node01, _node11)
      _node0 = _node01
      _node1 = _node11
      _neighbors0 = _neighbors01
      _neighbors1 = _neighbors11
    else:
      # check to make sure the two nodes are identical
      t0 = {t for c in _segment0.compartments for t in c.tags}
      print(t0)
      t1 = {t for c in _segment1.compartments for t in c.tags}
      print(t1)
      print(_node00.x, _node00.y, _node00.z, _node00.r1)
      print(_node01.x, _node01.y, _node01.z, _node01.r1)
      print(_node10.x, _node10.y, _node10.z, _node10.r1)
      print(_node11.x, _node11.y, _node11.z, _node11.r1)
      raise IOError(\
       'Tried to connect filaments but connecting nodes were different')

    # add segment0's neighbors to segment1 (and vice versa) if they join
    # at the same location
    for n, (loc0, locN, node0N) in _segment0.neighborsGeneral.items():
      if loc0 == _location0:
        _segment1.neighborsGeneral[n] = (_location1, locN, _node0)
        n.neighborsGeneral[_segment1] = (locN, _location1, _node0)
    # add segment1 to segment0's neighobrs
    _segment0.neighborsGeneral[_segment1] = (_location0, _location1, _node0)
    # add segment0's neighbors to segment1 (and vice versa) if they join
    # at the same location
    for n, (loc1, locN, node1N) in _segment1.neighborsGeneral.items():
      if loc1 == _location1:
        _segment0.neighborsGeneral[n] = (_location0, locN, _node0)
        n.neighborsGeneral[_segment0] = (locN, _location0, _node0)
    # add segment0 to segment1's neighobrs
    _segment1.neighborsGeneral[_segment0] = (_location1, _location0, _node0)
    
    # define a procedure to update sets of neighbors appropriately
    def _updateSet(_s1, _s2, _n):
      _s1.update(_x for _x in _s2 if _x != _n)
    
    # create a list of common neighbors
    _commonNeighbors = {_segment0, _segment1}
    _commonNeighbors.update(_neighbors0)
    _commonNeighbors.update(_neighbors1)
    
    # update the neighbors appropriately
    for _n in _neighbors0:
      _updateSet(_n.neighbors, _commonNeighbors, _n)
      if _segment0 in _n.neighbors0:
        _updateSet(_n.neighbors0, _commonNeighbors, _n)
      else:
        assert(_segment0 in _n.neighbors1)
        _updateSet(_n.neighbors1, _commonNeighbors, _n)
    for _n in _neighbors1:
      _updateSet(_n.neighbors, _commonNeighbors, _n)
      if _segment1 in _n.neighbors0:
        _updateSet(_n.neighbors0, _commonNeighbors, _n)
      else:
        assert(_segment1 in _n.neighbors1)
        _updateSet(_n.neighbors1, _commonNeighbors, _n)
    _updateSet(_segment0.neighbors, _commonNeighbors, _segment0)
    _updateSet(_neighbors0, _commonNeighbors, _segment0)
    _updateSet(_segment1.neighbors, _commonNeighbors, _segment1)
    _updateSet(_neighbors1, _commonNeighbors, _segment1)

    # swap _node1 out with _node0 in all segments that touch _node1
    _node0.segments.extend(_node1.segments)
    for _seg in _node1.segments:
      if _seg.nodes[0] == _node1:
        _seg.nodes[0] = _node0
      else:
        _seg.nodes[-1] = _node0
    
    # swap _node1 out with _node0 all the compartments that touch _node1
    _node0.compartments.extend(_node1.compartments)
    for _comp in _node1.compartments:
      if _comp.node0 == _node1:
        _comp.node0 = _node0
      else:
        _comp.node1 = _node0
    
    # remove _node1 from geometry
    self._removeNodes.add(_node1)
  
  
  def _mergeSegments(self, segmentA, segmentB, _segList):
    # Merge segmentA and segmentB into one segment, preserving their neighbor
    # information
    #
    assert segmentA in _segList
    assert segmentB in _segList
    assert segmentB in segmentA.neighborsGeneral and \
           segmentA in segmentB.neighborsGeneral, \
          "Merged segments must be neighbors"
    
    # find the connection location
    (locationA, locationB, nodeAB) = segmentA.neighborsGeneral[segmentB]
    assert locationA in [0.0, 1.0] and locationB in [0.0, 1.0], \
           "Tried to merge with locationA=%g, locationB=%g, but both locations must be an end point" \
           % (locationA, locationB)

    lenA = segmentA.length
    lenB = segmentB.length
    newLen = lenA + lenB
    
    if locationA == 1.0:
      if locationB == 0.0:
        # define function to get location on the new segment
        def _getNewLoc(locA=locationA, locB=locationB):
            return (locA * lenA + locB * lenB) / newLen
        # add segmentB compartments + nodes to segmentA
        segmentA.compartments.extend(segmentB.compartments)
        segmentA.nodes[-1] = segmentB.nodes[-1]
      else: #locationB == 1.0:
        def _getNewLoc(locA=locationA, locB=locationB):
            return (locA * lenA + (1.0 - locB) * lenB) / newLen
        # add segmentB compartments + nodes to segmentA
        segmentA.compartments.extend(reversed(segmentB.compartments))
        segmentA.nodes[-1] = segmentB.nodes[0]
    else:
      if locationB == 0.0:
        def _getNewLoc(locA=locationA, locB=locationB):
            return (locA * lenA + (1.0 - locB) * lenB) / newLen
        # add segmentB compartments + nodes to segmentA
        for c in reversed(segmentB.compartments):
          segmentA.compartments.insert(0, c)
        segmentA.nodes[0] = segmentB.nodes[-1]
      else: #locationB == 1.0:
        def _getNewLoc(locA=locationA, locB=locationB):
            return (locA * lenA + locB * lenB) / newLen
        # add segmentB compartments + nodes to segmentA
        for c in segmentB.compartments:
          segmentA.compartments.insert(0, c)
        segmentA.nodes[0] = segmentB.nodes[0]
    
    # remove segmentB from segmentA's neighbors
    segmentA.neighbors.remove(segmentB)
    segmentA.neighbors0.discard(segmentB)
    segmentA.neighbors1.discard(segmentB)
    segmentA.neighborsGeneral.pop(segmentB)
    # remove segmentA from segmentB's neighbors
    segmentB.neighbors.remove(segmentA)
    segmentB.neighborsGeneral.pop(segmentA)
    
    # recompute the location of segmentA's old neighbors
    for n, (locA, locN, nodeAN) in segmentA.neighborsGeneral.iteritems():
      newLocA = _getNewLoc(locA=locA)
      segmentA.neighborsGeneral[n] = (newLocA, locN, nodeAN)
      n.neighborsGeneral[segmentA] = (locN, newLocA, nodeAN)
      # remove n from A.neighbors0 or neighbors1 as appropriate
      if locA == 0.0 and newLocA != 0.0:
        segmentA.neighbors0.remove(n)
      if locA == 1.0 and newLocA != 1.0:
        segmentA.neighbors1.remove(n)
    
    # replace segmentB with segmentA in other segments' neighbors
    #  and add those neighbors as new neighbors to segmentA
    for s in segmentB.neighborsGeneral.keys():
      # replace B by A in s.neighbors
      s.neighbors.remove(segmentB)
      s.neighbors.add(segmentA)
      # add s to A.neighbors
      segmentA.neighbors.add(s)
      
      # replace B by A in s.neighborsGeneral
      (locS, locB, nodeSB) = s.neighborsGeneral.pop(segmentB)
      locA = _getNewLoc(locB=locB)
      s.neighborsGeneral[segmentA] = (locS, locA, nodeSB)
      # add s to A.neighborsGeneral
      segmentA.neighborsGeneral[s] = (locA, locS, nodeSB)

      # replace B by A in s.neighbors0 or s.neighbors1 if appropriate
      if segmentB in s.neighbors0:
        s.neighbors0.remove(segmentB)
        s.neighbors0.add(segmentA)
      elif segmentB in s.neighbors1:
        s.neighbors1.remove(segmentB)
        s.neighbors1.add(segmentA)
      # add s to A.neighbors0 or A.neighbors1 if appropriate
      if locA == 0.0:
        segmentA.neighbors0.add(s)
      elif locA == 1.0:
        segmentA.neighbors1.add(s)
    
    # update segmentB's tags into segmentA
    segmentA.tags.update(segmentB.tags)
    
    # remove segmentB from the list of segments
    #[SLOW]
    #_segList.remove(segmentB)
    self._removeSegments.add(segmentB)
  
  
  def mergeBranchesByOrder(self):
    # merge branches together if one of a branch's neighbors is clearly a
    # continuation of it (using branchOrder as method of determining
    # continuation)
    
    if self.branchOrders is None:
      self.calcBranchOrder(doPlot=False)
    
    def _getMergeBranch(currentBranch):
      # find the branch that should be merged with current branch
      # and what the order of the merged branch should be
      #if 'Soma' in currentBranch.tags:
      #  return None, currentBranch.branchOrder
      orders0 = {}
      orders1 = {}
      
      mergeBranch = None
      for n, (locC, locN, nodeC) in currentBranch.neighborsGeneral.iteritems():
        if locC not in [0, 1] or locN not in [0, 1]:
          # only merge at branch end points
          continue
        
        if n.branchOrder >= currentBranch.branchOrder:
          if mergeBranch is not None or \
             n.branchOrder != currentBranch.branchOrder + 1:
            # too many potential merges => this branch can't merge (but maybe
            # other branches can merge at this location)
            return None, currentBranch.branchOrder
          else:
            # potential merge
            mergeBranch = n
      if mergeBranch is None:
        return mergeBranch, currentBranch.branchOrder
      else:
        return mergeBranch, mergeBranch.branchOrder
    
    visited = {b : False for b in self.branches}
    openBranches = {b for b in self.branches if b.branchOrder == 0}
    
    while openBranches:
      current = openBranches.pop()
      visited[current] = True
      mergeBranch, mergeOrder = _getMergeBranch(current)
      while mergeBranch is not None:
        openBranches.discard(mergeBranch)
        visited[mergeBranch] = True

        self._mergeSegments(mergeBranch, current, self.branches)
        self.branchOrders.pop(current)
        current = mergeBranch

        current.branchOrder = mergeOrder
        (oldOrder, path) = self.branchOrders[current]
        self.branchOrders[current] = (mergeOrder, path)
        
        mergeBranch, mergeOrder = _getMergeBranch(current)
      
      openBranches.update( \
        n for n in current.neighborsGeneral if not visited[n])

    self.branches = [b for b in self.branches if b not in self._removeSegments]
    self._removeSegments = set()
    self._plotBranchOrderStatistics()

  
  def _addOneNodeCompartments(self):
    """
    Add extra compartments to account for regions covered by nodes with only
    one compartment attached (the end of the soma is probably the most
    important)
    """
    for segment in self.segments:
      if not segment.nodes:
        # empty segment
        print("Warning, empty segment: %s" % segment.name)
      
      node0 = segment.nodes[0]
      if len(node0.compartments) == 1:
        self._addCompartment(segment, node0)
      
      node1 = segment.nodes[-1]
      if len(node1.compartments) == 1:
        self._addCompartment(segment, node1)
  
  
  def _getBranch(self, _start):
    """
    Find the branch (bounded by compartments with more than two neighbors)
    that starts with startCompartment. The end of each branch is a node with
    a number of attached compartments not equal to 2.
    NOTE: don't add _neighbors0 or _neighbors1 to _branch.neighbors, because
    they are *segment* neighbors, and _branch will want *branch* neighbors
    """
    _branch = Segment(self)
    
    # add startCompartment
    
    if isinstance(_start, Segment):
      _startSeg = _start
    elif isinstance(_start, Compartment):
      # passed compartment, get the segment it's a part of
      _startSeg = _start.segment
    
    _branch.compartments = _startSeg.compartments[:]
    _branch.nodes = _startSeg.nodes[:]
    _branch.tags = {_t for _t in _startSeg.tags}
    _branch.name = 'branch%d' % len(self.branches)
    
    _checkSeg = _startSeg
    _neighbors0 = _checkSeg.neighbors0
    while len(_neighbors0) == 1:
      _oldSeg = _checkSeg
      _checkSeg = iter(_neighbors0).next()
      _branch.tags.update(_checkSeg.tags)
      if _oldSeg in _checkSeg.neighbors1:
        # should be the usual case
        _branch.compartments[0:0] = _checkSeg.compartments[:]
        _branch.nodes[0:0] = _checkSeg.nodes[:-1]
        _neighbors0 = _checkSeg.neighbors0
      else:
        # segment is flipped
        _branch.compartments[0:0] = reversed(_checkSeg.compartments[:])
        _branch.nodes[0:0] = reversed(_checkSeg.nodes[1:])
        _neighbors0 = _checkSeg.neighbors1

    _checkSeg = _startSeg
    _neighbors1 = _checkSeg.neighbors1
    while len(_neighbors1) == 1:
      _oldSeg = _checkSeg
      print(_oldSeg.name)
      _checkSeg = iter(_neighbors1).next()
      _branch.tags.update(_checkSeg.tags)
      if _oldSeg in _checkSeg.neighbors0:
        # should be the usual case
        _branch.compartments.extend(_checkSeg.compartments[:])
        _branch.nodes.extend(_checkSeg.nodes[1:])
        _neighbors1 = _checkSeg.neighbors1
      else:
        # segment is flipped
        _branch.compartments.extend(reversed(_checkSeg.compartments[:]))
        _branch.nodes.extend(reversed(_checkSeg.nodes[:-1]))
        _neighbors1 = _checkSeg.neighbors0
    
    return (_branch, _neighbors0, _neighbors1)
  
  
  def findSoma(self):
    """
    Find the compartment with the largest diameter (which is presumably in the
    soma). Return the branch that contains this compartment.
    """
    if self._soma is not None:
      soma = self._soma
      seg0 = soma.compartments[0].segment
      neighbors0 = {s for s in soma.nodes[0].segments if s != seg0}
      seg1 = soma.compartments[-1].segment
      neighbors1 = {s for s in soma.nodes[-1].segments if s != seg1}
      return (soma, neighbors0, neighbors1)
    
    # locate the largest radius compartment -it should be the soma
    somaCenter = max(self.compartments, key=lambda c: c.maxRadius)
    
    # get the branch that contains the soma (plus extra stuff)
    branchResults = self._getBranch(somaCenter)
    self._soma = branchResults[0]
    # add Soma tag to soma branch, but not its compartments
    self._soma.tags.add('Soma')
    
    # find the segments and add the tag Soma (but not to their compartments)
    somaSegs = set(c.segment for c in self._soma.compartments)
    if len(somaSegs) == 1:
      self._somaSeg = somaSegs.pop()
      self._somaSeg.tags.add('Soma')
    else:
      print('WARNING: %d segments in soma' % len(somaSegs))
      for s in somaSegs:
        s.tags.add('Soma')
        if somaCenter in s.compartments:
          self._somaSeg = s

    # find a cutoff on radius that defines the Soma proper
    rCutoff = (self._soma.minRadius * self._soma.maxRadius**3)**0.25
    
    # find the index to the center of the Soma
    centerInd = self._soma.compartments.index(somaCenter)
    
    # apply the Soma tag to contiguous stretch of large compartments near
    # the center
    self.tags['Soma'] = 0
    for ind in range(centerInd, len(self._soma.compartments)):
      c = self._soma.compartments[ind]
      if c.avgRadius >= rCutoff:
        c.tags.add('Soma')
        self.tags['Soma'] += 1
      else:
        break
    for ind in range(centerInd - 1, -1, -1):
      c = self._soma.compartments[ind]
      if c.avgRadius >= rCutoff:
        c.tags.add('Soma')
        self.tags['Soma'] += 1
      else:
        break

    return branchResults
  
  
  def findAxons(self):
    """
    Locate the axon branch
    """
    if self._axons is not None:
      # already found the axons, return the results
      return self._axons
    
    # need the branches
    if not self.branches:
      self.findBranches()
    # need the Soma to be tagged
    self.findSoma()
    # need to know the range of the whole geometry
    self._findGeometryRange()
    
    def _edgeNode(_node):
      _radius = _node.avgRadius
      def _edgeCoord(_coord, _dim):
        return _coord - 2 * _radius < self.minRange[_dim] or \
               _coord + 2 * _radius > self.maxRange[_dim]
      return _edgeCoord(_node.x, 0) or \
             _edgeCoord(_node.y, 1) or \
             _edgeCoord(_node.z, 2)
    
    self._axons = set()
    for branch in self.branches:
      if 'Soma' in branch.tags:
        # it's the soma
        continue
        
      ###### manually tag branch as below
  #    if branch.name == 'branch1534':
  #      print('Tagged axon %f' % branch.centroidPosition)
  #      self._axons.add(branch)
      
      if branch.length < 15:
        continue
      if (len(branch.neighbors0) == 0 and _edgeNode(branch.nodes[0])) or \
         (len(branch.neighbors1) == 0 and _edgeNode(branch.nodes[-1])):
        branch.addTag('Axon')
        self._axons.add(branch)
        for c in branch.compartments:
          c.segment.tags.add('Axon')
          print('Added tag "axon"')
      
    return self._axons
  
  
  def _findGeometryRange(self):
    """
    Find the physical extent of the geometry in x,y,z
    """
    if not any(scipy.isnan(x) for x in self.minRange) and \
       not any(scipy.isnan(x) for x in self.maxRange):
      # the range has already been found/specified
      return
    
    self.minRange = [float('inf'), float('inf'), float('inf')]
    self.maxRange = [float('-inf'), float('-inf'), float('-inf')]
    def _updateRange(_nodeCoord, _rangeInd):
      if _nodeCoord < self.minRange[_rangeInd]:
        self.minRange[_rangeInd] = _nodeCoord
      if _nodeCoord > self.maxRange[_rangeInd]:
        self.maxRange[_rangeInd] = _nodeCoord
    for _node in self.nodes:
      _updateRange(_node.x, 0)
      _updateRange(_node.y, 1)
      _updateRange(_node.z, 2)



class Segment:
  def __init__(self, geometry):
    self.geometry = geometry
    
    self.name = None
    self.tags = set()
    
    self.compartments = []
    self.nodes = []
    self.neighbors = set()
    self.neighbors0 = set()
    self.neighbors1 = set()
    # neighborsGeneral has dict structure:
    #   key = Segment
    #   value = tuple:
    #     (connection location in this segment [0.0 - 1.0],
    #      connection location in neighbor [0.0 - 1.0],
    #      Node )
    # ultimately replace neighbors with neighborsGeneral,
    #            eliminate neighbors0, neighbors1
    self.neighborsGeneral = {}
    
    self.branchOrder = None


  def clear(self):
    if self.compartments:
      for c in self.compartments:
        for tag in c.tags:
          self.geometry.tags[tag] -= 1
      self.geometry.compartments = [c for c in self.geometry.compartments
                                    if c not in self.compartments]
      self.compartments = []
    if self.nodes:
      delNodes = []
      for n in self.nodes:
        n.segments.remove(self)
        if not n.segments:
          delNodes.append(n)
      self.geometry.nodes = [n for n in self.geometry.nodes
                             if n not in delNodes]
      self.nodes = []

  
  def addTag(self, newTag):
    """
    Add a new tag to the segment and all its compartments
    """
    if newTag not in self.geometry.tags:
      self.geometry.tags[newTag] = 0
    
    self.tags.add(newTag)
    for c in self.compartments:
      c.tags.add(newTag)
      self.geometry.tags[newTag] += 1
  
  @property
  def length(self):
    return sum([c.length for c in self.compartments])
  
  @property
  def surfaceArea(self):
    return sum([c.surfaceArea for c in self.compartments])
  
  @property
  def maxRadius(self):
    # compute maximum radius
    return max(c.maxRadius for c in self.compartments)
  
  @property
  def minRadius(self):
    # compute minimum radius
    return min(c.minRadius for c in self.compartments)
  
  @property
  def avgRadius(self):
    # compute average radius, weighted by volume
    return sum(c.avgRadius * c.volume for c in self.compartments) / \
           sum(c.volume for c in self.compartments)
  
  @property
  def volume(self):
    return sum(c.volume for c in self.compartments)
  
  @property
  def isTerminal(self):
    return not self.neighbors0 or not self.neighbors1
  
  def lengthPerArea(self, _x1, _x2 = 0.5):
    """
    Compute length per cross sectional area for segment
    """
    if _x2 < _x1:
      _temp = _x1
      _x1 = _x2
      _x2 = _temp
    
    _lengths = [c.length for c in self.compartments]
    _cumLengths = scipy.cumsum(_lengths)
    def _findComp(_l):
      for n in range(len(_lengths)):
        if _cumLengths[n] >= _l:
          _c = self.compartments[n]
          _x = 1.0 - (_cumLengths[n] - _l) / _c.length
          return (n, _c, _x)
   
    # find 1st compartment, and proportion of distance across c1
    (_compInd1, _c1, _cX1) = _findComp(_x1 * _cumLengths[-1])
    # find 2nd compartment, and proportion of distance across c2
    (_compInd2, _c2, _cX2) = _findComp(_x2 * _cumLengths[-1])
      
    # compute length per area across distance
    if _compInd2 == _compInd1:
      return _c1.lengthPerArea(_cX1, _cX2)
    else:
      lPA = _c1.lengthPerArea(_cX1, 1.0) + _c2.lengthPerArea(0.0, _cX2)
      _compInd2 -= 1
      while _compInd2 > _compInd1:
        lPA += self.compartments[_compInd2].lengthPerArea(0.0, 1.0)
        _compInd2 -= 1
      return lPA
  
  def centroid(self, mandateTag=None):
    # return the centroid of a Segment, weighted by volume
    def _weightedC(_c):
      # return the weighted centroid of a compartment
      return tuple(_t * _c.volume / v for _t in _c.centroid)

    if mandateTag is None:
      v = self.volume
      centroids = [_weightedC(c) for c in self.compartments]
    else:
      v = sum(c.volume for c in self.compartments if mandateTag in c.tags)
      centroids = [_weightedC(c) for c in self.compartments if \
                   mandateTag in c.tags]
    
    # sum up the centroids
    centroid = (0.0, 0.0, 0.0)
    for pos in centroids:
      centroid = tuple(a + b for a,b in zip(centroid, pos))
    
    return centroid
  
  def centroidPosition(self, mandateTag=None):
    # return the position of the centroid of a Segment, weighted by volume
    #   (in this case, position is a number from 0 to 1, denoting proportion of
    #    distance along the segment)

    if mandateTag is None:
      halfV = self.volume / 2
      segLen = self.length
      centroidLen = 0.0
      for c in self.compartments:
        if c.volume < halfV:
          halfV -= c.volume
          centroidLen += c.length
        else:
          cFrac = halfV / c.volume
          centroidLen += c.length * cFrac
          break
    else:
      v = sum(c.volume for c in self.compartments if mandateTag in c.tags)
      halfV = v / 2
      segLen = self.length
      centroidLen = 0.0
      for c in self.compartments:
        if mandateTag in c.tags:
          if c.volume < halfV:
            halfV -= c.volume
            centroidLen += c.length
          else:
            cFrac = halfV / c.volume
            centroidLen += c.length * cFrac
            break
        else:
          centroidLen += c.length
    
    # return the position of the centroid
    return centroidLen / segLen
   

class Node:
  def __init__(self, _x, _y, _z, _r1, \
               _r2=None, _r3=None, _theta=0.0, _phi=0.0):
    self.x = _x
    self.y = _y
    self.z = _z
    self.r1 = _r1
    if self.r1 <= 0.0:
      self.r1 = 0.09
    #  if self.r1 < 0:
    #    raise ValueError('Encountered negative radius')
    #  else:
    #    raise ValueError('Encountered radius=0')
    if _r2 is None:
      if _r3 is not None:
        raise ValueError(\
          'Specify 1 radius for spherical nodes, 3 for ellipsoidal nodes')
      self.r2 = _r1
      self.r3 = _r1
    else:
      self.r2 = _r2
      self.r3 = _r3
    
    self.theta = _theta # angle from z axis
    self.phi = _phi # angle of azimuth (from x axis to semi-major axis)
    
    
    self.compartments = []
    self.segments = []
    self.tags = set()
  
  @property
  def maxRadius(self):
    return min(self.r1, self.r2, self.r3)
  
  @property
  def minRadius(self):
    return min(self.r1, self.r2, self.r3)
  
  @property
  def avgRadius(self):
    return (self.r1 * self.r2 * self.r3)**(1.0/3.0)
  
  def getElipse(self, node1):
    """
    Return details of elipse from intersection with compartment defined by
    connecting this node and node1
    """
    if self.r2 != self.r1 or self.r3 != self.r1:
      raise IOError('Currently cannot handle ellipsoidal nodes')
    
    # unnecessary when dealing with spheres:
    #(xAxis, yAxis, zAxis) = (node1.x - x, node1.y - y, node1.z - z)
    
    return (self.r1, self.r1, 0.0, self.x, self.y, self.z)



class Compartment:  
  def __init__(self):
    # do nothing, this is a pure virtual class
    self._surfaceArea = None
    self._volume = None
    self._length = None
    self.x0 = None
    self.y0 = None
    self.z0 = None
    self.x1 = None
    self.y1 = None
    self.z1 = None
    self.tags = set()
    self.name = None
    self.segment = None
    self._nodes = None
  
  @property
  def nodes(self):
    return self._nodes
  
  @property
  def neighbors(self):
    """
    Find the compartments neighbors
    """
    raise RuntimeError('Compartment is a pure virtual class')
  
  @property
  def length(self):
    if self._length is None:
      self._length = self._calcLength()
    return self._length
  
  def _calcLength(self):
    raise RuntimeError('Compartment is a pure virtual class')
  
  @property
  def surfaceArea(self):
    if self._surfaceArea is None:
      # compute surface area and convert from um^2 to mm^2
      self._surfaceArea = 1.0e-6 * self._calcSurfaceArea()
    return self._surfaceArea
  
  def _calcSurfaceArea(self):
    raise RuntimeError('Compartment is a pure virtual class')

  @property
  def volume(self):
    if self._volume is None:
      # compute volume and convert from um^3 to mm^3
      self._volume = 1.0e-9 * self._calcVolume()
    return self._volume
  
  def _calcVolume(self):
    raise RuntimeError('Compartment is a pure virtual class')
  
  def lengthPerArea(self, _x1, _x2 = 0.5):
    raise RuntimeError('Compartment is a pure virtual class')
  
  @property
  def maxRadius(self):
    return max(n.maxRadius for n in self._nodes)
  
  @property
  def minRadius(self):
    return min(n.minRadius for n in self._nodes)
  
  @property
  def avgRadius(self):
    """
    Return an estimate of average radius, pretending compartment was a cylinder
    or sphere (as appropriate to subclass)
    """
    raise RuntimeError('Compartment is a pure virtual class')
  
  @property
  def centroid(self):
    """
    return centroid of compartment as a tuple
    """
    raise RuntimeError('Compartment is a pure virtual class')


class OneNodeCompartment(Compartment):
  def __init__(self, node):
    
    # init Compartment object
    Compartment.__init__(self)
    
    self.node = node
    self._nodes = [node]
    
    connectCompartment = node.compartments[0]
    if connectCompartment.node0 == node:
      node1 = connectCompartment.node1
    else:
      node1 = connectCompartment.node0
    (self.semiMajor, self.semiMinor, self.theta, self.x0, self.y0, self.z0) = \
      node.getElipse(node1)
    
    _direction = (node.x - node1.x, node.y - node1.y, node.z - node1.z)
    _norm = scipy.sqrt(sum([x*x for x in _direction]))
    _scale = self.length / _norm
    
    self.x1 = self.x0 + _scale * _direction[0]
    self.y1 = self.y0 + _scale * _direction[1]
    self.z1 = self.z0 + _scale * _direction[2]
    
    _scale *= 3.0/8.0
    self._centroid = (self.x0 + _scale * _direction[0], \
                      self.y0 + _scale * _direction[1], \
                      self.z0 + _scale * _direction[2])
  
  @property
  def neighbors(self):
    """
    Return a list of this compartments neighbors
    """
    return [comp for comp in self.node.compartments if comp != self]
  
  def _calcLength(self):
    """
    compute and set length
    """
    if self.node.r2 != self.node.r1 or self.node.r3 != self.node.r1:
      raise IOError('Currently cannot handle ellipsoidal nodes')
    
    return self.semiMajor
  
  def _calcSurfaceArea(self):
    """
    compute and set surface area
    """
    if self.semiMajor == self.semiMinor:
      return 4 * scipy.pi * self.semiMajor * self.semiMinor
    else:
      a = node.r1
      b = node.r2
      c = node.r3
      p = 1.6075
      # approximate formula accurate to relative error of <= 1.061%
      return 4 * scipy.pi * ( ((a*b)**p + (a*c)**p + (b*c)**p)/3 )**(1/p)
  
  def _calcVolume(self):
    """
    compute and set volume
    """
    return 4 * scipy.pi / 3 * self.node.r1 * self.node.r2 * self.node.r3

  def lengthPerArea(self, _x1, _x2 = 0.5):
    """
    compute and return length per area between relative positions _x1 and _x2
    0 <= _x <= 1, _x2 defaults to 0.5
    """
    if self.node.r2 != self.node.r1 or self.node.r3 != self.node.r1:
      raise IOError('Currently cannot handle ellipsoidal nodes')

    if _x2 < _x1:
      _temp = _x2
      _x2 = _x1
      _x1 = _temp
    
    return scipy.log((1.0 - _x1*_x1)/(1.0 - _x2*_x2)) / \
             (2 * scipy.pi * self.node.r1)
  
  @property
  def avgRadius(self):
    """
    Return an estimate of average radius, pretending compartment was a cylinder
    or sphere (as appropriate to subclass)
    """
    return (1.0e9 * self.volume / scipy.pi)**(1.0/3.0)
  
  @property
  def centroid(self):
    """
    return centroid of compartment as a tuple
    """
    return self._centroid



class TwoNodeCompartment(Compartment):
  def __init__(self, node0, node1):
    
    # init Compartment object
    Compartment.__init__(self)
    
    self.node0 = node0
    self.node1 = node1
    self._nodes = [node0, node1]
    
    (self.semiMajor0, self.semiMinor0, self.theta0, self.x0, self.y0, self.z0)\
       = node0.getElipse(node1)
    (self.semiMajor1, self.semiMinor1, self.theta1, self.x1, self.y1, self.z1)\
       = node1.getElipse(node0)
      
    self._centroid = None
  
  @property
  def neighbors(self):
    """
    Return a list of this compartments neighbors
    """
    neighbors = [comp for comp in self.node0.compartments if comp != self]
    neighbors.extend(comp for comp in self.node1.compartments if \
                     comp != self and comp not in neighbors)
    return neighbors
  
  def _calcLength(self):
    """
    compute and set length
    """
    return scipy.sqrt((self.x1 - self.x0)**2 + \
                     (self.y1 - self.y0)**2 + \
                     (self.z1 - self.z0)**2)
  
  def _calcSurfaceArea(self):
    """
    compute and set surface area
    """
    ratio0 = self.semiMinor0 / self.semiMajor0
    ratio1 = self.semiMajor1 / self.semiMajor1
    if scipy.isnan(ratio0):
      if scipy.isnan(ratio1):
        raise IOError('Degenerate (zero radius) compartment')
      ratio0 = ratio1
    elif scipy.isnan(ratio1):
      ratio1 = ratio0
    elif ratio0 != ratio1:
      raise IOError('Don''t have formula for arbitrary eliptical frustrum')
    
    if self.semiMinor0 == self.semiMinor1:
      # cylinder
      coneFactor = 1.0
    else:
      # cone
      coneFactor = \
        scipy.sqrt(1 + ((self.semiMinor0 - self.semiMinor1)/self.length)**2)
    
    if ratio0 == 1.0:
      # circular cross section
      angleFactor = scipy.pi
    else:
      # eliptical cross section
      angleFactor = 2 * special.ellipe(scipy.sqrt((1 - ratio0**2)) / coneFactor)
    
    return coneFactor * angleFactor * \
      self.length * (self.semiMajor0 + self.semiMajor1)
  
  def _calcVolume(self):
    """
    compute and set volume
    """
    return (scipy.pi / 3.0) * self.length * \
      (self.semiMajor0 * self.semiMinor0 + self.semiMajor1 * self.semiMinor1 +\
       0.5 * (self.semiMajor0 * self.semiMinor1 + \
              self.semiMajor1 * self.semiMinor0))

  def lengthPerArea(self, _x1, _x2 = 0.5):
    """
    compute and return length per cross sectional area between relative
    positions _x1 and _x2 (0 <= _x <= 1, _x2 defaults to 0.5)
    """
    if _x2 < _x1:
      _temp = _x2
      _x2 = _x1
      _x1 = _temp
    
    if self.semiMajor0 == self.semiMajor1:
      semiMajor = self.semiMajor0
      if self.semiMinor0 == self.semiMinor1:
        # cylinder
        semiMinor = self.semiMinor0
        coneFact = _x2 - _x1
      else:
        # semi-minor axis is changing, semi-major is constant
        semiMinor = 0.5 * (self.semiMinor0 + self.semiMinor0)
        minorRatio = semiMinor / (self.semiMinor1 - self.semiMinor0)
        coneFact = minorRatio * \
          scipy.log( (minorRatio + _x2 - 0.5) / (minorRatio + _x1 - 0.5) )
        if coneFact < 0:
          coneFact = -coneFact
    else:
      semiMajor = 0.5 * (self.semiMajor0 + self.semiMajor0)
      majorRatio = semiMajor / (self.semiMajor1 - self.semiMajor0)
      if self.semiMinor0 == self.semiMinor1:
        # semi-major axis is changing, semi-minor is constant
        semiMinor = self.semiMinor0
        coneFact = majorRatio * \
          scipy.log( (majorRatio + _x2 - 0.5) / (majorRatio + _x1 - 0.5) )
        if coneFact < 0:
          coneFact = -coneFact
      else:
        semiMinor = 0.5 * (self.semiMinor0 + self.semiMinor0)
        minorRatio = semiMinor / (self.semiMinor1 - self.semiMinor0)
        ratioProd = majorRatio * minorRatio
        avgRatio = 0.5 * (minorRatio + majorRatio)
        scale1 = (_x1 - 0.5) / ratioProd + avgRatio
        scale2 = (_x2 - 0.5) / ratioProd + avgRatio
        
        if avgRatio > 1.0:
          # answer in terms of logs
          ratioRoot = scipy.sqrt(avgRatio**2 - 1.0)
          scale1 /= ratioRoot
          scale2 /= ratioRoot
          coneFact = 0.5 * ratioProd / ratioRoot * \
            scipy.log((scale2 - 1) * (scale1 + 1) / ((scale2 + 1) * scale1 - 1))
        elif avgRatio < 1.0:
          # answer in terms of arctan
          ratioRoot = scipy.sqrt(1.0 - avgRatio**2)
          scale1 /= ratioRoot
          scale2 /= ratioRoot
          coneFact = ratioProd / ratioRoot * \
            (scipy.arctan(scale2) - scipy.arctan(scale1))
        else:
          # answer in terms of 1/x
          coneFact = ratioProd / scale1 - ratioProd / scale2
    
    return coneFact * self.length / (scipy.pi * semiMajor * semiMinor)

  @property
  def avgRadius(self):
    """
    Return an estimate of average radius, pretending compartment was a cylinder
    or sphere (as appropriate to subclass)
    """
    return scipy.sqrt(1.0e9 * self.volume / self.length / scipy.pi)
  
  
  @property
  def centroid(self):
    """
    Return centroid of the compartment as a tuple
    """
    if self._centroid is None:
      # need to calculate centroid location
      weightedLength = 0.5 * scipy.pi * self.length**2 * \
        ((self.semiMajor0 * self.semiMinor0 + \
          self.semiMajor0 * self.semiMinor1 + \
          self.semiMajor1 * self.semiMinor0) / 6.0 + \
         0.5 * self.semiMajor1 * self.semiMinor1)
      ratio1 = weightedLength / (1.0e9 * self.volume) / self.length
      ratio0 = 1.0 - ratio1
      self._centroid = (self.x0 * ratio0 + self.x1 * ratio1, \
                        self.y0 * ratio0 + self.y1 * ratio1,
                        self.z0 * ratio0 + self.z1 * ratio1)
    
    return self._centroid
