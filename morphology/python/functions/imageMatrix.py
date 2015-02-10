# imageMatrix.py - image processing toolbox for neuron stuff
# usage: python imageMatrix.py tiffDirectory/ outFile

import os, sys, math
import numpy as np
from PIL import Image
from multiprocessing import Pool
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from timeit import default_timer as timer
#import pickle


# SIMPLE TOOLS
########################################################################
def load_img_array(imFile):
  img = Image.open(imFile)
  arr = np.array(img)
  return arr


def show_dist(l):
  fig = plt.figure()
  ax = fig.add_subplot(111)
  ax.hist(l, bins=int(len(l)/100))
  plt.show()
  

def make255(arr, T=255):
  zarr = np.zeros(np.shape(arr))
  for i in range(len(arr)):
    for j in range(len(arr[0])):
      if arr[i,j] > 0:
        zarr[i,j] = T
  return zarr


def matrix2coords(darr, voxel):
  # A 3-D array is converted to a list of triplets at each voxel
  print('Converting the matrix to coordinates.')
  coords = []
  for z in range(len(darr)):
    for x in range(len(darr[z])):
      for y in range(len(darr[z][x])):
        if darr[z][x,y] > 0:
          coords.append([x*voxel[0],y*voxel[1],z*voxel[2]])
    print('z-slice # %i done' %int(z+1))
  return coords


def coords2matrix(coords, voxel, i=1):
  # x-y-z tuples are converted to a matrix; "i" is the matrix value
  matrix = []
  dims = len(voxel)
  for c in coords:
    matrix.append([int(c[i]/voxel[i]) for i in range(dims)])
  matrix = np.array(matrix)
  maxs = [max(matrix[:,i]) for i in range(dims)]
  print(maxs, dims)
  newmat = np.zeros([i+1 for i in maxs])
  if dims == 2:
    for m in matrix:
      newmat[m[0],m[1]] = i
  elif dims == 3:
    for m in matrix:
      newmat[m[0],m[1],m[2]] = i
  return newmat


def gen_plane(vec, voxel, M=10, sshow=False):
  if len(vec) != 3:
    print('Error: a vector is defined by 3 values: i,j,k')
  else:
    plan = []
    xs = np.linspace(vec[0]-voxel[0]*M,vec[0]+voxel[0]*M, M*2) # default is + 10 voxels
    ys = np.linspace(vec[1]-voxel[1]*M,vec[1]+voxel[1]*M, M*2)
    def solve(vec, x, y):
      return -((x*vec[0] + y*vec[1])/vec[2])
    for m in xs:
      for n in ys:
        plan.append([m+voxel[0],n+voxel[1],solve(vec,m,n)+voxel[2]])
        # this also scales it back to the original location
    lin = [[v*i for v in vec] for i in range(-10,10)] 
    if sshow == True:
      fig = plt.figure()
      ax = fig.add_subplot(111, projection='3d')
      for p in plan:
        ax.scatter(p[0],p[1],p[2], c='b', edgecolor='b', alpha=0.1)
      for p in lin:
        ax.scatter(p[0],p[1],p[2], c='r', edgecolor='r')
      plt.show()
    return plan


def test_plane(plan, cross_sec, voxel):
  # if plane isn't large enough (all outer 1's adjacent to 0's) it doubles the size
  if len(plan[0]) != 3 or len(voxel) != 3 or len(cross_sec[0]) != 3:
    print('Need 3 values (x,y,z) for plane, cross section and voxel args')
    return False
  
  def distance(pt0,pt1):
    return np.sqrt(sum([(pt0[i]-pt1[i])**2 for i in range(3)]))
  
  def plane_info(plan):
    def farthest_pt(pt,pts):
      long_dist = 0
      for p in pts:
        if distance(p,pt) > long_dist:
          long_dist = distance(p,pt)
      return long_dist
      
    # start here
  
  def check_cs(p_info, cs_info):
    # if the span of plan <= span of cs, the plane needs to be extended
    # so that it is larger than the cross-section
    expand = 1
    for i in range(3):
      if p_info['M'][i] <= cs_info['M'][i]:
        expand = expand*0 
    return [False, True][expand]
  # if even 1 dimension is identical the plane should be extended
  
  #def expand_plan(p_info, voxel):
  #  minmax = [['xmin','xmax'],['ymin','ymax'],['zmin','zmax']]

  p_info = plan_info(plan)
  cs_info = plan_info(cross_sec)
  expand = check_cs(p_info, cs_info)
  return expand
  #if expand:
  #  new_plane = expand_plan(p_info, voxel)
  #  return [expand, new_plane]
  #else:
  #  return [expand]





def switch_binary(arr):
  zarr = np.zeros(np.shape(arr))
  for i in range(len(arr)):
    for j in range(len(arr[0])):
      if arr[i,j] == 1:
        zarr[i,j] = 125
      elif arr[i,j] == 2:
        zarr[i,j] = 255
  return zarr


def clean_filament(darr):
  #carr = np.zeros(np.shape(darr))
  for s in range(len(darr)): # for each slice
    width, start, row  = 0, None, None
    for r in range(len(darr[s])): # for each row
      count = 0
      for c in range(len(darr[s][r])):
        if darr[s][r][c] == 1 and count == 0:
          temp_start = c
        if darr[s][r][c] == 1:
          count = count + 1
      # at the end of each row:
      if count > width:
        width = count
        start = temp_start
        row = r
    # at the end of each slice:
    print(s, row, start, int(width/2))
    darr[s][row][start+int(width/2)] = 2
  return darr


def display_fake_filament(darr):
  rimgs = [Image.fromarray(darr[i]) for i in range(10)]
  for k in range(10):
    rimgs[k].show()
  return


def gen_segment(fake_filament):
  # default value
  
  def make_1d(arr, invert=True, gray=False): # make 1-D and grayscale, invert
    narr = np.zeros([len(arr), len(arr[0])])
    if gray:
      newscheme = [0, 125, 255] # gray skeleton on black neuron, background white
    else:
      newscheme = [0,2,1] # 'binary', not gray
    for i in range(len(arr)):
      for j in range(len(arr[0])):
        if arr[i,j,0] == 255:
          narr[i,j] = newscheme[0]
        elif arr[i,j,0] == 0:
          narr[i,j] = newscheme[2]
        else:
          narr[i,j] = newscheme[2]
    return narr
    
  def load_fake_segment(imfile):
    img = Image.open(imfile)
    arr = np.array(img)
    return make_1d(arr) # already been done!
    
  fils = os.listdir(fake_filament)
  fils = [fake_filament+i for i in fils]
  fils.sort()
  darr = []
  for f in fils:
    darr.append(load_fake_segment(f))
  #pool = Pool() # these calls were yielding a pickle error related to pool properties
  #darr = pool.map(load_fake_segment, fils) # 
  #pool.close()
  #pool.join()
  return darr # return multi-dimensional array


def plot_cross_secs(cs):
  # assumes each element in cs is a cross section with a [x,y,z]
  fig = plt.figure()
  ax = fig.add_subplot(111, projection='3d')
  cols = ['b','r','g','y','k']*int(len(cs)/5+1)
  for c in range(len(cs)):
    for i in range(len(cs[c])):
      ax.scatter(cs[c][i][0],cs[c][i][1],cs[c][i][2], c=cols[c], 
                 edgecolor = cols[c], alpha = 0.1)
  plt.show()


def fake_cross_section(fake_filament='/home/alex/data/morphology/848/848_081/fake_filament/'):
  def dist(inds0, inds1):
    if len(inds0) != len(inds1): print('Dimension mismatch!')
    else:
      return sum([math.sqrt((x-y)**2) for x,y in zip(inds0, inds1)])
  
  def return_nearest_dist(pt, points):
    mindist = np.inf
    for p in points:
      currdist = dist(pt, p)
      if currdist < mindist:
        mindist = currdist
    return mindist
  
  def get_skelcoords(darr, voxel, skel=2):
    skelcoords = []
    for s in range(len(darr)):
      for i in range(len(darr[s])):
        for j in range(len(darr[s][i])):
          if darr[s][i][j] == 2:
            skelcoords.append([x*y for x,y in zip(voxel,[i,j,s])])
    return skelcoords
  
  def return_cross_section(pt0, pt1, darr, voxel, M=10):
    # so that the plane can be scaled, get two points and instead of vec
    vec = get_vector(pt0, pt1)
    plancoords = gen_plane(vec, voxel, M, False)
    # scale the plancoords to match with pt0
    for p in range(len(plancoords)):
      for i in range(3):
        plancoords[p][i] = plancoords[p][i] + pt0[i]
    voxelcoords = matrix2coords(darr, voxel)
    vdist = 2 * dist([0,0,0],voxel)
    cross_sec = []
    for p in plancoords:
      if return_nearest_dist(p, voxelcoords) <= vdist:
        cross_sec.append(p)
        
    return cross_sec, plancoords
    # plan_mat = coords2matrix(plancoords, voxel)
    # want to find overlap between plane and segment
    
  def get_vector(pt0, pt1):
    if len(pt0) == len(pt1):
      return [x-y for x,y in zip(pt0,pt1)]
    
  voxel = [0.0176,0.0176,0.38]
  darr = gen_segment(fake_filament) # raw image
  carr = clean_filament(darr)
  skelcoords = get_skelcoords(carr, voxel)
  cross_secs = []
  
  for s in range(len(skelcoords)-1):
    pt0, pt1 = skelcoords[s],skelcoords[s+1]
    M=10
    cs, plancoords = return_cross_section(pt0, pt1, carr, voxel, M)
    notbigenough = test_plane(plancoords, cs, voxel)
    cycle = 0
    while notbigenough:
      M=M*2
      cs, plancoords = return_cross_section(pt0, pt1, carr, voxel, M)
      notbigenough = test_plane(plancoords, cs, voxel)
      cycle = cycle + 1
      print('%ith iteration. Current specs:' %cycle)
      print(np.shape(cs), M)
    cross_secs.append(cs) # once the plane exceeds the cross section, append
    
  return cross_secs, narr
  

def plot_multi_coords(skelcoords, voxelcoords=False, plancoords=False):
  fig = plt.figure()
  ax = fig.add_subplot(111, projection='3d')
  colors, alphas = ['r','b','k'], [0.1,0.01,0.01]
  print('Starting plot...')
  for s in skelcoords:
    ax.scatter(s[0],s[1],s[2],c=colors[0],edgecolor=colors[0],alpha=alphas[0])
  if voxelcoords:
    for v in voxelcoords:
      ax.scatter(v[0],v[1],v[2],c=colors[1],edgecolor=colors[1],alpha=alphas[1])
  if plancoords:
    for p in plancoords:
      ax.scatter(p[0],p[1],p[2],c=colors[2],edgecolor=colors[2],alpha=alphas[2])
  plt.show()
  



# BINARY OPTIONS 
########################################################################

def make_binary(imFile):
  # make a binary image with a moving average
  # this version is currently not being used
  arr = load_img_array(imFile)
  zarr = np.zeros(np.shape(arr))
  on = False
  avg = None
  firstcol, lastcol = 0, len(arr[0])-1
  for row in range(len(arr)):
    brightest = max(arr[row])
    ind = list(arr[row]).index(brightest)
    zarr[row,ind] = 1
    
    # proceed bidirectionally
    if ind != firstcol and ind != lastcol:
      # negative direction
      prev = brightest
      neg_ind = ind
      while neg_ind > firstcol:
        neg_ind = neg_ind - 1
        if abs((arr[row, neg_ind]-arr[row, ind]) \
          / max(arr[row,ind],arr[row,neg_ind])) < 0.1:
            # include in binary array
            zarr[row, neg_ind] = 1
            prev = arr[row, neg_ind]
      # positive direction
      prev = brightest
      pos_ind = ind
      while pos_ind < lastcol:
        pos_ind = pos_ind + 1
        if abs((arr[row, pos_ind]-arr[row,ind]) \
          / max(arr[row,ind], arr[row,pos_ind])) < 0.1:
            # include in binary array
            zarr[row, pos_ind] = 1
            prev = arr[row, pos_ind]
      # else they will be zero as pre-set
    elif ind == firstcol:
      # only need positive direction
      prev = brightest
      pos_ind = ind
      while pos_ind < lastcol:
        pos_ind = pos_ind + 1
        if abs((arr[row, pos_ind]-arr[row,ind]) \
          / max(arr[row,ind], arr[row,pos_ind])) < 0.1:
            # include in binary array
            zarr[row, pos_ind] = 1
            prev = arr[row, pos_ind]
    elif ind == lastcol:
      # only need negative direction
      prev = brightest
      neg_ind = ind
      while neg_ind > firstcol:
        neg_ind = neg_ind - 1
        if abs((arr[row, neg_ind]-arr[row, ind]) \
          / max(arr[row,ind],arr[row,neg_ind])) < 0.1:
            # include in binary array
            zarr[row, neg_ind] = 1
            prev = arr[row, neg_ind]
    else:
      print('Bad column found in row %i, col %i' %(row, ind))
  # marks end of a row
  
  return zarr # binary array



def make_binary_thresh(imfile, thresh=20):
  # uses a hard threshold to determine which pixels to include
  img = Image.open(imfile)
  arr = np.array(img)
  # helper function
  def betch(arr):
    bets = np.zeros(np.shape(arr))
    for i in range(len(arr)):
      for j in range(len(arr[0])):
        if arr[i,j] > thresh:
          bets[i,j] = 1
    return bets
  zarr = betch(arr)
  return zarr



def make_binary_fromhist(imfile,T=1.0, sshow=False):
  img = Image.open(imfile)
  arr = np.array(img)
  # helper function
  def betch(arr):
    bets = []
    for i in range(len(arr)):
      for j in range(len(arr[0])):
        bets.append(arr[i,j])
    return bets
  bins, edges = np.histogram(betch(arr), 255) # one bin for each intensity
  edges = [(edges[i]+edges[i+1])/2 for i in range(len(edges)-1)]
  target = bins[-1]*T # last bin should be 255, mult by the provided T val
  curr, ind = np.inf, -1
  while curr > target:
    ind = ind+1
    curr = bins[ind]
  thresh = edges[ind-1]
  # make binary image
  zarr = np.zeros(np.shape(arr))
  for i in range(len(arr)):
    for j in range(len(arr[0])):
      if arr[i,j] > thresh:
        if sshow == True:
          zarr[i,j] = 255
        else:
          zarr[i,j] = 1
  if sshow == True:
    zimg = Image.fromarray(zarr)
    zimg.show()
    
  print('file %s done' %imfile)
  return zarr



# MEAT AND POTATOES
########################################################################

# this mapping function will parallelize image loading and thresholding
def import_images(folder, par=True, ttime=True):
  """
  This function loads images from a folder as PIL Image files and
  thresholds them, creating a list of z-slices to be turned into a matrix
  This version is not currently used.
  """
  fils = [os.listdir(folder)]
  def keep_tifs(rawlist):
    tiflist = []
    for f in rawlist:
      if len(f.split('.'))>1:
        if f.split('.')[1] == 'tif':
          tiflist.append(f)
    return tiflist
  tiflist = keep_tifs(fils)
  newtiflist = [folder+f for f in tiflist].sort() # alphabetize
  tifobjs = [load_img_array(f) for f in tiflist]
  
  # here start parallel stuff
  if par or ttime:
    start_time_par = timer()
    pool = Pool(8)
    results_par = pool.map(show_at_thresh, tifobjs)
    pool.close()
    pool.join()
    total_time_par = timer() - start_time_par
  # or non-parallel stuff
  elif par==False or ttime:
    start_time_nopar = timer()
    results_nopar = [show_at_thresh(f) for f in newtiflist]
    total_time_nopar = timer() - start_time_nopar
  print('Time for parallel: %.2f seconds' % total_time_par)
  print('Time for non-parallel: %.2f seconds' % total_time_nopar)
  
  return results_par, results_nopar
  
  


def save_coords(coords, fname='tempcoords.p'):
  pickle.dump(coords, open(fname, 'wb'))
  print('Coordinates written to %s as pickle' %fname)
  return



def get_voxel_locations(folder, fname, voxel=[0.176,0.176,0.38], ssave=True):
  # uses raw threshold function
  # get images as list
  fils = os.listdir(folder)
  def keep_tifs(rawlist):
    tiflist = []
    for f in rawlist:
      if len(f.split('.'))>1:
        if f.split('.')[1] == 'tif':
          tiflist.append(f)
    return tiflist
  tiflist = keep_tifs(fils) # this indexing may be removed if needed
  newtiflist = [folder+f for f in tiflist]
  newtiflist.sort() # alphabetize
  # commandeer all cores
  stime = timer()
  pool = Pool()
  darr = pool.map(make_binary_thresh, newtiflist) # adjust threshold in function
  pool.close()
  pool.join()
  print('Time taken for retrieving coordinates: %.2f' %(timer()-stime))
  # send to matrix2coords to get tuples back
  coords = matrix2coords(darr, voxel)
  if ssave:
    # save this
    save_coords(coords)
  
  return coords, darr




### testing shit
"""
start = timer()
zarr = [make255(i) for i in result]
print('time taken: %.2f' %(timer()-start))

start = timer()
pool = Pool()
zzarr = pool.map(make255, result)
print('time taken: %.2f' %(timer()-start))
"""


############################ CONTROL ##############################

# usage: python imageMatrix.py[0] tiffDirectory/[1] outFile[2]
if __name__ == '__main__':
  arguments = sys.argv
  directory = arguments[1]
  if len(arguments) > 2:
    outfile = arguments[2]
  else:
    outfile = 'temp_coords.p'
  get_voxel_locations(directory, outfile, [1,1,1])
 







