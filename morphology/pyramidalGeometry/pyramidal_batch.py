# pyramidal_batch.py - eliminate redundant segments for an entire directory
# usage: python pyramidal_batch.py directory(str)

import sys, os
import json
from pyramidal_readExportedGeometry import *
from neuron_getProperties import *



def getGeofiles(folder):
  """
  Return the list of hoc files.
  """
  hoclist = os.listdir(folder)
  hoclist = [h for h in hoclist if h.split('.')[-1]=='hoc']
  hoclist = [folder+h for h in hoclist]
  geofiles = [demoReadsilent(h) for h in hoclist]
  return geofiles



def saveDict(dic, outfile, ks='fileNames'):
  """
  Save a dic as a matlab-friendly array. ks is the key (beginning of
  each row)
  """
  written = []
  with open(outfile, 'w') as fOut:
    for u in range(len(dic[ks])):
      for key in dic.keys():
        if key != ks:
          thing = [str(dic[ks][u])]
          for k in dic[key][u]:
            thing.append(str(k))
          # Write this to the file
          fOut.write(' '.join(thing))
          fOut.write('\n')
          written.append(key)
  written = list(set(written))
  for w in written:
    print('%s written to %s' %(w, outfile))
  return




def runBatch(folder, save=False):
  """
  Run the analysis on the passed folder retain the properties in keep
  """
  geofiles = getGeofiles(folder)
  branchAngles = [branch_angles(g) for g in geofiles]
  p_ = [path_lengths2(g) for g in geofiles]
  pathLengths = [p[0] for p in p_]
  pathTorts = [p[1] for p in p_]
  sholl = [hooser_sholl(g)[0] for g in geofiles]
  shollDistances = [s[0] for s in sholl]
  shollCounts = [s[1] for s in sholl]
  asymmetry = [path_asymmetry(g)[0] for g in geofiles]
  
  randomDistances = []
  for g in geofiles:
    try:
      randomDistances.append(neuropil_fit(g))
    except:
      randomDistances.append(['nan'])
  b_ = [branch_lengths(g) for g in geofiles]
  branchLengths = [b[0] for b in b_]
  branchLocations = [b[1] for b in b_]
  branchOrder = [branch_order(g) for g in geofiles]
  
  props  =  {'fileNames': [g.name for g in geofiles],
             'branchAngles': branchAngles,
             'pathLengths': pathLengths,
             'pathTortuosities': pathTorts,
             'shollDistances': shollDistances,
             'shollCounts': shollCounts,
             'asymmetry': asymmetry,
             'randomDistances': randomDistances,
             'branchLengths': branchLengths,
             'branchLocations': branchLocations,
             'branchOrder': branchOrder }
  
  if save: # save should be the root name
    for p in props.keys():
      tempname = save+'_'+str(p)+'.txt'
      saveDict({'fileNames': props['fileNames'], p: props[p]},
               tempname, 'fileNames')
  return props
  




if __name__ == '__main__':
  args = sys.argv
  runBatch(args[1])
