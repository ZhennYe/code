# Get some basic properties of the trace -- spikes, Hz, # bursts, IBI, duty cycle
import numpy as np


def get_t(v, dt=0.01):
  # Default is dt = 0.01 ms 100kHz (from HH_oscillator)
  return np.linspace(0,450,len(v))



def spike_times(v, t=None, vth=-10):
  """
  Return the spike times.
  """
  if t is None:
    t = get_t(v)
  tms = []
  for tt in range(len(v)-1):
    if v[tt+1] > vth and v[tt] < vth:
      tms.append(t[tt])
  return tms



def frequency(v, t=None, t_range=None):
  """
  Return the frequency for whole trace (t_range=None) or a specific
  time window. Also converts from ms to s.
  """
  if t is None:
    t = get_t(v)
  tms = spike_times(v, t)
  if t_range is None:
    return len(tms)/(max(t)/1000)
  elif len(t_range) == 2: # t_range should be like [10,100]
    t_tms = [i for i in tms if i>t_range[0] and i<t_range[1]]
    return len(t_tms)/((range[1]-range[0])/1000) 
  else:
    print('t_range must be None or list len()==2')
    return None



def burst_tms(v, t=None):
  """
  Get start and end times of each burst.
  """
  tms = spike_times(v, t)
  diff = [tms[i+1]-tms[i] for i in range(len(tms)-1)] # Beginning of spike
  h, b_e = np.histogram(diff, bins=2) # force bimodal distribution 
  bur_th = b_e[1] # Set burst threshold
  bur_ind = [i for i in range(len(diff)) if diff[i] > bur_th]
  bur_end = [tms[i] for i in bur_ind if i<len(tms)] # This gives END of burst
  bur_start = [tms[i+1] for i in bur_ind if (i+1)<len(tms)] # Burst START
  
  for i in range(len(tms)-1):
    if (tms[i+1]-tms[i]) > bur_th:
      # Start burst
      















