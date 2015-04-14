import numpy as np
import calendar as cal
from itertools import cycle
import json



class Calendar():
  def __init__(self, calfile=None, year=2015, startmonth=1):
    self.weekdays = ['sun','mon','tues','wed','thur','fri','sat']
    self.months = ['jan','feb','mar','apr','may','jun','jul','aug','sep',
                   'oct','nov','dec'][startmonth:]
    self.year, self.startmonth = year, startmonth
    self.sorted_days = {} # a dict of sorted lists of each day
    self.calfile, self.newcal = calfile, 'revised_calendar.txt'
    if self.calfile is None:
      self.calendar = {}
      self.gen_calendar()
    else:
      calin = open(self.calfile)
      self.calendar = json.load(calin)
    self.sort_days()
    # stuff
    
    #self.save_cal()
    return 


  def add_to_calendar(self, day, month, year, dow, eve=True, away=[]):
    calkey = '%i%s%i' %(day, month, year)
    self.calendar[calkey] = {'day': day, 'month': month, 'year': year,
                        'dow': dow, 'eve': eve, 'away': away,
                        'valid': True, 'MLM': None, 'JC': None}
                        # eve = True, Eve can attend
                        # away = list of people who cannot attend
                        #        should be initials in lower case
                        # valid = True, not a holiday or some conflict
                        # 
    return self



  def gen_calendar(self):
    # generate a default calendar from a start date
    endmonth = 12
    for m in range(1,len(self.months)+1):
      kal = cal.monthcalendar(self.year, m)
      # for each week
      for w in kal: 
        # for each day
        for d in range(len(w)): # day is index, not object
          if w[d] != 0:
            dow, month, day = self.weekdays[d], self.months[m-1], w[d]
            self.add_to_calendar(day, month, self.year, dow)
    # calendar created for rest of year as dict objects
    return self



  def sort_days(self, startdate=None):
    # returns only valid members of a day-of-week starting at startdate
    for d in self.weekdays:
      # for each new day, re-initialize
      l, temp, counts = [], [], []
      #_months = cycle(self.months)
      for m in self.months:
        for k in self.calendar.keys():
          if self.calendar[k]['dow'] == d and self.calendar[k]['month'] == m:
            temp.append(k)
            counts.append(self.calendar[k]['day'])
        # got all the given days for that month, now sort by day number
        counts.sort()
        for c in counts:
          for t in temp:
            if self.calendar[t]['day'] == c:
              l.append(t)
      # got all the days in all months for that particular day of week
      if startdate:
        l = l[l.index(startdate):]
      self.sorted_days[d] = l
    return self

        
        
  def sort_daylist(self, item=None, prettyprint=False):
    # sorts a list of days regardless of the day
    newlist, temp, counts = [], [], []
    if item in 
    for m in self.months:
      for k in l:
        if self.calendar[k]['month'] == m:
          temp.append(k)
          counts.append(self.calendar[k]['day'])
      # got all the days for that month
      counts.sort()
      for c in counts: # days of month
        for t in temp: # actual calendar keys
          if self.calendar[t]['day'] == c:
            newlist.append(t)
    # got all the months
    if prettyprint:
      for k in newlist:
        thing = self.calendar[k]
        if item in self.calendar[k].keys():
          print('%s did %s on %s %i, %i'
                %(thing[item], item, thing['month'], thing['day'], self.year))
        else:
          print('%s did %s on %s %i, %i'
                %(thing['month'], thing['day'], self.year))
    return self, newlist



  def update_cal(self):
    # reflects validity based on eve and number of people there
    for k in self.calendar.keys():
      if self.calendar[k]['eve'] == False:
        self.calendar[k]['valid'] = False
      if len(self.calendar[k]['away']) > 0:
        for i in self.calendar[k]['away']:
          if self.calendar[k]['away'].count(i) > 1:
            self.calendar[k]['away'].pop(self.calendar[k]['away'].index(i))
      if len(self.calendar[k]['away']) > 4:
        self.calendar[k]['valid'] = False
    return self




  def eve_away(self, raw=None):
    # manually or list-input eve's away dates as 'ddmmmyyyy' eg: 23apr2015
    if raw is None:
      away_dates = []
      # use raw
      print('Enter date or z to finish.')
      go = True
      while go:
        f = input('Next date: ')
        if len(f) < 8 and f != 'z':
          print('Bad date. Format is ddmmyyyy, eg: 23apr2069. z to finish.')
        elif f == 'z':
          go = False
        else:
          away_dates.append(f)
    # now update the schedule for eve
    print('Adding Eves %i away dates to self.calendar' %len(away_dates))
    go = input('Proceed? y/n')
    if go not in ['y', 'n']:
      print('Invalid response. Type y or n')
      go = input('Proceed? y/n')
    if go != 'y':
      print('Aborting')
      return self.calendar
    # otherwise, add the dates
    for e in away_dates:
      if e not in self.calendar.keys():
        print('Date %s missing from self.calendar' %e)
      else:
        self.calendar[e]['eve'] = False
    print('Done updates Eves status')
    return self



  def change_day(self, key, eve=None, away=None, valid=None, JC=None, MLM=None):
    # can be used to add a holiday, change eve's status, or add away person
    # or add an event
    if key not in self.calendar.keys():
      print('Warning: %s not found in Calendar.' %key)
      return
    if away is not None:
      self.calendar[key]['away'].append(away)
    if JC is not None:
      self.calendar[key]['JC'] = JC
    if MLM is not None:
      self.calendar[key]['MLM'] = MLM
    if eve is not none:
      self.calendar[key]['eve'] = eve
    if valid is not None:
      self.calendar[key]['valid'] = valid
    print('Entry %s changed to reflect edit.' %key)
    return self



  def set_MLM(self, rota=None, startdate=None):
    # marder lab meetings happen on fridays; this will set all lab meetings
    self.update_cal()
    fridays = self.sorted_days['fri']
    if startdate is None:
      startdate = fridays[0]
    else:
      fridays = fridays[fridays.index(startdate):]
    if rota is None:
      rota = ['gd','vr','ah','jg','sh','pr','as','mg','ao','dp','to']
    _rota = cycle(rota)
    ind = fridays.index(startdate)
    for f in fridays[ind:]:
      self.calendar[f]['MLM'] = next(_rota)
    return self



  def set_JC(self, rota=None, startdate=None):
    # ML JC happens on wednesday; this sets ALL JCs
    self.update_cal()
    wednesdays = self.sorted_days['wed']
    if startdate is None:
      startdate = wednesdays[0]
    else:
      wednesdays = wednesdays[wednesdays.index(startdate):]
    if rota is None:
      rota = ['gd','vr','ah','jg','sh','pr','as','mg','ao','dp','to']
    _rota = cycle(rota)
    ind = wednesdays.index(startdate)
    for f in fridays[ind:]:
      self.calendar[f]['JC'] = next(_rota)
    return self



  def point_mutation(self, date, key, change):
    # given a certain date (cal key) and an item (eve, JC, etc), change it change
    if date in self.calendar.keys():
      if key in self.calendar[date].keys():
        self.calendar[date][key] = change
        print('Point mutation made.')
    return self



  def shift_item(self, date, key, insert=None):
    # this shifts all events of a certain key by one week; insert can add
    # something in that place, if None then it just slides everyone back
    if key == 'JC':
      days = self.sorted_days['wed']
    elif key == 'MLM':
      days = self.sorted_days['fri']
    days = days[days.index(date):]
    moved, current = None, insert
    for d in days:
      if self.calendar[d][key]:
        moved = self.calendar[d][key]
      self.calendar[d][key] = current
      current = moved
    return self
  
  
  
  def MLM_schedule(self):
    self.update_cal()
    # get all MLM days regardless of what day they actually fall on
    MLMdays = []
    for k in self.calendar.keys():
      if self.calendar[k]['MLM']:
        if type(self.calendar[k]['MLM']) is str:
          MLMdays.append(k)
    # now sort?
    ######## in progress #########
    return
  
  
  

  def save_cal(self):
    calout = open(self.newcal, 'w')
    json.dump(calout, self.calendar)
    return 








