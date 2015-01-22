# imageMatrix.py - image processing toolbox for neuron stuff
# usage: python imageMatrix.py tiffDirectory/ outFile

import os, sys
import numpy as np
from PIL import Image
from multiprocessing import Pool
import matplotlib as plt
from timeit import default_timer as timer
import pickle


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
    print('z-slice # %i done' %z)
  return coords


def coords2matrix(coords, voxel, i=1):
  # x-y-z tuples are converted to a matrix; "i" is the matrix value
  matrix = []
  for c in coords:
    matrix.append([c[0]/voxel[0], c[1]/voxel[1], c[2]/voxel[2]])
  matrix = np.array(matrix)
  maxs = [max(matrix[:,i]) for i in range(3)]
  newmat = np.zeros([maxs[0],maxs[1],maxs[2]])
  for m in matrix:
    newmat[m[0],m[1],m[2]] = i
  return newmat


def gen_plane(vec, sshow=True):
  if len(vec) != 3:
    print('Error: a vector is defined by 3 values: i,j,k')
  else:
    plan = []
    xs = np.linspace(vec[0]-10,vec[0]+10)
    ys = np.linspace(vec[1]-10,vec[1]+10)
    def solve(vec, x, y):
      return -((x*vec[0] + y*vec[1])/vec[2])
    for m in xs:
      for n in ys:
        plan.append([m,n,solve(vec,m,n)])
    lin = [[v*i for v in vec] for i in range(-10,10)] 
    if sshow == True:
      import matplotlib.pyplot as plt
      from mpl_toolkits.mplot3d import Axes3D
      fig = plt.figure()
      ax = fig.add_subplot(111, projection='3d')
      for p in plan:
        ax.scatter(p[0],p[1],p[2], c='b', edgecolor='b', alpha=0.1)
      for p in lin:
        ax.scatter(p[0],p[1],p[2], c='r', edgecolor='r')
      plt.show()
    return plan


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
  pool = Pool()
  darr = pool.map(load_fake_segment, fils) # 
  pool.close()
  pool.join()
  return darr # return multi-dimensional array


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
  
  def return_cross_section(vec, darr, voxel):
    plancoords = gen_plane(vec, False)
    voxelcoords = matrix2coords(darr, voxel)
    vdist = dist([0,0,0],voxel)
    cross_sec = []
    for p in plancoords:
      if return_nearest_dist(p, voxelcoords) <= vdist:
        cross_sec.append(p)
    return cross_sec
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
    cross_secs.append(return_cross_section(get_vector(skelcoords[s],
                                                      skelcoords[s+1]),
                                                      carr, voxel))
  
    
  
  return narr
  
  



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
 







