import numpy as np
import matplotlib.pyplot as plt

def gen_points(span=[0,5], n=10):
  def gen_pt(x=1):
    return np.random.random(3)+x
  pts =[]
  if len(span) < 2:
    span = [0, span]
  for i in range(span[0],span[1]):
    count = 0
    while count < n:
      pts.append(gen_pt(i))
      count = count + 1
  return pts
  

def gen_ellipse_pts(diam=[10,5], N=1000, show=False):
  xs = np.random.normal(diam[0]/2.,diam[0]/4.,N)
  ys = np.random.normal(diam[1]/2.,diam[1]/4.,N)
  if show == True:
    fig = plt.figure()
    ax = fig.add_subplot(111)
    ax.scatter(xs, ys, c='b',edgecolor='b', alpha=0.1)
    ax.axis('equal')
    plt.show()
  return np.array(list(zip(xs, ys)))


def bin_cross_section(pts, voxel=[0.1,0.1], show=False):
  xs = [p[0] for p in pts]
  ys = [p[1] for p in pts]
  xmin, xmax, ymin, ymax = min(xs), max(xs), min(ys), max(ys)
  h, xedges, yedges = np.histogram2d(xs, ys, bins=(int((xmax-xmin)/voxel[0]),
                                                   int((ymax-ymin)/voxel[1])))
  xcents = [np.mean([xedges[i],xedges[i+1]]) for i in range(len(xedges)-1)]
  ycents = [np.mean([yedges[i],yedges[i+1]]) for i in range(len(yedges)-1)]
  binary_pts = []
  for i in range(len(h)):
    for j in range(len(h[i])):
      if h[i,j] > 0:
        binary_pts.append([xcents[i], ycents[j]])
  if show == True:
    fig = plt.figure()
    ax = fig.add_subplot(111)
    for p in binary_pts:
      ax.scatter(p[0], p[1], c='b', edgecolor='b', alpha=0.5)
    ax.axis('equal')
    plt.show()
  return binary_pts
  
    
  


def subtract_mean(pts):
  means = []
  for i in range(3):
    means.append(np.mean([c[i] for c in pts]))
  return pts-means


def norm_eval(pts):
  pts = np.array(pts)
  if np.shape(pts)[0] != np.shape(pts)[1]:
    f_matrices = False
  else:
    f_matrices = True
  
  def normalize_eval(s, lenpts):
    return 2*s[0]/np.sqrt(lenpts)
  
  newpts = subtract_mean(pts)
  u, s, v = np.linalg.svd(pts, full_matrices=f_matrices) # might not work
  return normalize_eval(s, len(pts))


def fix_holes(matrix, runs=10):
  rows, cols = np.shape(matrix)[0], np.shape(matrix)[1]
  target = matrix[int(rows/2),int(cols)/2] # this should be part of neurite
  alt = None
  count = 0
  while count < runs:
    for r in range(1,rows-1): # omit first and last rows
      for c in range(1, cols-1):
        nebs = 0
        if matrix[r,c] != target: # check neighbors
          alt = matrix[r,c]
          if matrix[r-1,c] == target:
            nebs = nebs + 1
          if matrix[r+1,c] == target:
            nebs = nebs + 1
          if matrix[r,c-1] == target:
            nebs = nebs + 1
          if matrix[r,c+1] == target:
            nebs = nebs + 1
          if nebs >= 3:
            matrix[r,c] = target
        if matrix[r,c] == target: # check for bad ones
          if matrix[r-1,c] != target:
            nebs = nebs + 1
          if matrix[r+1,c] != target:
            nebs = nebs + 1
          if matrix[r,c-1] != target:
            nebs = nebs + 1
          if matrix[r,c+1] != target:
            nebs = nebs + 1
          if nebs >= 3:
            matrix[r,c] = alt
    count = count + 1
  print('Ran %i times' %count)
  return matrix
        

def show_matrix(mat):
  img = Image.fromarray(mat)
  fig = plt.figure()
  ax = fig.add_subplot(111)
  ax.imshow(img)
  plt.show()


def spiral(matrix):
  # assumes very center of matrix is origin
  return


class Test():
  def __init__(self,i=10):
    self.A = i
    self.B = None
    self.times()
  def times(self):
    self.B = self.A*100
    
