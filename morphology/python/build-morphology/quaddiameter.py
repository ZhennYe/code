# quaddiameter.py - Using the quadratic tapering constants dervied
#                   by H Cuntz et al (TREES Toolbox), fit a quadratic
#                   taper to a given skeleton.
# usage: python quaddiameter.py hocFile fittedFile
# Here, the radii from hocFile are ignored; fittedFile is a hoc file
# with the same nodes but with different radii (quad-fit).


import os, sys
sys.path.append('/home/alex/code/morphology/python/functions/')
import numpy as np
import matplotlib.pyplot as plt
from read_exportedNeuronGeometry import *



def load_hoc(hocFile):
  return demoRead(hocFile)



def load_params():
  """
  Load the parameters for the fits.
  """
  P = []
  with open('P.txt','r') as fIn:
    for line in fIn:
      if line:
        splitLine = line.split(None)
        P.append([float(i) for i in splitLine])
  # All ldend values are tab or space-separated on one line
  ldend = []
  with open('ldend.txt', 'r') as fIn:
    for line in fIn:
      if line:
        splitLine = line.split(None)
        for s in splitLine:
          ldend.append(float(s))
  return P, ldend
  


def quadfit(hocFile):
  """
  Fit quadratic taper to the given neuron.
  """
  
  
  

























