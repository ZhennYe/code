# XmlToHoc.py - converts an xml file created in knossos to a hoc file

import os, sys
import numpy as np
import networkx as nx



class SkelHoc():
  def __init__(self, xmlFile, hocFile='hoc_from_xml.hoc'):
    self.xmlfile = xmlFile
    self.hocfile = hocFile
    self.nodes = {'*': None}
    self.edges = []
    self.branchpoints = []
    self.branchset = None
    self.segments = {'*': None}
    self.connections = {'*': None}
    self.sources, self.targets = None, None
    
    # now do shit
    self.read_xml()
  
  ####### Read File #######
  def add_node(self, line):
    splits = line.split('"')
    ID, x, y, z, rad = splits[1], float(splits[5]), float(splits[7]), \
                       float(splits[9]), float(splits[3])
    self.nodes[ID] = [x,y,z,rad]
    return self
  
  def add_edge(self, line):
    splits = line.split('"')
    e = {'source': int(splits[1]), 'target': int(splits[3])}
    self.edges.append(e)
    return self
  
  def add_branchpoint(self, line):
    splits = line.split('"')
    self.branchpoints.append(splits[1])
    return self
  
  def read_xml(self):
    with open(self.xmlfile, 'r') as fIn:
      lineNum = 0
      for line in fIn:
        lineNum = lineNum + 1
        splitLine = line.split(None)[0].split('<')[1]
        if splitLine == 'node':
          self.add_node(line)
        elif splitLine == 'edge':
          self.add_edge(line)
        elif splitLine == 'branchpoint':
          self.add_branchpoint(line)
        
    return self
  
  
  
  ####### Do analysis ########
  
  def create_segments(self):
    """
    Run through edges, nodes and branchpoints to return the actual
    segments. 
    """
    # start with a random edge (or node)
    self.sources = [i['source'] for i in self.edges]
    self.targets = [i['target'] for i in self.edges]
    Edges = zip(sources, targets)
    self.branchset = self.branchpoints*2
    
    while len(self.branchpoints) > 0:
      rand_ind = int(np.random.random(1)*len(self.branchpoints
      self.
    
  def crawl(self, b):
    """
    Current is always already added to node list.
    """
    nodes = [] # only in scope of current function; else self.nodes
    go = False
    if b in self.sources or b in self.targets:
      go = True
    else:
      self.branchset.pop(self.branchset.index(b))
      return
    current = b
    while go:
      if current in self.sources and current not in self.branchpoints:
        ind = self.sources.index(current)
        current = self.targets[ind]
        nodes.append(current)
        self.sources.pop(ind)
        self.targets.pop(ind)
      if current in self.targets and current not in self.branchpoints:
        ind = self.targets.index(current)
        current = self.sources[ind]
        nodes.append(current)
        self.sources.pop(ind)
        self.targets.pop(ind)
      if current in self.branchpoints:
        go = False
        try:
          self.branchset.pop(self.branchset.index(current))
        except:
          print('Tried to pop %i from branchset but was already popped'
                % current)
    # end of while loop
    if nodes[0] not in self.branchpoints or nodes[-1] not in self.branchpoints:
      print('Segment does not start and end with branchpoints:')
      print(nodes)
    self.format_segment(nodes) # format the segment to prepare for hoc
    return self

  

  def format_segment(self, nodes):
    self.segments[str(len(self.segments.keys()))] = {}
    for n in range(len(nodes)):
      self.segments[str(len(self.segments.keys()))][str(n)] = \
        self.nodes[str(nodes[n])]
    return self
    
    
  
  

class Segment():
  def __init__(self, branchpoints, all_nodes, edges, n):
    # n is the current branchpoint number
    self.branchpoints = branchpoints
    self.all_nodes = all_nodes
    self.edges = edges
    self.n = n
    
  
  
  def node_crawl(self):
    """
    Go from one branchpoint to the next.
    """
    




class Node():
  def __init__(self, name, x, y, z, rad):
    self.name = name
    self.x = x
    self.y = y
    self.z = z
    self.rad = rad
    return self




















