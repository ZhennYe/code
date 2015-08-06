## hocOrders.py -- for a neuron with max branch order N, create
#                  N/10 hoc files, each with 10 steps of increasing
#                  complexity
# usage: python hocOrder.py infile.hoc outfile.hoc (optional)


def get_seglist(geo):
  """
  Return the seglist and the PathDistanceFinder object.
  """
  pDF = PathDistanceFinder(geo, geo.soma)
  seglist = list(pDF.branchOrders.keys())
  return pDF, seglist



def get_ordern(geo, pDF, seglist, n):
  """
  Only return segments of order n and below.
  """
  def add_seg(segarray, seg, segnum):
    for node in seg.nodes:
      segarray.append([segnum, node.x, node.y, node.z, node.r1])
    return segarray
  def real_seg(segname):
    s = segname.split('_')[1]
    filament = int(s.split('[')[0])
    seg = int(s.split('[')[1].split(']')[0])
    if filament == 999:
      return [seg]
    else:
      return [filament, seg]
  #
  segarray = [] # [seg, x, y, z, rad]
  for s in seglist:
    if pDF.branchOrder(s) <= n:
      segarray = add_seg(segarray, s, real_seg(s.name))
  return segarray



def write_hoc(geofile, segarray, outfile=None):
  """
  This writes the hoc file.
  """
  if outfile is None:
    outfile = geofiles.split('.')[-2]+'_order.hoc'
  with open(outfile, 'w') as fOut:
    
    prevseg = None
    for s in range(len(segarray)):
      elem = segarray[s]
      if elem[-1] != prevseg: # Start a new segment
      
  
  


def get_orders(geo, orders):
  """
  
  """
  
  pDF, seglist = get_seglist(geo)
  

