# Construct a NEURON model from the geo object and the Quadfit object
# usage: python2 morpho_Neuron.py hocfile (options)


import sys, os
import neuron, nrn
from quaddiameter import *
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import animation
from neuron import h
import PyNeuronToolbox
from PyNeuronToolbox.morphology import shapeplot
from PyNeuronToolbox.record import ez_record, ez_convert



def get_quadObject(hocfile):
  return Quadfit(hocfile)



def show_plot(quad):
  """
  Plot the thing.
  """
  h('xopen(quad.hocfile)')
  plt.figure(figsize=(7,7))
  shapeax = plt.subplot(111, projection='3d')
  shapeax.view_init(75,66)
  shapeplot(h,shapeax)
  plt.show()
  return



def passive_cable(stimloc=1., stimdur=5., show=False):
  """
  This simulates a pulse injected into a simple passive cable.
  """
  # Set up model
  h.load_file('stdrun.hoc')
  cell = h.Section()
  cell.nseg = 11   # It is a good idea to have nseg be an odd number
  cell.Ra = 35.4   # Ohm*cm
  cell.insert('pas')
  
  # create 3d structure
  h.pt3dadd(0,0,0,1.0,sec=cell)
  h.pt3dadd(1732,1732,1732,1.0,sec=cell)
  # Specify current injection
  stim = h.IClamp(stimloc,sec=cell) # Stim @ 1th end of segment
  stim.delay = 5   # ms
  stim.dur = stimdur   # ms
  stim.amp = 0.2   # nA
  print("Stim: %.2f nA, %.2f ms, at location %.2f" %(stim.amp, stim.dur,
                                                     stimloc))
  
  # Segment positions, equall spaced from 0 to 1
  seg_positions = np.linspace(0,1,cell.nseg)
  # Use toolbox to record v
  # ez_record records v in all compartments by default
  (v,v_labels) = ez_record(h)
  # Manually record time and current
  t, I = h.Vector(), h.Vector()
  t.record(h._ref_t)
  I.record(stim._ref_i)
  
  # Run the simulation 
  h.init()
  h.tstop = 30
  h.run()
  # Use toolbox convert v into a numpy 2D array
  v = ez_convert(v) 
  
  # Plotting options
  if show:
    fig = plt.figure()
    for i in range(cell.nseg):
      t = [v[u][i] for u in range(len(v))]
      ax = fig.add_subplot(cell.nseg, 1, i+1)
      ax.plot(t)
    plt.show()
  return h, v



def return_soma_seg(quad, h):
  # Return the soma segment
  for sec in list(h.allsec()):
    if sec.name() == quad.geo.soma.name:
      return sec



def passive_soma(quad, show=False):
  """
  Creates the model with basic pyramidal passive properties.
  """
  # Load the hoc into neuron
  h('xopen(%s)' %quad.hocfile)
  h.load_file('stdrun.hoc')
  seclist = list(h.allsec())
  for sec in seclist:
    sec.insert('pas')
    sec.Ra = 200.
  
  # Current injection into soma or tip
  soma_sec = return_soma_seg(quad, h)
  stim_loc = 0.
  stim = h.IClamp(stim_loc, sec=soma_sec)
  stim.delay = 1 # ms
  stim.dur = 1 # ms
  stim.amp = 20 # nA
  
  # Run sim and record data
  (v, labels) = ez_record(h) # PyNeuron to record all compartments
  t, I = h.Vector(), h.Vector()
  t.record(h._ref_t)
  I.record(stim._ref_i)
  h.init()
  h.tstop = 10 # s?
  h.run()
  v = ez_convert(v) # Convert v to numpy 2D array
  
  # If show, plot, else just return v
  if show:
    
























