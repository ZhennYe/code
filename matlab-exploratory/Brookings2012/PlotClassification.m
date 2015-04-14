function varargout = PlotClassification(data, groupNums, varargin)
% figureHandle = PlotClassification(data, groupNums, options)
% Visualize the output of a classification algorithm. If the data has > 2
% dimensions, it will be projected down to two dimensions by finding the
% dimensions that provide maximal separation between gruops (calling
% ReduceClusterDimensions.m)
%  INPUTS:
%    data:       numDataPoints x numDataDim matrix of points
%    groupNums:  numDataPoints x 1 list of found group indices
%   OPTIONS
%    trueGroupNums          []
%       numDataPoints x 1 list of true group indices
%    title                  'Classification'
%       Title of plot
%    groupNames             {'Group 1', 'Group 2', ...}
%       Cell array of names for each group
%    dimensionNames         {'x_1', 'x_2', ...}
%       Cell array of names for each dimension of data
%    scales                 []
%       Length scale associated with each dimension. Only used to eliminate
%       dimensions with very small length scales from consideration
%    colormap               []
%       numGroups x 3 matrix of RGB values used to plot points, with the
%       first row corresponding to group 1, the 2nd to group 2, etc. If
%       empty, a colormap will be created automatically.
%  OUTPUTS:
%    figureHandle:  a handle to the classification figure will be returned
%                   if requested


% Preliminary sanity check on data
if isempty(data)
  error('Empty data set')
end
numDim = size(data, 2);

% get options for the visualization
default_options = {...
  'trueGroupNums', [], ...
  'title', 'Classification', ...
  'groupNames', getDefaultGroupNames(groupNums), ...
  'dimensionNames', getDefaultDimNames(numDim), ...
  'scales', [], ...
  'colormap', []};
options = GetOptions(default_options, varargin, true);
if isempty(options.trueGroupNums)
  % if the true group numbers are not specified, just plot as though they
  % were all correct
  options.trueGroupNumes = groupNums;
end


% Pick the plotting routine based on the dimensionality of the data
switch numDim
  case 1, h = plotClass1D(data, groupNums, options);
  case 2, h = plotClass2D(data, groupNums, options);
  otherwise, h = plotClassND(data, groupNums, options);
end


% return handle to figure if requested
if nargout == 1
  varargout = {h};
else
  varargout = {};
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function defaultGroupNames = getDefaultGroupNames(groupNums)
% Make up some names for the different groups if none are specified

groups = unique(groupNums);
if ~isfloat(groups)
  defaultGroupNames = groups;
  return
end
numGroups = length(groups);
defaultGroupNames = cell(1, numGroups);
for n = 1:numGroups
  defaultGroupNames{n} = sprintf('Group %g', groups(n));
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function defaultDimNames = getDefaultDimNames(numDim)
% Make up some names for the different data dimensions if none are
% specified

defaultDimNames = cell(1, numDim);
for n = 1:numDim
  defaultDimNames{n} = sprintf('x_{%d}', n);
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function h = plotClass1D(data, groupNums, options)
% Add a second dimension of all zeros, then plot the resulting points

data = [data, zeros(size(data))];
h = plotClass2D(data, groupNums, options);
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function h = plotClass2D(data, groupNums, options)
% Plot the data points, coloring by group and correct identification

% get the group IDs and number
groups = unique(groupNums);
numGroups = length(groups);

% choose a coloring scheme
if size(options.colormap, 1) >= numGroups
  colormap = options.colormap;
else
  colormap = DistinctColormap(numGroups);
end
colors = cell(numGroups, 1);
for n = 1:numGroups
  colors{n} = colormap(n,:);
end

% determine which points are correctly classified
correct = (groupNums == options.trueGroupNums);

% set the marker size
numPoints = length(groupNums);
maxSize = 10;
minSize = 2;
if numPoints <= 100
  markerSize = maxSize;
elseif numPoints >= 1000
  markerSize = minSize;
else
  deltaSize = maxSize - minSize;
  markerSize = round(maxSize - deltaSize * log10(numPoints/100));
end
incorrectMarkerSize = 2 * markerSize;

% create a figure
h = NamedFigure(options.title);
clf
hold on

% plot the data by group number and correctness, adding a legend label for
% each type
legendLabels = {};
for n = 1:numGroups
  groupColor = colors{n};
  ind = find(options.trueGroupNums == groups(n) & correct);
  if ~isempty(ind)
    % plot correctly identified points from group n as filled circles
    symbol = 'o';
    plot(data(ind,1), data(ind,2), symbol, ...
      'MarkerEdgeColor', groupColor, ...
      'MarkerFaceColor', groupColor, ...
      'MarkerSize', markerSize);
    legendLabels = [legendLabels, {['correct ', options.groupNames{n}]}]; %#ok<AGROW>
  end
  ind = find(options.trueGroupNums == groups(n) & ~correct);
  if ~isempty(ind)
    % plot incorrectly identified points from group n as Xs
    symbol = 'x';
    plot(data(ind,1), data(ind,2), symbol, ...
      'MarkerEdgeColor', groupColor, ...
      'MarkerFaceColor', groupColor, ...
      'MarkerSize', incorrectMarkerSize);
    legendLabels = [legendLabels, {['incorrect ', options.groupNames{n}]}]; %#ok<AGROW>
  end
end

% add legend and axis labels
legend(legendLabels{:}, 'Location', 'Best');
xlabel(options.dimensionNames{1}, 'FontName', 'Arial', 'FontSize', 16)  
ylabel(options.dimensionNames{2}, 'FontName', 'Arial', 'FontSize', 16)  
hold off
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function h = plotClassND(data, groupNums, options)
% find the best 2D projection of the data, project the data down
%  to it, and then plot the classification on the projection

% calculate a few things in preparation for finding the projections
% specifically, we need to construct
% dataDeviations: numPoints x numDim matrix of positions with the
%                 group mean subtracted from them
% dataMeansDiffs: numGroupPairs x numDim matrix of differences
%                 between group means.
%                 e.g. if there are three groups:
%                 row one is the vector difference between mean
%                         group 1 position and mean group 2 position,
%                 row two is between groups 1 and 3
%                 row three is between groups 2 and 3

% use the true group numbers to use to reduce dimensions:
projGroupNums = options.trueGroupNums;

% if dimension scale information exists, use it to eliminate some
% dimensions in advance:
if isfield(options, 'scales') && ~isempty(options.scales)
  useDims = options.scales > 0.1 * max(options.scales);
  if sum(useDims) == 1
    useDims(find(~useDims, 1)) = true;
  end
  data = data(:,useDims);
  options.dimensionNames = options.dimensionNames(useDims);
end

% use cluster membership to reduce the dimensionality to 2 for plotting
[data, projection] = ReduceClusterDimensions(data, projGroupNums, 2);

% label the projection dimensions with their formuli
pName1 = 'Proj 1';
pName2 = 'Proj 2';
pName1 = addFormula(pName1, projection(:,1), options);
pName2 = addFormula(pName2, projection(:,2), options);

options.dimensionNames = {pName1, pName2};

% plot the classification
h = plotClass2D(data, groupNums, options);
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function pName = addFormula(pName, w, options)
% add a formula describing the projection dimension

% don't include any unimportant dimensions in formula
sigCutoff = 0.05 * max(abs(w));
significant = abs(w) > sigCutoff;

w = w(significant);
dimNames = options.dimensionNames(significant);

pName = [pName, ' = ', sprintf('%0.2f*%s', w(1), dimNames{1})];
for n = 2:length(w)
  if w(n) > 0
    w_n = w(n);
    op = ' + ';
  else
    w_n = -w(n);
    op = ' - ';
  end
  
  pName = [pName, op, sprintf('%0.2f*%s', w_n, dimNames{n})]; %#ok<AGROW>
end
return
