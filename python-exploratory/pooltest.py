# test the python pool 

import numpy as np
from timeit import default_timer as timer
from multiprocessing import Pool


def fake_voxels(N=1000000, R=10):
  voxels = []
  for I in range(R):
    voxels.append(np.random.rand(N/10)+I)
  return np.array(voxels).flatten()


def return_nearest(point, plist=list(range(10))):
  # 1-D only
  minp, refp = np.inf, None
  for p in plist:
    curr = abs(p-point)
    if curr < minp:
      minp = curr
      refp = plist.index(p)
  return refp


def assign_nearest(voxels, R=10):
  plist = list(range(R))
  segs = {'*':None}
  for v in voxels:
    refp = return_nearest(v)
    if str(refp) not in segs.keys():
      segs[str(refp)] = []
    segs[str(refp)].append(v)
  return segs


# this function is where map etc is added
def pool_assign_nearest(voxels, R=10, workers=8):
  pool = Pool(workers)
  plist = list(range(R))
  segs = {'*': None}
  results = pool.map(return_nearest, voxels)
  pool.close()
  pool.join()
  return results


def control(N=1000000,R=10):
  start = timer()
  voxels = fake_voxels(N,R)
  segs = assign_nearest(voxels,R)
  ttime = timer() - start
  print('Took %f seconds to run.' %ttime)
  return segs


def pool_control(N=1000000,R=10):
  start = timer()
  voxels = fake_voxels(N,R)
  segs = pool_assign_nearest(voxels,R)
  ttime = timer() - start
  print('Took %f seconds to run.' %ttime)
  return segs


if __name__ == '__main__':
  s = pool_control()
  s = control()
