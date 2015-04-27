# retrieve simple properties from a geo instance

from neuron_readExportedGeometry import *
import numpy as np
import math



#######################################################################
# branch angles

def dist3(pt0, pt1):
  if len(pt0) == len(pt1) and len(pt0) == 3:
    return math.sqrt(sum([(pt0[i]-pt1[i])**2 for i in range(3)]))
  else:
    print('dimension mismatch')
    print(pt0, pt1)



def get_angle(pt0, midpt, pt1):
  if pt0 in [midpt, pt1] or pt1 in [midpt, pt0] or midpt in [pt0,pt1]:
    print('Some points are the same!')
    print(pt0, midpt, pt1)
  PT0 = dist3(pt1, midpt)
  PT1 = dist3(pt0, midpt)
  MIDPT = dist3(pt0, pt1)
  try:
    ang = math.acos( (MIDPT**2 - PT1**2 - PT0**2) / (2*PT1*PT0) )
    ang = ang*180/math.pi
  except:
    ang = 'nan'
  return ang



def find_points(seg0, seg1):
  seg0list, seg1list = [], []
  pt0where, pt1where, midwhere = None, None, None
  switchdict = {0: -1, -1: 0}
  # make a list of the node locations
  for n in seg0.nodes:
    seg0list.append([n.x,n.y,n.z])
  for n in seg1.nodes:
    seg1list.append([n.x,n.y,n.z])
    # find the common node, then use that to find the distinct ones
  for n in seg0list:
    if n in seg1list:
      midpt = n
  if seg0list.index(midpt) != 0:
    pt0where = 0
    pt0 = seg0list[0]
  else:
    pt0where = -1
    pt0 = seg0list[-1]
  if seg1list.index(midpt) != 0:
    pt1where = 0
    pt1 = seg1list[0]
  else:
    pt1where = -1
    pt1 = seg1list[-1]
  
  f = True
  if pt0 == pt1 or pt0==midpt:
    f = False
    if pt0where == 0:
      try:
        pt0=seg0list[1]
        f = True
      except:
        pass
    elif pt0where == -1:
      try:
        pt0=seglist[-2]
        f = True
      except:
        pass
  if pt0 == pt1 or pt1==midpt:
    if pt1where == 0:
      try:
        pt1=seg1list[1]
        f = True
      except:
        pass
    elif pt1where == -1:
      try:
        pt1=seg1list[-2]
        f = True
      except:
        pass
  if f == False:
    print('Tried to find new coordinates, but failed. Skipping')
  
  if pt0 in [midpt, pt1] or pt1 in [midpt, pt0] or midpt in [pt0,pt1]:
    print(seg0list, seg1list)
  #print('pt0 at %i, pt1 at %i' %(pt0where, pt1where))
  if pt1 and pt0 and midpt:
    return pt0, midpt, pt1
  else:
    print('could not figure out segments %s and %s' %(seg0.name, seg1.name))
    print(seg0list, seg1list)
    return [False]


def branch_angles(geo):
  angles = []
  for b in geo.branches:
    for n in b.neighbors:
      pts = find_points(n, b)
      if len(pts) == 3:
        pt0, midpt, pt1 = pts[0], pts[1], pts[2]
      angles.append(get_angle(pt0, midpt, pt1))
  angles = [a for a in angles if a!='nan']
  with open('temp_angles.txt', 'w') as fOut:
    for a in angles:
      fOut.write('%.10f, \n' %a)
  return angles



#######################################################################
# path length and tortuosity

def path_lengths(geo):
  tips, tipinds = geo.getTipIndices()
  pDF = PathDistanceFinder(geo, geo.soma, 0.5)
  tipsegs = [geo.segments[i] for i in tips]
  path = [pDF.distanceTo(x,y) for x, y in zip(tipsegs, tipinds)]
  tort = [pDF.tortuosityTo(x,y) for x, y in zip(tipsegs, tipinds)]
  return path, tort
  
# if FilamentIndex != geo.segments[index], use this: 
# tipsegs = [i for i in geo.segments if geo.getFilamentIndex(i) in tips]
# path, tort = [], []
# for x, y in zip(tipsegs, tipinds):          
#  try:
#    p, t = pDF.distanceTo(x,y), pDF.tortuosityTo(x,y)
#    path.append(p)
#    tort.append(t)
#  except:
#    continue




#######################################################################
# sholl stuff

def interpoint_dist(geo):
  # determine the distances between successive points
  def nodex(node):
    return [node.x, node.y, node.z]
  dists = []
  for s in geo.segments:
    for n in range(len(s.nodes)-1):
      dists.append(dist3(nodex(s.nodes[n]), nodex(s.nodes[n+1])))
  print('Mean distance (%i points): %.5f +/- %.5f' 
         %(len(dists), np.mean(dists), np.std(dists)))
  return dists


def interpolate_nodes(geo):
  # find the most common distance betwixt successive nodes and then,
  # when successive nodes leave integer multiples of this distance
  # interpolate the difference to 'even' it out
  def nodex(node):
    return [node.x, node.y, node.z]
    
  def interp(pt1, pt2, ints):
    Xs = np.linspace(pt1[0], pt2[0], ints)
    Ys = np.linspace(pt1[1], pt2[1], ints)
    Zs = np.linspace(pt1[2], pt2[2], ints)
    return [[Xs[i],Ys[i],Zs[i]] for i in range(len(Xs))]
    
  dist = np.median(interpoint_dist(geo))
  pts = []
  segcount = -1
  for s in geo.segments:
    segcount = segcount + 1
    if segcount % 100 == 0:
      print('Completed %i/%i segments ' 
             %(segcount,len(geo.segments)))
    for n in range(len(s.nodes)-1):
      # if too far between nodes, interpolate
      if dist3(nodex(s.nodes[n]),nodex(s.nodes[n+1])) > 2 * dist:
        integer_interpolate = int((dist3(nodex(s.nodes[n]),
                                         nodex(s.nodes[n+1])))
                                   /dist)
        new_pts = interp(nodex(s.nodes[n]),nodex(s.nodes[n+1]),
                         integer_interpolate)
      # else just add the regular node pts
      else:
        new_pts = [nodex(s.nodes[n]), nodex(s.nodes[n+1])]
      # add the points as long as they don't already exist in pts
      for p in new_pts:
        if p not in pts:
          pts.append(p)
  # now should have all the points
  soma = geo.soma.coordAt(0.5)
  distances = []
  for p in pts:
    distances.append(dist3(soma, p))
  return distances


######################################################################
# partition asymmetry

def get_segment(geo, segname):
  for s in geo.segments:
    if s.name == segname:
      return s


def tips_asymmetry(geo):
  # Get the tip asymmetry of the neuron. Follow the soma's neighbors
  # until there are more than 1, then start there.
  # seg_lengths: dict with a section_name for keys and float as values
  # seg_tips: dict with sec_name as key and list of segment objects as values
  prevsegs = [geo.soma]
  newsegs = [i for i in geo.soma.neighbors if i not in prevsegs]
  go = True
  while go:
    if len(newsegs) > 1:
      nebs = newsegs
      go = False
    else:
      for k in newsegs:
        prevsegs.append(k)
        for j in k.neighbors:
          newsegs.append(j)
        # not sure if this is allowed, but should be since not referencing by index
        newsegs.pop(newsegs.index(k))
        
  pDF = PathDistanceFinder(geo, geo.soma, 0.5)
  # nebs = geo.soma.neighbors
  tips, tipPositions = geo.getTips()
  seg_names = {}
  seg_tips = {}
  for n in nebs:
    seg_names[n.name] = []
    seg_tips[n.name] = []
  seg_lengths = {}
  for t, pos in zip(tips, tipPositions):
    curr_path = pDF.pathTo(t, pos)
    for n in seg_names.keys():
      # if the bifurcation in question is contained in the path soma->tip
      if get_segment(geo,n) in curr_path:
        # add this tip to n
        seg_tips[n].append(t)
        for c in curr_path:
          if c not in seg_names[n]:
            seg_names[n].append(c)
  # now should have all of the segments that lead to the tips in each key
  for k in seg_names.keys():
    seg_lengths[k] = 0
    for s in seg_names[k]:
      seg_lengths[k] = seg_lengths[k] + s.length
  
  return seg_lengths, seg_tips


def tip_coords(geo, seg_tips):
  # return x-y-z tuples for each tip; just use the (1) position of each tip seg
  tip_coords = {}
  for k in seg_tips.keys():
    tip_coords[k] = []
    for t in seg_tips[k]:
      tip_coords[k].append(t.coordAt(1))
  return tip_coords
  
  


######################################################################
# torques

def getNormVector(points):
  #print(points, np.shape(points))
  v1 = [points[1][0][i] - points[0][0][i] for i in range(3)]
  v2 = [points[2][0][i] - points[0][0][i] for i in range(3)]
  normVec = np.cross(v1,v2)
  return normVec


def angleBetween(plane1,plane2,planCoords):
  # get normal vectors
  n1, n2 = getNormVector(planCoords[plane1]), \
           getNormVector(planCoords[plane2])
  angle = np.arccos( (abs(n1[0]*n2[0] + n1[1]*n2[1] + n1[2]*n2[2])) /
                     ( np.sqrt(n1[0]**2+n1[1]**2+n1[2]**2) *
                       np.sqrt(n2[0]**2+n2[1]**2+n2[2]**2) ) )
  return angle*180/np.pi


def get_torques(geo)
  # return bifurcation torques
  Cons =  geo.connections
  Seg1s, Seg2s = [], []
  for c in Cons:
    Seg1s.append(c['filament1']) # here, location1 is always 0
    Seg2s.append(c['filament2']) # here, location2 is always 1
    #geometry.c['filament1'].coordAt(c['location1'])
  
  tsegs = np.array([Seg1s,Seg2s]).T
  tsegs = tsegs.reshape(len(tsegs)*2)
  segs = set(tsegs)
  planCoords = {}
  
  count = 0
  for seg in segs:
    friends, friendcoords = [], []
    for s in geo.segments:
      if s.name == seg:
        friends.append(s.name)
        if s.name in Seg1s:
          friends.append(Seg2s[Seg1s.index(s.name)])
        if s.name in Seg2s:
          friends.append(Seg1s[Seg2s.index(s.name)])
    #print('friends compiled')
      
    for s in geometry.segments:
      if s.name in friends:
        friendcoords.append([s.coordAt(1)])
    count = count + 1
    #if count%100 == 0:
    #  print('%i of %i segments done' %(count, len(segs)))
    if len(friendcoords) > 2: # need 3 points to define plane
      planCoords[seg]=friendcoords
  
  planCoordskeys = []
  for s in geo.segments: # loop through segments to find plane-neighbors
    if s.name in planCoords.keys():
      for n in s.neighbors:
        if n.name in planCoords.keys(): # if the neighbor is also a bifurcation
          planCoordskeys.append([s.name, n.name]) # add it
        else: # otherwise, keep looking for a neighbor that is
          for nn in n.neighbors:
            if nn.name in planCoords.keys():
              planCoordskeys.append([s.name, nn.name])
  
  # get torques
  torques = []
  for P in planCoordskeys:
    torques.append(angleBetween(P[0],P[1],planCoords))
  return torques
































