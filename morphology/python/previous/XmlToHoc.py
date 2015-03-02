# XmlToHoc.py - converts an xml file created in knossos to a hoc file

import os, sys
import numpy as np
import networkx as nx



class SkelHoc():
  def __init__(self, xmlFile, hocFile='hoc_from_xml.hoc'):
    self.xmlfile = xmlFile
    self.hocfile = hocFile
    self.nodes = {'*': None}
    self.nodelist = []
    self.edges = []
    self.branchpoints = []
    self.branchset = []
    self.segments = {'*': None}
    self.connections = []
    self.sources, self.targets = None, None
    self.dist = None
    
    # now do shit
    self.read_xml()
    self.q_distance()
    self.create_segments()
    self.create_connections()
  
  
  
  
  ####### Read File #######
  def add_node(self, line):
    splits = line.split('"')
    ID, x, y, z, rad = splits[1], float(splits[5]), float(splits[7]), \
                       float(splits[9]), float(splits[3])
    self.nodes[ID] = [x,y,z,rad]
    self.nodelist.append(int(ID))
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
  
  
  def q_distance(self):
    def dist(pt0, pt1):
      if len(pt0) != len(pt1):
        print('Dimension mismatch between pt0 and pt1')
        return
      return np.sqrt(sum([(pt1[i]-pt0[i])**2 for i in range(len(pt0))]))
    
    mindist = np.inf
    nlist = list(self.nodes.keys())
    nlist.pop(nlist.index('*'))
    for n in range(1,len(nlist)):
      curr_dist = dist(self.nodes[nlist[0]][:3], self.nodes[nlist[n]][:3])
      if curr_dist < mindist and curr_dist != mindist:
        mindist = curr_dist
    self.dist = mindist
    print('Quantal distance is %.5f ' %mindist)
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
    All = self.sources + self.targets
    for a in All:
      if All.count(a) > 2 and a not in self.branchset:
        self.branchset.append(a)
      if a not in self.nodelist:
        print('A source/target node not in the nodelist')
    
    self.branchset = self.branchset*2
    self.nodelist = self.nodelist + self.branchset
    
    
    orig = int(len(self.branchset))
    while len(self.branchset) > 0:
      if len(self.branchset)%500 == 0:
        print('%f percent branchpoints done.' %float(100-100*len(self.branchset)/orig))
      #print('%i branchpoints remaining ...' %len(self.branchset))
      rand_ind = int(np.random.random(1)*len(self.branchset))
      self.crawl(int(self.branchset[rand_ind]))
      
    return self
  
  
    
  def crawl(self, b):
    """
    Current is always already added to node list.
    """
    nodes = [] # only in scope of current function; else self.nodes
    go = False
    if b in self.sources or b in self.targets:
      go = True
    else:
      try:
        self.branchset.pop(self.branchset.index(b))
      except:
        print()
      return
    current = b
    while go:
      if current in self.sources and current not in self.branchpoints:
        ind = self.sources.index(current)
        current = self.targets[ind]
        if current not in nodes:
          nodes.append(current)
        if current in self.nodelist:
          self.nodelist.pop(self.nodelist.index(current))
        else:
          print('Current %i not in self.nodelist!' %current)
        
      if current in self.targets and current not in self.branchpoints:
        ind = self.targets.index(current)
        current = self.sources[ind]
        if current not in nodes:
          nodes.append(current)
        if current in self.nodelist:
          self.nodelist.pop(self.nodelist.index(current))
        else:
          print('Current %i not in self.nodelist!' %current)
        self.sources.pop(ind)
        self.targets.pop(ind)
      if current in self.branchpoints:
        go = False
        try:
          self.branchset.pop(self.branchset.index(current))
        except:
          print('Tried to pop %i from branchset but was already popped'
                % current)
        self.sources.pop(ind)
        self.targets.pop(ind)
      else:
        go = False
    # end of while loop
    #if nodes[0] not in self.branchpoints or nodes[-1] not in self.branchpoints:
    #  print('Segment does not start and end with branchpoints:')
    self.format_segment(nodes) # format the segment to prepare for hoc
    return self

  

  def format_segment(self, nodes):
    newseg = len(self.segments.keys())
    self.segments[str(newseg)] = {}
    for n in range(len(nodes)):
      self.segments[str(newseg)][str(n)] = \
        self.nodes[str(nodes[n])]
    return self
  
  
  
  def create_connections(self):
    end_segs, end0s, end1s, no_connections = [], [], [], []
    if '*' in self.segments.keys():
      self.segments.pop('*')
    for c in self.segments.keys():
      end_segs.append(c)
      end0s.append(self.segments[c]['0'])
      last = len(self.segments[c])-1
      end1s.append(self.segments[c][str(last)])
    print(len(end0s), len(end1s))
    for e in range(len(end_segs)):
      if end1s[e] in end0s:
        # found a segment that connects to e's 1th end
        f = end0s.index(end1s[e])
        newcon = {'0': end_segs[f], '1': end_segs[e]}
        if newcon not in self.connections:
          self.connections.append(newcon)
      if end0s[e] in end1s:
        # found a segment that connects to e's 0th end
        f = end1s.index(end0s[e])
        newcon = {'0': end_segs[e], '1': end_segs[f]}
        if newcon not in self.connections:
          self.connections.append(newcon)
      if end0s[e] not in end1s and end1s[e] not in end0s:
        no_connections.append(e)
    print('Total connections: %i. No connections found for %i segments.'
     %(len(self.connections), len(no_connections)))
    # end of for loop
    return self

  
  
  ####### write the hoc file #####
  def write_hoc(self):
    with open(self.hocfile, 'r') as fOut:
      
      def start_hoc(total_segs):
        fOut.write('create filament_999[%i]\n\n' %total_segs)
        return
      
      def create_filament(sname):
        fOut.write('filament_999[%i] {\n' %sname)
        fOut.write('  pt3dclear()\n')
        return
      
      def pt3dadd(node):
        fOut.write('  pt3dadd(%f, %f, %f, %f)\n'
                   %(node[0], node[1], node[2], node[3]))
        return
      
      def end_filament():
        fOut.write('}\n\n')
        return
      
      def connect_filaments(conns):
        # conns is a dict of shape {'0': filA, '1': filB}
        fOut.write('connect filament_999[%i](0.0), filament_999[%i](1.0)\n'
                   %(conns['0'], conns['1']))
        return
      
      # do stuff
      start_hoc(len(self.segments.keys()))
      for sname in self.segments.keys():
        create_filament(int(sname))
        for node in self.segments[sname].keys():
          pt3dadd(self.segments[sname][node])
        end_filament()
      
      for con in self.connections:
        pass
      return
  






































