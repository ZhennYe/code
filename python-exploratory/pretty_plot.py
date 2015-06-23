# simply plotting suite

import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import numpy as np
import networkx as nx


########################################################################
# helper functions

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


def condition_by_name(labels, arr, arr2=None, arr3=None):
  # sort by common labels in order so same types show up next to one another
  unique_labels = np.unique([i for i in labels])
  order = []
  for i in unique_labels:
    for x in range(len(labels)):
      if labels[x] == i:
        order.append(x)
  new_labels = [labels[j] for j in order]
  new_arr = [arr[j] for j in order]
  if arr2:
    new_arr2 = [arr2[j] for j in order]
  if arr3:
    new_arr3 = [arr3[j] for j in order]
    return new_labels, new_arr, new_arr2, new_arr3
  if arr2:
    return new_labels, new_arr, new_arr2
  return new_labels, new_arr



def group_by_name(labels, arr, metric='mean'):
  # Group data by name for bar/summary plotting, metric=('mean','median')
  llist = list(np.unique(labels))
  vals = {}
  for l in range(len(labels)):
    if labels[l] not in vals.keys():
      vals[labels[l]] = []
    if metric == 'mean':
      vals[labels[l]].append(np.mean(arr[l]))
    elif metric == 'median':
      vals[labels[l]].append(np.median(arr[l]))
  lab_data, arr_data = [k for k in vals.keys()], [np.mean(d) for d in vals.values()]
  return lab_data, arr_data, [np.std(d) for d in vals.values()]
  



########################################################################
# pretty plots

def pretty_boxplot(V, labelsin, title=None, ticks=None, axes=None):
  """
  V is a matrix of arrays to be plotted; labels must be same length.
  labels = ['PD', 'LG' ...], ticks = ['791_233', ...]
  ticks=False to omit x-ticks altogether; None for legend=ticks
  """
  # mi, ma = lims(V)
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
  elif ticks is None:
    box = ax.boxplot(V, labels=[' ' for i in range(len(labelsin))],
                     showmeans=True, notch=True, patch_artist=True,
                     showfliers=False)
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
  # titles
  if axes:
    ax.set_xlabel(axes[0], fontsize=15)
    ax.set_ylabel(axes[1], fontsize=15)
  ax.set_title(title, fontsize=20)
  
  plt.show()
  return



def pretty_scatter(V, labelsin, title=None, ticks=None, axes=None, showleg=True):
  """
  """
  V = [np.mean(i) for i in V] # Take the mean of each element (okay for lists and scalars)
  L = list(np.unique(labelsin))
  C = [L.index(i) for i in labelsin]
  fcolors = ['darkkhaki', 'royalblue', 'forestgreen','tomato']
  # plotting
  fig = plt.figure()
  ax = fig.add_subplot(111)
  for v in range(len(V)):
    ax.scatter(L.index(labelsin[v]), V[v],  c=fcolors[C[v]], s=80, edgecolor=fcolors[C[v]])
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
    tomato_patch = mpatches.Patch(color='tomato', 
                  label=labelsin[C.index(fcolors.index('tomato'))])
    patches.append(tomato_patch)
  if showleg:
    ax.legend(handles=patches, loc='best')
  if ticks is None:
    ax.tick_params(axis='x',which='both',bottom='off',top='off',
                         labelbottom='off')
  # title
  if title:
    ax.set_title(title, fontsize=20)
  if axes is not None:
    ax.set_xlabel(axes[0], fontsize=15)
    ax.set_ylabel(axes[1], fontsize=15)
  plt.show(); return 
  


#def pretty_errbar(V, labelsin, std




def pretty_bar(v, labelsin, stderr=None, ticks=None, title=None, axes=None):
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
      ax.bar(e,V[e], yerr=Vstd[e], color=fcolors[C[e]], ecolor='k', width=0.5)
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
  plt.legend(handles=patches, loc='best')
  if stderr is not None:
    ax.set_ylim([0, 1.5*(max(V)+np.mean(Vstd))])
  else:
    ax.set_ylim([0, max(V)+0.5*min(V)])
  ax.set_xlim([-.5, len(V)])
  if ticks is None:
    ax.tick_params(axis='x',which='both',bottom='off',top='off',
                         labelbottom='off')
  # title
  if title:
    ax.set_title(title, fontsize=20)
  if axes is not None:
    ax.set_xlabel(axes[0], fontsize=15)
    ax.set_ylabel(axes[1], fontsize=15)
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



# def regress_scatter(xdata, 

def plot_hori_bullshit(xdata, labelsin, title=None, axes=None, norm=False,
                       showmean=True, switch=False):
  # xdata is list of lists (distribution)
  if switch:
    for i in range(len(xdata)-1):
      xdata.append(xdata.pop(0))
      labelsin.append(labelsin.pop(0))
  colors = ['darkkhaki', 'royalblue', 'forestgreen','tomato']
  L = list(np.unique(labelsin))
  C = [L.index(i) for i in labelsin]
  # print(L,C)
  fig = plt.figure()
  plots = [fig.add_subplot(1,len(xdata),i) for i in range(len(xdata))]
  if norm is True:
    tdata = np.linspace(0,100,len(xdata[0]))
  else:
    minm, maxm = np.inf, 0 # condition the data
    for x in xdata:
      if np.mean(x)-np.std(x) < minm:
        minm = np.mean(x)-np.std(x)
      if np.mean(x)+np.std(x) > maxm:
        maxm = np.mean(x)+np.std(x)
    if minm < 0:
      minm = 0.
  for p in range(len(xdata)): # now plot
    b_e = np.linspace(minm, maxm, 100) #int(len(xdata[p])/nbins)) # len/100 bins
    hist, _ = np.histogram(xdata[p], bins=b_e)
    #except:
    #  b_e = np.linspace(minm, maxm, int(len(xdata[p])/10)) # len/100 bins
    #  hist, _ = np.histogram(xdata[p], bins=b_e)
    plotbins = [(b_e[i]+b_e[i+1])/2. for i in range(len(b_e)-1)]
    # divine the appropriate bar width
    hgt = (maxm-minm)/len(plotbins)
    if norm is True:
      print('plotting normalized bars')
      plots[p].barh(tdata, xdata[p], height=1, color=colors[C[p]],
                    edgecolor=colors[C[p]])
    else:
      #print(b_e[:10], hist[:10]) # height is bar width !
      plots[p].barh(plotbins, hist, height=hgt, color=colors[C[p]],
                    edgecolor=colors[C[p]])
      # show the means:
      if showmean:
        def r_bin(bins, target): # always start from below
          j = [i for i in bins]
          #j.sort();
          for i in j:
            if i > target:
              return i
        plots[p].plot([0,max(hist)], [r_bin(plotbins, np.mean(xdata[p])),
                      r_bin(plotbins,np.mean(xdata[p]))], '-', linewidth=3, c='k')
        plots[p].plot([0,max(hist)], [r_bin(plotbins, np.median(xdata[p])),
                      r_bin(plotbins, np.median(xdata[p]))],'--', linewidth=3, c='k', )
        q25, q75 = np.percentile(xdata[p], [25, 75])
        b25, b75 = r_bin(plotbins, q25), r_bin(plotbins, q75)
        plots[p].axhspan(b25, b75, facecolor=colors[C[p]], alpha=0.2)
    if p == 1: #if first plot, show the axes
      plots[p].tick_params(axis='x',which='both',bottom='off',top='off',
                           labelbottom='off')
      if axes:
        plots[p].set_ylabel(axes[1], fontsize=15)
      plots[p].set_ylim([minm, maxm])
    else:
      #plots[p].axis('off')
      plots[p].tick_params(axis='x',which='both',bottom='off',top='off',
                           labelbottom='off')
      plots[p].get_yaxis().set_visible(False)
      plots[p].set_ylim([minm,maxm])
    #plots[p].set_title(titles[p])
  if title:
    plt.suptitle(title, fontsize=20)
  plt.show()
  return



def hori_bars_legend():
  plt.bar(range(100),np.random.random(100),facecolor='darkgray', edgecolor='darkgray')
  plt.axvspan(25, 75, 0,1, color='gray', alpha=0.3)
  plt.plot([50,50],[0,1], '-', c='k', linewidth=3)
  plt.plot([55,55],[0,1], '--', c='k', linewidth=3)
  plt.tick_params(axis='x', which='both',bottom='off', top='off', labelbottom='off')
  plt.tick_params(axis='y', which='both',left='off', right='off', labelleft='off')
  plt.show()

"""
def plot_hori_bars(xdata, labelsin, title=None, axes=None, norm=False):
  # xdata is list of lists (distribution)
  colors = ['darkkhaki', 'royalblue', 'forestgreen','tomato']
  L = list(np.unique(labelsin))
  C = [L.index(i) for i in labelsin]
  # print(L,C)
  fig = plt.figure()
  plots = [fig.add_subplot(1,len(xdata),i) for i in range(len(xdata))]
  if norm:
    tdata = np.linspace(0,100,len(xdata[0]))
  for p in range(len(xdata)):
    hist, b_e = np.histogram(xdata[p], bins=20)
    plotbins = [(b_e[i]+b_e[i+1])/2. for i in range(len(b_e)-1)]
    # area = [10*i for i in hist[0]]
    if norm:
      plots[p].barh(tdata, xdata[p], height=1, color=colors[C[p]],
                    edgecolor=colors[C[p]])
    else:
      plots[p].barh(plotbins, xdata[p], height=1, color=colors[C[p]],
                    edgecolor=colors[C[p]])
    if p == 1: #if first plot
      plots[p].tick_params(axis='x',which='both',bottom='off',top='off',
                           labelbottom='off')
      plots[p].set_ylabel(axes[1], fontsize=15)
      plots[p].set_ylim([0,100])
    else:
      #plots[p].axis('off')
      plots[p].tick_params(axis='x',which='both',bottom='off',top='off',
                           labelbottom='off')
      plots[p].get_yaxis().set_visible(False)
      plots[p].set_ylim([0,100])
    #plots[p].set_title(titles[p])
  if title:
    plt.suptitle(title, fontsize=20)
  plt.show()
  return
"""



def line_scatter(xdata, ydata, labelsin=None, title=None, 
                 lines=True, groups=None, ax_titles=None):
  # a scatter plot with lines through the data
  # groups are a list of listed indices for which ydata belong together
  # i.e.: groups=[[0,1],[2,3]]
  colorlist = ['forestgreen','limegreen','royalblue','lightskyblue',
                'deeppink','orchid']
  markerlist = ['v','o','*','s']
  fig = plt.figure()
  ax = fig.add_subplot(111) # change this to 121 for legend outside fig
  
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
      edges.append([s, geo.segments.index(n)])np.percentile(xdata[p], [25, 75])]
        plots[p].axvspan(0,max(hist), b25, b75, facecolor=colors[C[p]], alpha=0.2)
    if p == 1: #if first plot, show the axes
      plots[p].tick_params(axis='x',which='both',bottom='off',top='off',
                           labelbottom='off')
      if axes:
        plots[p].set_ylabel(axes[1], fontsize=15)
      plots[p].set_ylim([minm, maxm])
    else:
      #plots[p].axis('off')
      plots[p].tick_params(axis='x',which='both',bottom='off',top='off',
                           labelbottom='off')
      plots[p].get_yaxis().set_visible(False)
      plots[p].set_ylim([minm,maxm])
    #plots[p].set_title(titles[p])
  if title:
    plt.suptitle(title, fontsize=20)
  plt.show()
  return



"""
def plot_hori_bars(xdata, labelsin, title=None, axes=None, norm=False):
  # xdata is list of lists (distribution)
  colors = ['darkkhaki', 'royalblue', 'forestgreen','tomato']
  L = list(np.unique(labelsin))
  C = [L.index(i) for i in labelsin]
  # print(L,C)
  fig = plt.figure()
  plots = [fig.add_subplot(1,len(xdata),i) for i in range(len(xdata))]
  if norm:
    tdata = np.linspace(0,100,len(xdata[0]))
  for p in range(len(xdata)):
    hist, b_e = np.histogram(xdata[p], bins=20)
    plotbins = [(b_e[i]+b_e[i+1])/2. for i in range(len(b_e)-1)]
    # area = [10*i for i in hist[0]]
    if norm:
      plots[p].barh(tdata, xdata[p], height=1, color=colors[C[p]],
                    edgecolor=colors[C[p]])
    else:
      plots[p].barh(plotbins, xdata[p], height=1, color=colors[C[p]],
                    edgecolor=colors[C[p]])
    if p == 1: #if first plot
      plots[p].tick_params(axis='x',which='both',bottom='off',top='off',
                           labelbottom='off')
      plots[p].set_ylabel(axes[1], fontsize=15)
      plots[p].set_ylim([0,100])
    else:
      #plots[p].axis('off')
      plots[p].tick_params(axis='x',which='both',bottom='off',top='off',
                           labelbottom='off')
      plots[p].get_yaxis().set_visible(False)
      plots[p].set_ylim([0,100])
    #plots[p].set_title(titles[p])
  if title:
    plt.suptitle(title, fontsize=20)
  plt.show()
  return
"""



def line_scatter(xdata, ydata, labelsin=None, title=None, 
                 lines=True, groups=None, ax_titles=None):
  # a scatter plot with lines through the data
  # groups are a list of listed indices for which ydata belong together
  # i.e.: groups=[[0,1],[2,3]]
  colorlist = ['forestgreen','limegreen','royalblue','lightskyblue',
                'deeppink','orchid']
  markerlist = ['v','o','*','s']
  fig = plt.figure()
  ax = fig.add_subplot(111) # change this to 121 for legend outside fig
  
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



























  
  
  return nodes, edges


























