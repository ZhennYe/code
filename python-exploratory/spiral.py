# spiral.py -- this spirals in a space-filling way

import numpy as np

class Cross(): # cross-scetion
  def __init__(self, arr):
    self.arr = np.array(arr)
    self.shape = np.shape(self.arr)
    self.area_pts = []
    self.area = 0
    self.surface = 0
    self.live_pts = []
    self.dead_pts = []
    self.surface_points = []
    self.pos = self.set_start()
    self.x, self.y = None, None
    self.prev = None
    self.target = self.arr[self.pos[0], self.pos[1]]
    self.no_change = {'prev_dead_pts': None, 'prev_live_pts': None,
                      'count': 0}
    self.spiral_control()

    # here do stuffs
  
  
  def set_start(self):
    adjx = 1 if self.shape[0] % 2 == 0 else 0
    adjy = 1 if self.shape[1] % 2 == 0 else 0
    return [int(i) for i in [self.shape[0]/2-adjx, self.shape[1]/2-adjy]]
  
  
  def no_changes(self):
    
    # assess whether spiral process is done
    if self.no_change['prev_dead_pts'] is None:  # set prev dead pts
      self.no_change['prev_dead_pts'] = len(self.dead_pts)
    elif self.no_change['prev_dead_pts'] == len(self.dead_pts): # increment count
      self.no_change['count'] = self.no_change['count'] + 0.5
      print('Incremented delta to %.1f' %self.no_change['count'])
    else:
      self.no_change['prev_dead_pts'] == len(self.dead_pts) # set new prev dead pts
    
    if self.no_change['prev_live_pts'] is None:  # set prev live pts
      self.no_change['prev_live_pts'] = len(self.live_pts)
    elif self.no_change['prev_dead_pts'] == len(self.live_pts): # increment count
      self.no_change['count'] += 0.5
      print('Incremented delta to %.1f' %self.no_change['count'])
    else:
      self.no_change['prev_live_pts'] = len(self.live_pts) # set new prev live pts
    print(self.no_change)
    return self.no_change['count']
        
  
  
  # directional functions
  move_up = lambda x,y: [x,y-1]
  move_down = lambda x,y: [x,y+1]
  move_right = lambda x,y: [x+1,y]
  move_left = lambda x,y: [x-1,y]
  
  
  
  def check_next(self):
    check = {'right': move_down,      #    -------->
             'left': move_up,         #    ^   x-->
             'up': move_right,        #    |       |
             'down': move_left}       #    <-------  
    next_pt = check[self.prev](self.x, self.y) # get next pt
    if next_pt not in self.dead_pts and next_pt not in self.live_pts:
      if next_pt[0] in range(0,self.shape[0]) and next_pt[1] in range(0,self.shape[1]):
        print(check[self.prev](self.x,self.y))
        print('Next move OK')
        print(self.live_pts)
        return [True, check[self.prev](self.x,self.y)]
      else:
        return [False]
    else:
      print('Next move not OK')
      return [False]
  
    
  
  
  def default_dead_pts(self):
    # make non-target points dead
    for i in range(self.shape[0]):
      for j in range(self.shape[1]):
        if self.arr[i,j] != self.target:
          self.dead_pts.append([i,j]) 
    print('Starting with %i live points and %i dead points' 
          %(len(self.live_pts),len(self.dead_pts)))
    return self
  
  
  def check_live_pt(self, i,j):
    # determine if point should be dead or alive
    dead = 0
    for fn in [move_down, move_up, move_right, move_left]:
      if fn(i,j) in self.dead_pts or fn(i,j) in self.live_pts:
        dead = dead+1 
    if dead == 4 and [i,j] not in self.dead_pts:
      self.dead_pts.append([i,j]) # add it to dead points
      try:
        ind = self.live_pts.index([i,j])
        self.area_pts.append(self.live_pts.pop(ind))
        
      except:
        print('[%i,%i] not in self.live_pts' %(i,j))
      print('Live points: %i, dead points: %i' %(len(self.live_pts),
                                                 len(self.dead_pts)))
      return False
    elif dead == 4 and [i,j] in self.dead_pts:
      return False
    else:
      return True
        
    
  
  def log_pt(self): 
    # log and check if point is dead
    print('Trying to add point %i %i' %(self.x, self.y))
    if self.arr[self.x,self.y] == self.target: # double check
      self.area = self.area + 1
      if self.check_live_pt(self.x, self.y): # if point is alive, add it
        if [self.x,self.y] not in self.live_pts:
          self.live_pts.append([self.x,self.y])
          print('Added point %i %i. Live points: %i' %(self.x, self.y,
                                                     len(self.live_pts)))
      # else it is automatically added to dead_pts
  
  
  def retire_pts(self):
    # if a point is surrounded on all sides by active or dead points
    # (or both), make it dead
    # print(self.shape)
    for i in range(self.shape[0]):
      for j in range(self.shape[1]):
        if not self.check_live_pt(i,j):
          print('Live points: %i, dead points: %i' %(len(self.live_pts),
                                                     len(self.dead_pts)))
    
        
  
  
  def spiral(self):
    # continue to spiral until an edge is hit; then try other directions
    # when all directions hit an edge, move to another point
    from itertools import cycle
    self.area = self.area + 1
    self.x, self.y = self.pos[0], self.pos[1]
    self.prev = 'up' # set fake previous move
    self.log_pt()
    moves = [move_down, move_left, move_up, move_right]
    prevs = ['right','down','left','up']
    _moves = cycle(moves)
    _prevs = cycle(prevs)
    delta = self.no_changes()
    # cycle through points
    while len(self.live_pts) > 2 or delta < 20:
      count = 0
      self.retire_pts()
      delta = self.no_changes()
      print('Delta is %.1f' %delta)
      # take a point until exhausted
      self.x, self.y = self.live_pts[0]
      print('Starting new point %i %i' %(self.x, self.y))
      while count < 4: # if next exists, go to it
        if self.check_next()[0]:
          # print(self.check_next())
          try:
            self.x, self.y = self.check_next()[1][0], self.check_next()[1][1]
          except:
            self.x, self.y = self.check_next()[1][0][0], self.check_next()[1][0][1]
          self.log_pt()
          count = 0 # reset count, found a new point
          
          next_move = next(_moves)
          self.prev = next(_prevs)
        else:
          next_move = next(_moves)
          self.prev = next(_prevs)
          count = count + 1
          if count ==4:
            print(count)
          # otherwise, check the others and increase count
  
  
  def spiral_control(self):
    # control the class
    self.default_dead_pts()
    self.spiral()
    print(self.dead_pts)
    return self
        
        
        














