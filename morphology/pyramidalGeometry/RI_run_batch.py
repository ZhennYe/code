

dir1 = '/home/acs/Data/vanhooser/RIgny/'
dir2 = '/home/acs/Data/vanhooser/RIgny2/'
dir3 = '/home/acs/Data/vanhooser/RIuxc/'
dir4 = '/home/acs/Data/vanhooser/RIxpc/'
dir5 = '/home/acs/Data/vanhooser/RIxpc2/'
dir6 = '/home/acs/Data/vanhooser/RIydy/'
dir7 = '/home/acs/Data/vanhooser/RIydy2/'

dirs = [dir1, dir2, dir3, dir4, dir5, dir6, dir7]

root1 = 'RIgny_'
root2 = 'RIgny2_'
root3 = 'RIuxc_'
root4 = 'RIxpc_'
root5 = 'RIxpc2_'
root6 = 'RIydy_'
root7 = 'RIydy2_'

roots = [root1, root2, root3, root4, root5, root6, root7]


def get_hocfiles(directory):
  files = os.listdir(directory)
  files = [i for i in files if i.split('.')[-1]=='hoc']
  files = [directory+i for i in files]
  return files
  


list_of_sholls = []
for d in range(len(dirs)):
  hocs = get_hocfiles(dirs[d])
  geos = []
  for g in hocs:
    try:
      geos.append(demoRead(g))
    except:
      continue
  sholl_dists = [hooser_sholl(g)[0][0] for g in geos]
  sholl_counts = [hooser_sholl(g)[0][1] for g in geos]
  files = [dirs[d]+i for i in hocs]
  sholl = {'distances': sholl_dists, 'counts': sholl_counts, 'files': files}
  save_dict(sholl, roots[d])

