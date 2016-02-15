# Stuff for Maria


# Make sure we're in python 2.7
import sys
if sys.version_info.major > 2:
  print('Need to run with python 2.7 (for PyMC capability)!!')
else:
  import pymc as pm
  
  
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import subprocess



def containsSpikes(filelist):
  """
  Given a txt file which is a list of .csv files, this returns the
  files that have valid spikes (>= 1 spike) and the files that do not.
  """
  fnames, numlines = [], []
  # Load the filenames
  with open(filelist, 'r') as fIn:
    for line in fIn:
      if line:
        fnames.append(line.split(None)[0])
  
  # Run through the filenames and get the number of lines
  for nam in fnames:
    # runstr = 'wc -l %s' %nam
    p = subprocess.Popen(['wc', '-l', nam],
        stdout=PIPE, stderr=PIPE, stdin=PIPE)
    out = p.stdout.read()
    try:
      numlines.append(int(out.split(None)[0]))
    except:
      numlines.append(0)
  # print(numlines[:10])
  
  # Find the min (as long as it's not zero)
  minlines = np.inf
  for n in range(len(numlines)):
    if numlines[n] < minlines and numlines[n] != 0:
      minlines = numlines[n]
  # And replace that and min with nan, export the resulting list
  newlines = [1 if i > minlines else 0 for i in numlines]
  # print(newlines[:10])
  
  return [fnames[u] for u in range(len(newlines)) if newlines[u] > 0]



def showprofile(csvfile, color='rand'):
  """
  Show all properties of the data frame. The fields listed below
  are ignored.     
  """
  ignore = ['n1List', 'n2List', 'maxVtms', 'maxVinds', 'maxDerivtms', 
     'maxDerivinds', 'minDerivtms', 'minDerivinds', 'preMintms', 'preMininds',
     'postMintms', 'postMininds', 'preMaxCurvetms', 'preMaxCurveinds',
     'postMaxCurvetms', 'postMaxCurveinds', 'times']
  f = pd.read_csv(csvfile)
  columns = [col for col in f.columns if col not in ignore and len(col.split(None)) == 1]
  ncol = int(len(columns)/2.) + 1
  if color is 'rand':
    color = np.random.random(3)
  
  # Plot these mofos
  fig = plt.figure()
  plots = [fig.add_subplot(2, ncol, i+1) for i in range(len(columns))]
  for col in range(len(columns)):
    try:
      plots[col].hist(f[columns[col]].dropna(), bins=50, facecolor=color,
                      edgecolor='none', alpha=0.5)
    except:
      print(columns[col])
    plots[col].set_title(columns[col])
    plots[col].set_ylim()
  plt.show()
  return



def stateChange(csvfile, nstates=2, show=False):
  """
  Assuming the neuron changes states once, find the most likely time
  that change occurred and what the values were before and after.
  nstates = number of non-distinct states; tonic -> burst -> tonic = 3, not 2.
  """
  
  
  
#























