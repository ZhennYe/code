# pyramidal_batch.py - eliminate redundant segments for an entire directory
# usage: python pyramidal_batch.py directory(str)

import sys, os
import json
from pyramidal_traceback import *
from XmlToHoc_simple import *
from neuron_getProperties import *



def get_filelist(directory):
  all_files = os.listdir(directory)
  xml_files = [i for i in all_files if i.split('.')[1]=='xml']
  fnames = [i.split('.')[0] for i in xml_files]
  hoc_files = [i+'.hoc' for i in fnames]
  if len(xml_files) == len(hoc_files):
    return xml_files, hoc_files
  else:
    print('xml and hoc files do not match')
    return False


def get_hocfiles(directory):
  _, hoc_files = get_filelist(directory)
  return [directory+i for i in hoc_files]



def batch_sholl(dirs):
  list_of_sholls = []
  for d in dirs:
    files = os.listdir(d)
    files = [d+i for i in files]
    files = [i for i in files if i.split('.')[-1]=='hoc']
    fnames = [i.split('/')[-1] for i in files]
    geos = []
    for g in files:
      try:
        geo = demoRead(g)
      except:
        continue
      geos.append(geo)
    # geos = [demoRead(g) for g in files]
    sholl_dists = [hooser_sholl(g)[0][0] for g in geos]
    sholl_counts = [hooser_sholl(g)[0][1] for g in geos]
    sholl = {'files': fnames, 'shollDistances': sholl_dists,
             'shollCounts': sholl_counts}
    list_of_sholls.append(sholl)
  return list_of_sholls
    



def run_batch(directory):
  xml_files, hoc_files = get_filelist(directory)
  xml_files = [directory+i for i in xml_files]
  hoc_files = [directory+i for i in hoc_files]
  success = []
  # first convert xml to hoc
  for f in range(len(xml_files)):
    SkelHoc(xml_files[f], hoc_files[f])
  # now process redundant sections for each hoc file
  for f in hoc_files:
    try:
      remove_redundant(f)
      success.append(f)
    except:
      print('Could not remove for %s' %f)
  return success 


### continue to analysis part?
def get_properties(success, save=None):
  geo_files = []
  for f in success:
    try:
      geo_files.append(demoRead(f))
    except:
      pass
  branchAngles = [branch_angles(g) for g in geo_files]
  pathLengths = [path_lengths2(g)[0] for g in geo_files]
  pathTorts = [path_lengths2(g)[1] for g in geo_files]
  shollDistances = [interpolate_nodes(g) for g in geo_files]
  asymmetryLengths = [tips_asymmetry(g)[0] for g in geo_files]
  asymmetryTips = [tips_asymmetry(g)[1] for g in geo_files]
  simple_asymmetryLengths = [simplify_asymmetry(x,y)[0] for x, y in
                             zip(asymmetryLengths, asymmetryTips)]
  simple_asymmetryTips = [simplify_asymmetry(x,y)[1] for x,y in
                             zip(asymmetryLengths, asymmetryTips)]
  ellipsoidDistances = [get_distances(g) for g in geo_files]
  branchLengths = [branch_lengths(g)[0] for g in geo_files]
  branchLocations = [branch_lengths(g)[1] for g in geo_files]
  branchOrder = [branch_order(g) for g in geo_files]
  props  =  {'fileNames': success,
             'branchAngles': branchAngles,
             'pathLengths': pathLengths,
             'pathTortuosities': pathTorts,
             'shollDistances': shollDistances,
             'asymmetryLengths': asymmetryLengths,
             # 'asymmetryTips': asymmetryTips,
             'simple_asymmetryLengths': simple_asymmetryLengths,
             'simple_asymmetryTips': simple_asymmetryTips,
             'ellipsoidDistances': ellipsoidDistances,
             'branchLengths': branchLengths,
             'branchLocations': branchLocations,
             'branchOrder': branchOrder }
  if save:
    savefile = open(save, 'w')
    json.dump(props, savefile)
  return props



if __name__ == '__main__':
  args = sys.argv
  run_batch(args[1])
