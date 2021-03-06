#!/usr/bin/python


from robust_map import robust_map
from neuron_readExportedGeometry import demoRead
import os
import sys
if sys.version_info[0] == 3:
  import subprocess
else:
  import commands as subprocess
import json
import matplotlib.pyplot as pyplot
from scipy import sign, logspace, linspace, median
from math import floor, ceil, sqrt, log10


###############################################################################
def system(commandStr):
  """
  Execute command string, returning output on success, or throwing error
  if the return value was not null
  """
  retVal, output = subprocess.getstatusoutput(commandStr)
  if retVal != 0:
    raise IOError('Error executing:' + commandStr + '\n' + output)
  return output


###############################################################################
def getProperties(geoFile, passivePropsFile, display=True):
  from neuron_readExportedGeometry import demoRead
  properties, units = demoRead(geoFile, passivePropsFile, display=display)
  return (properties, units)


###############################################################################
def computeCellProperties(cellTypesFile, passivePropsFile, populationPropsFile,
                          numProcesses=0):
  geoFiles = {}
  baseDir = os.path.dirname(cellTypesFile)
  with open(cellTypesFile, 'r') as fIn:
    geoFiles = [] ; cellTypes = []
    for line in fIn:
      geoFile, cellType = line.split()
      geoFile = os.path.join(baseDir, geoFile)
      geoFiles.append(geoFile) ; cellTypes.append(cellType)
  
  
  results = robust_map(getProperties, geoFiles, args=(passivePropsFile,),
                          numProcesses=numProcesses)
  properties, units = zip(*results)
  units = units[0]
  
  analysis = {
    'geoFiles' : geoFiles,
    'cellTypes' : cellTypes,
    'properties' : properties,
    'units' : units
  }
    
  with open(populationPropsFile, 'w') as fOut:
    json.dump(analysis, fOut, indent=1)
  
  return analysis


###############################################################################
def _saveCurrentFig(savePlotsDir, figName, fType='.pdf'):
  # save the current figure
  savePath = os.path.join(savePlotsDir, figName.replace(os.sep, '-') + fType)
  pyplot.savefig(savePath, bbox_inches='tight')


###############################################################################
def plotProp(analysis, prop, unit, plotLegend=False, savePlotsDir=""):
  markers = ['d', 'o', '^', 'p']
  colors = ['r', 'b', 'm', 'c']

  if savePlotsDir:
    fig = pyplot.gcf()
    fig.clf()
  else:
    fig = pyplot.figure()

  axes = fig.add_subplot(111)
  xList = list(set(analysis['cellTypes']))
  yLists = [ [props[prop] for cellType, props in
              zip(analysis['cellTypes'], analysis['properties'])
              if cellType == xCellType ]
            for xCellType in xList]
  
  for listInd, (cellType, yList) in enumerate(zip(xList, yLists)):
    marker = colors[listInd] + markers[listInd]
    axes.plot([listInd]*len(yList), yList, marker, color=colors[listInd],
                markeredgecolor='none', label=cellType)
  pyplot.xlim([-0.5, len(xList) - 0.5])
  if unit:
    pyplot.ylabel(prop + ' (' + unit + ')')
  else:
    pyplot.ylabel(prop)

  pyplot.title(prop + ' vs Cell Type')
  pyplot.xticks(range(len(xList)), xList)
  
  if plotLegend:
    axes.legend(loc='best')
  pyplot.tight_layout()
  if savePlotsDir:
    # save the figure
    _saveCurrentFig(savePlotsDir, prop + ' vs Cell Type')
  

###############################################################################
def plotListProp(analysis, prop, unit, plotSingles=True, savePlotsDir=""):
  # display the distribution of values for a given property, broken down by
  # individual cell identity

  # get the range of values
  yMin = float('inf') ; yMax = -yMin
  for props in analysis['properties']:
    yList = props[prop]
    yMin = min(yMin, min(yList))
    yMax = max(yMax, max(yList))
  
  # make box plots
  if savePlotsDir:
    fig = pyplot.gcf()
    fig.clf()
  else:
    fig = pyplot.figure()
  axes = fig.add_subplot(111)
  colors = ['r', 'b']
  markers = ['.', '.'];
  typeList = list(set(analysis['cellTypes']))
  signSet = set()
  
  for n, (cellType, props) in enumerate(zip(analysis['cellTypes'],
                                            analysis['properties'])):
    typeInd = typeList.index(cellType)
    marker = colors[typeInd] + markers[typeInd]
    yList = props[prop]
    signSet.update(sign(yList))
    bp = axes.boxplot(yList, sym=marker, positions=[n])
    pyplot.setp(bp['boxes'], color='black')
    pyplot.setp(bp['whiskers'], color='black')
  # decide whether or not to use a log scale for values
  if len(signSet) == 1 and 'angle' not in prop.lower():
    useLog=True
    axes.set_yscale('log')
    yMin *= 0.95 ; yMax *= 1.05
  else:
    useLog=False
    yBuff = 0.05 * (yMax - yMin)
    yMin -= yBuff ; yMax += yBuff
  numCells = n + 1
  pyplot.xlim([-0.5, 0.5 + numCells])
  pyplot.ylim([yMin, yMax])
  if unit:
    pyplot.ylabel(prop + ' (' + unit + ')')
  else:
    pyplot.ylabel(prop)
  pyplot.title(prop)
  cellNames = ['_'.join(os.path.basename(geoFile).split('_')[0:2])
               for geoFile in analysis['geoFiles']]
  
  pyplot.xticks(range(numCells), cellNames, rotation=45)
  pyplot.tight_layout()
  if savePlotsDir:
    # save the figure
    _saveCurrentFig(savePlotsDir, prop + 'BoxPlot')
  
  numHistRows = floor(sqrt(numCells))
  numHistCols = int(ceil(numCells / numHistRows))
  numHistRows = int(numHistRows)
  if savePlotsDir:
    fig = pyplot.gcf()
    fig.clf()
  else:
    fig = pyplot.figure()
  
  # plot histograms
  for n, (cellType, props) in enumerate(zip(analysis['cellTypes'],
                                            analysis['properties'])):
    color = colors[typeList.index(cellType)]
    yList = props[prop]
    axes = fig.add_subplot(numHistRows, numHistCols, n + 1)
    numBins = int(sqrt(len(yList)))
    if useLog:
      bins = logspace(log10(min(yList)), log10(max(yList)), numBins)
    else:
      bins = linspace(min(yList), max(yList), numBins)
    axes.hist(yList, color=color, bins=bins, normed=True,
              edgecolor='none')
    if useLog:
      axes.set_xscale('log')
    pyplot.xlim([yMin, yMax])
    pyplot.title(cellNames[n])
    if unit:
      pyplot.xlabel(prop + ' (' + unit + ')')
    else:
      pyplot.xlabel(prop)

    
  pyplot.suptitle('Histogram of %s' % prop)
  pyplot.tight_layout()
  pyplot.subplots_adjust(top=0.9)
  if savePlotsDir:
    # save the figure
    _saveCurrentFig(savePlotsDir, prop + ' Histogram')
  
  if plotSingles:
    # make plot of median values vs cell type
    medianProp = 'Median ' + prop
    medianAnalysis = {
      'cellTypes' : analysis['cellTypes'],
      'properties' : [ { medianProp : median(props[prop]) }
                       for props in analysis['properties'] ]
    }
    plotProp(medianAnalysis, medianProp, unit, savePlotsDir=savePlotsDir)
  
  
###############################################################################
def displayAnalysis(analysis, plotSingles=False, plotLists=True,
                    savePlotsDir=""):
  units = analysis['units']
  for prop in analysis['properties'][0]:
    if hasattr(analysis['properties'][0][prop], '__len__'):
      # this property is a list, make a series of scatters/histograms for each
      # cell
      if plotLists:
        plotListProp(analysis, prop, units[prop], plotSingles=plotSingles,
                     savePlotsDir=savePlotsDir)
      else:
        print('Skipping plot of %s' % prop)
    else:
      # this property is a number, make a scatter of cells broken down by
      # cell type
      if plotSingles:
        plotProp(analysis, prop, units[prop], savePlotsDir=savePlotsDir)
      else:
        print('Skipping plot of %s' % prop)
  
  if not savePlotsDir:
    pyplot.show()


###############################################################################
def _parseArguments():
  import argparse
  parser = argparse.ArgumentParser(description=
    "Compute and display neuron properties from a list of geometries specified"
    + " by cellTypesFile, assuming passive properties in passivePropsFile.",
    formatter_class=argparse.ArgumentDefaultsHelpFormatter)
  
  # this idea from https://gist.github.com/brantfaircloth/1252339
  class FullPaths(argparse.Action):
    """Expand user- and relative-paths"""
    def __call__(self, parser, namespace, values, option_string=None):
        setattr(namespace, self.dest, os.path.abspath(os.path.expanduser(values)))
 
  
  parser.add_argument("cellTypesFile", nargs="?", type=str,
                  default="~/data/morphology/analyze/cellTypes.txt",
                  help="file specifying cell types and which cells to analyze",
                  action=FullPaths)
  parser.add_argument("passivePropsFile", nargs="?", type=str,
                      default="passive_properties.txt",
                      help="file specifing passive properties",
                      action=FullPaths)
  parser.add_argument("populationPropsFile", nargs="?", type=str,
                      default="populationProperties.txt",
                      help="output file storing results",
                      action=FullPaths)
  parser.add_argument("-sS", "--suppressSingles", action='store_false',
                    dest="plotSingles",
                    help="suppress plots of properties that are single values")
  parser.add_argument("-sL", "--suppressLists", action='store_false',
                      dest="plotLists",
                      help="suppress plots of properties that are lists")
  parser.add_argument("-s", "--savePlotsDir", nargs="?", type=str, default="",
                help="save plots to .pdf files in this dir instead of drawing",
                action=FullPaths)
  return parser.parse_args()
  

###############################################################################
if __name__ == "__main__":
  options = _parseArguments()
  if os.access(options.populationPropsFile, os.R_OK):
    with open(options.populationPropsFile, 'r') as fIn:
      analysis = json.load(fIn)
  else:
    analysis = computeCellProperties(options.cellTypesFile,
                                     options.passivePropsFile,
                                     options.populationPropsFile)
  displayAnalysis(analysis, plotSingles=options.plotSingles,
                  plotLists=options.plotLists,
                  savePlotsDir=options.savePlotsDir)
