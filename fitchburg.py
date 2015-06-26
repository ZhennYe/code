# get fitchburg delay

import sys, os
import pandas as pd
import copy


url = 'http://developer.mbta.com/lib/RTCR/RailLine_9.json'
url = 'http://developer.mbta.com/lib/RTCR/RailLine_9.csv'


class getTrains:
  def __init__(self, show=False):
    self.url = 'http://developer.mbta.com/lib/RTCR/RailLine_9.csv'
    self.data = pd.read_csv(self.url)
    self.stations = ['Brandeis/ Roberts', 'Belmont', 'Porter Square']
    self.inbound = {}
    self.outbound = {}
    self.report = []
    self.show = show
    for s in self.stations:
      self.inbound[s] = []
      self.outbound[s] = []
    self.flags = {'sch': 'No lateness data',
                  'pre': 'Data is accurate',
                  'app': 'Approaching station, location accurate',
                  'arr': 'Arriving at station, location accurate',
                  'dep': 'Departing station, location accurate',
                  'del': 'Train not moving' }
    # methods
    self.getRelevant()
    self.show_report()
    return
  
  
  def realTime(self, t0):
    # Convert 'epoch time' (seconds since midnight Jan 1 1970 GMT) to real
    # necessary? Can just subtract 'scheduled' from 'timestamp' for seconds
    # to next arrival...
    return self
  
  
  def getRelevant(self):
    # Return the trains approaching relevant stations within the hour
    currtime = self.data['TimeStamp'].iloc[0]
    for r in range(self.data.shape[0]):
      s = self.data.iloc[r]
      # If station is relevant
      if s['Stop'] in self.stations:
        # If within an hour
        if (s['Scheduled'] + s['Lateness']) - currtime < 3600:
          # Add to inbound or outbound
          if s['Destination'] == 'Fitchburg':
            self.outbound[s['Stop']].append(s)
          elif s['Destination'] == 'North Station':
            self.inbound[s['Stop']].append(s)
          else:
            print('Bad destination: %s' %str(s['Destination']))
    return self
    
  
  def p_report(self, ser): # must be a pd.Series
    mi = int((ser['Scheduled']+ser['Lateness']-ser['TimeStamp']) / 60)
    se = int((ser['Scheduled']+ser['Lateness']-ser['TimeStamp']) % 60)
    if self.show:
      print('Train %i to %s arriving at %s in %i min %i sec.\nStatus:'
            %(ser['Trip'], ser['Destination'], ser['Stop'], mi, se))
      print(self.flags[ser['Flag']])
    self.report.append('Train %i to %s arriving at %s in %i min %i sec.\nStatus:'
            %(ser['Trip'], ser['Destination'], ser['Stop'], mi, se))
    self.report.append(self.flags[ser['Flag']])
    return self
  
  
  def show_report(self):
    # Organize and report findings
    if self.show:
      # First inbound trains
      print('Inbound to Boston:')
      for k in self.inbound.keys():
        print('Station: %s' %k)
        for s in self.inbound[k]:
          self.p_report(s)
      # Outbound trains
      print('Outbound to Fitchburg:')
      for k in self.outbound.keys():
        print('Station: %s' %k)
        for s in self.outbound[k]:
          self.p_report(s)
    # Else, log report
    self.show = False
    # First inbound trains
    self.report.append('Inbound to Boston:')
    for k in self.inbound.keys():
      self.report.append('Station: %s' %k)
      for s in self.inbound[k]:
        self.p_report(s)
    # Outbound trains
    self.report.append('Outbound to Fitchburg:')
    for k in self.outbound.keys():
      self.report.append('Station: %s' %k)
      for s in self.outbound[k]:
        self.p_report(s)
    
    return
    
    

########################################################################
if __name__ == "__main__":
  g = getTrains()






