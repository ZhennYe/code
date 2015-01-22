import cv2
import numpy as np

def make_binary(image):
  """
  this function preserves the 2D shape of np array
   ....:     but makes it binary; more common number -> 0, 
   ....:     others -> 1, 2, etc
   assumes structure: [row, col][1x3]
  """
  if isinstance(image, basestring) == True:
    ary = cv2.imread(image)
  else:
    ary = image
    
  nrows, ncols = len(ary), len(ary[1])
  vals = {}
  curVal = -1
  newAry = np.zeros([nrows, ncols])
  
  for j in xrange(nrows):
    for k in xrange(ncols):
      cpVal = None
      curRGB = ary[j,k]
      
      if curRGB in vals.values():
        for key, val in vals.iteritems():
          if val == curRGB:
            cpVal = key
      else:
        curVal = curVal + 1
        vals[curVal] = curRGB
        cpVal = curVal
      
      newAry[j,k] = cpVal
    # all cols in row done
  # all rows done
  
  counts = [newAry.count(i) for i in xrange(curVal+1)]
  newvals = []
  for i in xrange(len(counts)):
    newvals.append(counts.index(np.max(counts)))
    

