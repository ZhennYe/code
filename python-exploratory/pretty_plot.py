# simply plotting suite

import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import numpy as np



def lims(V):
  # get upper and lower bounds on a LIST OF LISTS (or matrix) D=2
  # returns min, max
  mi, ma = np.inf, 0
  for m in V:
    for n in m:
      if n > ma:
        ma = n
      if n < mi:
        mi = n
  return mi, ma



def pretty_boxplot(V, labelsin, title=None, ticks=None):
  """
  V is a matrix of arrays to be plotted; labels must be same length.
  labels = ['PD', 'LG' ...], ticks = ['791_233', ...]
  ticks=False to omit x-ticks altogether; None for legend=ticks
  """
  mi, ma = lims(V)
  L = list(np.unique(labelsin))
  C = [L.index(i) for i in labelsin]
  fcolors = ['darkkhaki', 'royalblue', 'forestgreen','lavenderblush']
  # plotting
  fig = plt.figure()
  ax = fig.add_subplot(111)
  if ticks:
    box = ax.boxplot(V, labels=ticks, showmeans=True, notch=True, 
                     patch_artist=True)
    #ax.set_xticks([i-1 for i in range(len(ticks))])
    #ax.set_xticklabels(ticks, rotation=45) # rotate ticks
  elif ticks is False:
    box = ax.boxplot(V, labels=[' ' for i in range(len(labelsin))],
                     showmeans=True, notch=True, patch_artist=True)
  else:
    box = ax.boxplot(V, labels=labelsin, showmeans=True, notch=True, 
                     patch_artist=True, showfliers=False)
  for patch, color in zip(box['boxes'], [fcolors[i] for i in C]):
    patch.set_facecolor(color)
  # set y axis range
  # ax.set_ylim([mi, ma])
  # legend
  khaki_patch = mpatches.Patch(color='darkkhaki', 
                label=labelsin[C.index(fcolors.index('darkkhaki'))])
  royal_patch = mpatches.Patch(color='royalblue', 
                label=labelsin[C.index(fcolors.index('royalblue'))])
  forest_patch = mpatches.Patch(color='forestgreen', 
                label=labelsin[C.index(fcolors.index('forestgreen'))])
  lavender_patch = mpatches.Patch(color='lavenderblush', 
                label=labelsin[C.index(fcolors.index('lavenderblush'))])
  plt.legend(handles=[khaki_patch, royal_patch, forest_patch, lavender_patch])
  # title
  ax.set_title(title, fontsize=20)
  
  plt.show(); return



def pretty_scatter(V, labelsin, title=None):
  """
  
  """
  return
  


def pretty_bar(v, labelsin, stderr=None, ticks=None, title=None):
  """
  
  """
  #mi, ma = lims(v)
  L = list(np.unique(labelsin))
  C = [L.index(i) for i in labelsin]
  fcolors = ['darkkhaki', 'royalblue', 'forestgreen','lavenderblush']
  # plotting
  fig = plt.figure()
  ax = fig.add_subplot(111)
  if len(np.shape(v))>1:
    V = [np.mean(i) for i in v]
    Vstd = [np.std(i) for i in v]
  else:
    V=v
    Vstd=stderr
  num = len(V)
  
  # plotting
  if stderr is not None:
    for e in range(num):
      ax.bar(e,V[e], yerr=Vstd[e], color=fcolors[C[e]], ecolor='k')
  else:
    for e in range(num):
      ax.bar(e,V[e], color=fcolors[C[e]], ecolor='k', width=0.5)
  # legend
  khaki_patch = mpatches.Patch(color='darkkhaki', 
                label=labelsin[C.index(fcolors.index('darkkhaki'))])
  royal_patch = mpatches.Patch(color='royalblue', 
                label=labelsin[C.index(fcolors.index('royalblue'))])
  patches = [khaki_patch, royal_patch]
  if len(L) > 2:
    forest_patch = mpatches.Patch(color='forestgreen', 
                  label=labelsin[C.index(fcolors.index('forestgreen'))])
    patches.append(forest_patch)
  if len(L) > 3:
    lavender_patch = mpatches.Patch(color='lavenderblush', 
                  label=labelsin[C.index(fcolors.index('lavenderblush'))])
    patches.append(lavender_patch)
  plt.legend(handles=patches)
  if stderr is not None:
    ax.set_ylim([0, max(V)+np.mean(Vstd)])
  else:
    ax.set_ylim([0, max(V)+0.5*min(V)])
    ax.set_xlim([-.5, len(V)])
  # title
  if title:
    ax.set_title(title, fontsize=20)
  
  plt.show(); return  



def plot_cum_dist(V, labelsin, title=None):
  """
  Plot lines showing cumulative distribution, i.e. for Sholl.
  Labels must be len(V) (one label per array).
  """
  if max(V[0]) > 1:
    normed = []
    for i in V:
      M = max(i)
      normed.append([a/M for a in i])
    V = normed
  L = list(np.unique(labelsin))
  C = [L.index(i) for i in labelsin]
  fcolors = ['darkkhaki', 'royalblue', 'forestgreen','tomato']
  fig = plt.figure(); ax = fig.add_subplot(111)
  
  for i in range(len(V)):
    ax.plot(V[i], [a/len(V[i]) for a in range(len(V[i]))], color=fcolors[C[i]],
            linewidth=2)
    ax.plot(V[i], [a/len(V[i]) for a in range(len(V[i]))], color=fcolors[C[i]],
            linewidth=4, alpha=0.5)
  
  # legend
  khaki_patch = mpatches.Patch(color='darkkhaki', 
                label=labelsin[C.index(fcolors.index('darkkhaki'))])
  royal_patch = mpatches.Patch(color='royalblue', 
                label=labelsin[C.index(fcolors.index('royalblue'))])
  patches = [khaki_patch, royal_patch]
  if len(L) > 2:
    forest_patch = mpatches.Patch(color='forestgreen', 
                  label=labelsin[C.index(fcolors.index('forestgreen'))])
    patches.append(forest_patch)
  if len(L) > 3:
    lavender_patch = mpatches.Patch(color='tomato', 
                  label=labelsin[C.index(fcolors.index('tomato'))])
    patches.append(lavender_patch)
  plt.legend(handles=patches)
  # title
  if title:
    ax.set_title(title, fontsize=20)
  
  plt.show()
  


def line_scatter(xdata, ydata, labelsin=None, title=None, 
                 lines=True, groups=None, ax_titles=None):
  # a scatter plot with lines through the data
  # groups are a list of listed indices for which ydata belong together
  # i.e.: groups=[[0,1],[2,3]]
  colorlist = ['forestgreen','limegreen','royalblue','lightskyblue',
                'deeppink','orchid']
  markerlist = ['v','o','*','s']
  fig = plt.figure()
  ax = fig.add_subplot(222) # change this to 121 for legend outside fig
  
  if groups is not None:
    L = len(groups)
    print(L)
    cols = []
    for i in groups:
      cols.append(colorlist[i[0]])
      cols.append(colorlist[i[1]])
    marks = [markerlist[i] for i in range(2*L)]
  else:
    L = len(ydata)
    print(L)
    cols = [colorlist[i] for i in range(L)]
    marks = [markerlist[i] for i in range(L)]
    
  for i in range(len(ydata)):
    ax.scatter(xdata, ydata[i], c=cols[i], edgecolor=cols[i],
               marker=marks[i], s=40)
    ax.plot(xdata, ydata[i], c=cols[i], linewidth=3, alpha=0.2)
  
  forest_patch = mpatches.Patch(color='forestgreen', 
                  label=labelsin[1])
  green_patch = mpatches.Patch(color='limegreen',
                  label=labelsin[0])
  patches = [forest_patch, green_patch]
  if L > 2:
    royal_patch = mpatches.Patch(color='royalblue', 
                  label=labelsin[2])
    blue_patch = mpatches.Patch(color='lightskyblue',
                  label=labelsin[3])
    patches.append(royal_patch)
    patches.append(blue_patch)
  if L > 4:
    pink_patch = mpatches.Patch(color='deeppink', 
                  label=labelsin[4])
    orchid_patch = mpatches.Patch(color='orchid',
                  label=labelsin[5])
    patches.append(pink_patch)
    patches.append(orchid_patch) 
  # This can be useful for putting the legend outside the fig  vvv
  #plt.legend(handles=patches,loc='upper left')# bbox_to_anchor=(1.05,1), loc=2, borderaxespad=0.)
  if title:
    ax.set_title(title, fontsize=40)
  ax.spines['left'].set_position('zero')
  ax.spines['bottom'].set_position('zero')
  if ax_titles:
    ax.set_xlabel(ax_titles[0], fontsize=25)
    ax.xaxis.set_label_coords(0.5,0)
    ax.set_ylabel(ax_titles[1], fontsize=25)
    ax.yaxis.set_label_coords(0,0.5)
  plt.xticks([-3,3],fontsize=25)
  plt.yticks([-40,20],fontsize=25)
  plt.show()



def pretty_distribution(data, benchmark=None, bins=None, bars=False, title=None):
  # plots a pretty distribution of the Data, with or without a benchmark
  from scipy.interpolate import spline
  if bins is None:
    bins = len(data)/20
  hist, bin_edges = np.histogram(data, bins=bins)
  bin_e = [(bin_edges[i]+bin_edges[i+1])/2 for i in range(len(bin_edges)-1)]
  # interpolate for smoothed spline curve
  xnew = np.linspace(bin_e[0], bin_e[-1], 300)
  h_down = [np.mean(hist[i*4:i*4+4]) for i in range(int(len(hist)/4))] # maybe need -1 sometimes
  b_down = [np.mean(bin_e[i*4:i*4+4]) for i in range(int(len(bin_e)/4))]
  power_smooth = spline(b_down, h_down, xnew)
  fig = plt.figure()
  ax = fig.add_subplot(111)
  # draw curved line
  ax.plot(xnew, power_smooth, linewidth=2, c='royalblue')
  # fill under smoothed line or bars
  if not bars:
    ax.fill(xnew, power_smooth, facecolor='royalblue',alpha=0.4)
  else:
    ax.hist(data, bins=bins, edgecolor='lightskyblue', color='royalblue', alpha=0.4)
  if benchmark:
    ax.plot([benchmark, benchmark],[0,max(hist)*.8], color='deeppink', linewidth=3)
  if title:
    ax.set_title(title, fontsize=40)
  plt.show()



def pretty_line(xdata, ydata, labels=None, axlabels=None, title=None):
  if axlabels:
    if len(axlabels) != 2:
      print('Axlabels should have 2 items')
      axlabels=None
  colorlist = ['forestgreen','royalblue','deeppink','darkkhaki']
  fig = plt.figure()
  ax = fig.add_subplot(111)
  # use simple heuristics for determining if there are multiple y's
  if len(ydata) > 1 and len(ydata) <= 4 and len(ydata) != len(xdata):
    for y in range(len(ydata)):
      ax.plot(xdata, ydata[y], linewidth=2, c=colorlist[y])
      ax.plot(xdata, ydata[y], linewidth=3, c=colorlist[y], alpha=0.2)
  # if multiple x's and y's
  elif len(ydata) > 1 and len(ydata) <= 4 and len(ydata) == len(xdata):
    for y in range(len(ydata)):
      ax.plot(xdata[y], ydata[y], linewidth=2, c=colorlist[y])
      ax.plot(xdata[y], ydata[y], linewidth=3, c=colorlist[y], alpha=0.2)
  else:
    ax.plot(xdata, ydata, linewidth=2, c=colorlist[0])
    ax.plot(xdata, ydata, linewidth=3, c=colorlist[0], alpha=0.2)
  # simple legend !
  if labels:
    if len(labels) != len(ydata):
      print('Num labels must equal num ydata!')
    else:
      patches = []
      for y in range(len(labels)):
        patch = mpatches.Patch(color=colorlist[y], label=labels[y])
        patches.append(patch)
      ax.legend(handles=patches)
  if axlabels:
    ax.set_xlabel(axlabels[0], fontsize=15)
    ax.set_ylabel(axlabels[1], fontsize=15)
  if title:
    ax.set_title(title, fontsize=25)
  ax.set_ylim([-0.5,3.5])
  ax.set_xlim([-0.5,5.5])
  plt.show()




def pretty_dendrogram(nodes):

  import numpy as np
  from scipy.cluster.hierarchy import linkage
  import matplotlib.pyplot as plt

  linkage_matrix = linkage(nodes, "single")

  plt.figure()
  plt.clf()
  
  show_leaf_counts = True
  ddata = augmented_dendrogram(linkage_matrix,
                 color_threshold=1,
                 p=600,
                 truncate_mode='lastp',
                 show_leaf_counts=show_leaf_counts,
                 )
  plt.title("show_leaf_counts = %s" % show_leaf_counts)

  plt.show()



def pretty_skeleton(geo):
  # geo is a geometry object
  nodes, edges = get_edges(geo)
  G = nx.Graph()
  G.add_edges_from(edges)
  nx.draw_networkx_edges(G, nodes)
  plt.show()
  



######################### Helper Functions ############################3
from scipy.cluster.hierarchy import dendrogram
import matplotlib.pyplot as plt
import networkx as nx


def augmented_dendrogram(*args, **kwargs):

    ddata = dendrogram(*args, **kwargs)

    if not kwargs.get('no_plot', False):
        for i, d in zip(ddata['icoord'], ddata['dcoord']):
            x = 0.5 * sum(i[1:3])
            y = d[1]
            plt.plot(x, y, 'ro')
            plt.annotate("%.3g" % y, (x, y), xytext=(0, -8),
                         textcoords='offset points',
                         va='top', ha='center')

    return ddata


def get_edges(geo):
  # geo is a hoc geometry object from neuron_readExportedGeometry.py
  nodes, edges = {}, []
  
  for s in range(len(geo.segments)):
    nodes[s] = [geo.segments[s].coordAt(0)[0], geo.segments[s].coordAt(0)[1]]
    for n in geo.segments[s].neighbors:
      edges.append([s, geo.segments.index(n)])
  
  
  return nodes, edges


























