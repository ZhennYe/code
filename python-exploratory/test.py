def hangman(word):
  alpha = 'abcdefghijklmnopqrstuvwxyz'
  sofar = '_'*len(word)
  def show_guess(sofar): # Show the guess
    print('So far, computer has guessed: %s' %sofar)
    return
  def add_char(char, pos, sofar): # Add new character
    newsofar = ''
    for s in range(len(sofar)):
      if s != pos:
        newsofar = newsofar + sofar[s]
      else:
        newsofar = newsofar + char
    return newsofar
  while sofar != word:
    a = alpha[int(np.random.random(1)*26)]
    for char in range(len(sofar)):
      if sofar[char] == '_': # if it hasn't been guessed
        if word[char] == a:
          sofar = add_char(a, char, sofar)
          show_guess(sofar)
          break
    show_guess(sofar)
  return



#########################################################################


def rad_vs_somadist(geo):
  rad, dist = [], []
  pDF = PathDistanceFinder(geo, geo.soma)
  for s in geo.segments:
    rad.append(s.avgRadius)
    dist.append(pDF.distanceTo(s))
  return rad, dist
  

#########################################################################



def polar_bar():
  N = 20
  thetas = np.linspace(np.pi/2, np.pi, N)
  rads = np.random.random(N)
  width = np.pi/(2*N)
  #
  ax = plt.subplot(111, polar=True)
  bar = ax.bar(thetas, rads, width=width, bottom=4.)
  [b.set_facecolor('deeppink') for b in bar.patches]
  #[b.set_alpha(0.5) for b in bar.patches]
  [b.set_edgecolor('deeppink') for b in bar.patches]
  print(len(bar.patches))
  plt.show()
  return



def circular_hist(angles, labelsin, title=None):
  """
  # IMPORT DEPENDENCIES FROM TOP.
  """
  def to_radians(angs):
    return [i*np.pi/180. for i in angs]
  def r_bin(bins, target): # Return the target bin value, always start from below
    j = [i for i in bins]
    for i in j:
      if i > target:
        return i
  def angulize(angs, nbins=100): # Do everything for the plotting except plot
    if max(angs) > 2*np.pi:
      angs = to_radians(angs)
    rads, thetas_b = np.histogram(angs, bins=nbins)
    width = np.pi/(4*nbins)
    # Normalize hist height and center the bins
    rads = [i/max(rads) for i in rads]
    thetas = [(thetas_b[i]+thetas_b[i+1])/2. for i in range(len(thetas_b)-1)]
    q25, q75 = np.percentile(angs, [25, 75])
    b25, b75 = r_bin(thetas, q25), r_bin(thetas, q75)
    return rads, thetas, width, b25, b75
  # If it's just one object, plot it simply; else nest the lists
  if type(angles[0]) is not list:
    angles = [angles]
  # Else, create the nested plots
  colors = ['darkkhaki', 'royalblue', 'forestgreen','tomato']
  ax = plt.subplot(111, polar=True)
  for A in range(len(angles)):
    rads, thetas, width, t25, t75 = angulize(angles[A])
    bar = ax.bar(thetas, rads, width=width, bottom=2.+2*A)
    [b.set_facecolor(colors[A]) for b in bar.patches]
    [b.set_edgecolor(colors[A]) for b in bar.patches]
    iqr = ax.bar(np.linspace(t25, t75, 100), np.ones(100)*1.5, 
                 width=np.pi/(400), bottom=2.+2*A)
    [i.set_facecolor(colors[A]) for i in iqr.patches]
    [i.set_alpha(0.3) for i in iqr.patches]
    [i.set_linewidth(0.) for i in iqr.patches]
    mean = ax.bar(np.mean(angles), 1.5, width=np.pi/400, bottom=2.+2*A)
    med = ax.bar(np.median(angles), 1.25, width=np.pi/400, bottom=2.+2*A)
    k=['k','orange']
    for m in [med.patches[0], mean.patches[0]]:
      m.set_facecolor(k[[med.patches[0], mean.patches[0]].index(m)])
      m.set_linewidth(0.)
  # legend
  khaki_patch = mpatches.Patch(color='darkkhaki', 
                label=labelsin[0])
  patches = [khaki_patch]
  if len(angles) > 1:
    royal_patch = mpatches.Patch(color='royalblue', 
                  label=labelsin[1])
    patches.append(royal_patch)
  if len(angles) > 2:
    forest_patch = mpatches.Patch(color='forestgreen', 
                  label=labelsin[2])
    patches.append(forest_patch)
  if len(angles) > 3:
    lavender_patch = mpatches.Patch(color='tomato', 
                  label=labelsin[3])
    patches.append(lavender_patch)
  plt.legend(handles=patches, loc=4)
  # title
  if title:
    ax.set_title(title, fontsize=20)
  ax.set_yticklabels([])
  plt.show()
  return


##################################################################################

def fix_fuckup(infile, outfile):
  with open(infile, 'r') as fIn:
    for line in fIn: # assumes one long-ass line to be split into 4-col space-sep lines
      sline = line.split(' ')
      nline = []
      for s in sline:
        g = s.split('.')
        if len(g) > 2: # has more than 1 number, do the work
          dec0 = g[1][:6] # requires %.6f
          num1 = g[1][6:]
          nline.append(g[0]+'.'+dec0)
          nline.append(num1+'.'+g[2])
        else:
          nline.append(s)
      with open(outfile, 'w') as fOut:
        for n in range(int(len(nline)/4)): # skip first
          fOut.write('%f ' %float(nline[(n*4)]))
          fOut.write('%f ' %float(nline[(n*4)+1])) # 
          fOut.write('%f ' %float(nline[(n*4)+2]))
          fOut.write('%f ' %float(nline[(n*4)+3]))
          fOut.write('\n')
  return
  
############################################################################

edges, nodes = [], []
with open('new_hoc.hoc', 'r') as fIn:
  for line in fIn:
    if line:
      splitLine = line.split(None)
      if type(splitLine) is list:
        if splitLine[0] == 'connect':
          e1 = splitLine[1].split('[')[1].split(']')[0]
          if e1 not in nodes:
            nodes.append(e1)
          e1 = e1 + '.' + splitLine[1].split('(')[1].split(')')[0]
          e2 = splitLine[2].split('[')[1].split(']')[0]
          if e2 not in nodes:
            nodes.append(e2)
          e2 = e2 + '.' + splitLine[2].split('(')[1].split(')')[0]
          edges.append([e1, e2])



somapos = [[5.39,1.81,125.6], [157.82,0.9,130.9], [18.06,-6,30],
[20.91,3.97,124.04], [136.28,-2.56,98.6], [135.97,5.21,94.05], 
[141.5,2.72,3], [76.05,2.9,155], [82.21,-7.79,119],
[142.92,-12.91,55.2], [113.39,9.32,45.25], [142.55,-2.93,101],
[159.19,0.016,190], [60.77,-24.49,72.12], [161.54,0.065,203.52],
[98.014,1.68,238.58]]

soma_cells = ['LG','LG','LG','LG','LP','LP','LP','LP','PD','PD','PD','PD','GM','PD','GM','GM']


frac_dim=[1.1627001304,1.3001412706,1.191728541,1.2790871821,1.3250156116,1.3595518406,1.1348792355,1.2340712067,1.333833344,1.3390849673,1.3182916312,1.3117662475,1.2091119931,1.2903322362,1.1847130274,1.39]




############################################################################

def get_sub_lengths(infile):
  sublen = {}
  temp, f = None, None
  with open(infile, 'r') as fIn:
    for line in fIn:
      if line:
        splitLine = line.split(None)
        if splitLine[0] == 'file':
          sublen[f] = temp
          temp, f = [], splitLine[-1]
        else:
          pah = int(splitLine[-1])
          if pah not in temp:
            temp.append(int(splitLine[-1]))
  return sublen




total_wire = [20492.7,21997.68,26331.426,30048.63,27976.96,5083,41533.64,23660.896,17543.044,30399.49,26037.479,20325.15,18579.34,14774.77,15478.95,25036.36843]
xlcells = ['GM','LG','LG','GM','GM','PD','LP','LG','PD','PD','LG','PD','LP','LP','PD','LP']
num_subtrees = [32,25,15,35,37,10,23,19,6,15,33,2,12,6,3,20]
path_lengths = [1507,944.681,1105.2,6849,529.239,699.93,432.96,832.83,640.62,677.559159,842.117,1050.98,916.85716,365.041,700.75]
path_percent = [0.0685,0.0359,0.0368,0.2448,0.10411,0.01685,0.0183,0.04747,0.02107,0.02602,0.04143,0.0566,0.006206,0.02358,0.02799]









