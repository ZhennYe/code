function DisplayCorrelationResults(resultS, varargin)
% DisplayCorrelationResults(resultS, options)
% Display the results of running FindPropertyCorrelations
% INPUTS
%    resultS  structure produced by FindPropertyCorrelations
%  (OPTIONAL)
%   options  default options can be overloaded by passing ordered key/value
%            pairs or a single structure with key/value elements.
%     plotIndividual     true
%        Make plots of properties vs predicted properties for each fit
%     displaySummary     true
%        Display some summary results (histogram of p-values, etc.)
%     labelSize          18
%        font size of the plots' x and y labels (in points)
%     titleSize          18
%        font size of the plot title (in points)
%     typeLabels         {}
%        numPoints x 1 cell array of strings labeling type for each point
%     colormap           []
%        numTypes x 3 matrix of colors for individual plots. If colormap is
%        empty and numTypes > 1, a colormap will be created automatically
%        using DistinctColormap.m


% get the options that will control the display function
defaultOptions = { ...
  'plotIndividual', true, ...
  'displaySummary', true, ...
  'plotImportance', false, ...
  'labelSize', 18, ...
  'titleSize', 18, ...
  'typeLabels', {}, ...
  'colormap', [] ...
  };
options = getDisplayOptions(defaultOptions, varargin{:});

correlations = resultS.correlations;
numSections = length(correlations.sections);
for n = 1:numSections
  section = correlations.sections(n);
  fprintf('%s %s %s\n', '########', section.label, '########')
  for ind = section.inds
    %Report significance
    if resultS.isSignificant(ind)
      fprintf('%s ', '[Significant]     ')
    else
      fprintf('%s ', '[Not significant] ')
    end
    
    %Report fit names and R^2/p-values
    fprintf('%s\n', correlations.descriptions{ind})
    fprintf('               p=%g', resultS.pAdjusted(ind))
    if isfield(correlations, 'rSquared')
      fprintf(' R^2=%g', correlations.rSquared(ind))
    end
    if isfield(correlations, 'pValues')
      fprintf(' pRaw=%g', correlations.pValues(ind))
    end
    if isfield(correlations, 'fitErr')
      fprintf(' fitErr=%g', correlations.fitErr(ind))
    end
    fprintf('\n')

    %Make individual plots
    if options.plotIndividual
      plotIndividualResult(resultS, ind, options);
    end
  end
  
  %Make overall section plots
  if options.plotImportance
    keyVals = -log10([0.001; 0.01; 0.05]);
    keyNames = {'p<0.001', 'p<0.01', 'p<0.05'};
    numY = length(section.fitLabels);
    numX = length(section.paramLabels);
    displayMat = zeros(numY, numX);
    for m = 1:numY
      ind = section.inds(m);
      displayMat(m, correlations.indices{ind}) = ...
        -log10(resultS.pAdjusted(ind));
    end
    titleStr = section.label;
    h = NamedFigure(titleStr);
    set(h, 'WindowStyle', 'docked')
    clf
    FigureImportance(displayMat, section.fitLabels, ...
		     section.paramLabels, 0.15, [], keyVals, ...
		     keyNames);
    %title(titleStr, 'FontName', 'Arial', 'FontSize', titleSize);
  end
end

if options.displaySummary
  % display some summary statistics and graphs
  displaySummary(resultS);
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function options = getDisplayOptions(defaultOptions, varargin)
% get the options that will control the display function

%  check for user overrides
options = GetOptions(defaultOptions, varargin, true);

if options.plotIndividual
  % add a few options to pass information to plotting program
  options.types = unique(options.typeLabels);
  options.numTypes = length(options.types);

  if options.numTypes > 1
    options.typeIndices = cell(options.numTypes, 1);
    for n = 1:options.numTypes
      options.typeIndices{n} = strcmp(options.typeLabels, options.types{n});
    end
  end
  if isempty(options.colormap) || ...
     size(options.colormap, 1) ~= options.numTypes
    options.colormap = DistinctColormap(options.numTypes);
  end
end

return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function plotIndividualResult(resultS, ind, options)
%  plot true property values vs values predicted by the fit that was found

% get the data
x = resultS.correlations.propVals{ind};
y = resultS.correlations.predictedPropVals{ind};
% compute the full range of the plot (we want the plot to be square)
yRange = [min(min(x), min(y)), max(max(x), max(y))];
% compute title and axis labels
titleStr = resultS.correlations.descriptions{ind};
yLabelInd = strfind(titleStr, ' fit');
yLabel = titleStr(1:(yLabelInd-1));

% make the figure object
h = NamedFigure(titleStr);
set(h, 'WindowStyle', 'docked')
hold off
lineHandle = plot(yRange, yRange, 'k-');
hAnnotation = get(lineHandle,'Annotation');
hLegendEntry = get(hAnnotation','LegendInformation');
set(hLegendEntry,'IconDisplayStyle','off')

hold on
if options.numTypes > 1
  for m = 1:options.numTypes
    typeInds = options.typeIndices{m};
    typeColor = options.colormap(m,:);
    
    plot(x(typeInds), y(typeInds), '.', ...
         'MarkerEdgeColor', typeColor, 'MarkerSize', 20)
  end
  legend(options.types, 'Location', 'Best', 'FontName', 'Arial')
else
  plot(x, y, '.', 'MarkerEdgeColor', 'k', 'MarkerSize', 20);
end
hold off
xlabel(['Scored ', yLabel], 'FontName', 'Arial', ...
  'FontSize', options.labelSize);
ylabel(['Fit to Scored ', yLabel], 'FontName', 'Arial', ...
  'FontSize', options.labelSize);
title(titleStr, 'FontName', 'Arial', 'FontSize', options.titleSize);
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function displaySummary(resultS)
% display some summary statistics and graphs

labels = getAllLabels(resultS);
numLabels = length(labels);
numSig = zeros(numLabels, 1);

for n = 1:length(resultS.correlations.descriptions)
  if resultS.isSignificant(n)
    desc = resultS.correlations.descriptions{n};
    for m=1:numLabels
      if StringCheck(desc, labels{m})
        numSig(m) = numSig(m) + 1;
      end
    end
  end
end

[numSig, sortInd] = sort(numSig, 'descend');
labels = labels(sortInd);

disp('Number of significant fits involving each property:')
for n = 1:length(labels)
  fprintf('  %s:  %g\n', labels{n}, numSig(n))
end

pVal = resultS.pAdjusted;
n = histc(pVal, [0, 0.001, 0.01, 0.05]);
n(4) = length(pVal) - sum(n(1:3));
n = n / length(pVal);

titleStr = ...
  ['Distribution of adjusted p-values for ', resultS.correlations.label];
h = NamedFigure(titleStr);
set(h, 'WindowStyle', 'docked')
bar(n)
set(gca, 'XTickLabel', {'< .001', ' < .01', ' < .05', '>= .05'})
ylabel('Fraction of fits', 'FontName', 'Arial', 'FontSize', 18)
xlabel('p-values', 'FontName', 'Arial', 'FontSize', 18)
title(titleStr, 'FontName', 'Arial', 'FontSize', 18)
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function labels = getAllLabels(resultS)
labels = {};
for n = 1:length(resultS.correlations.descriptions)
  desc = resultS.correlations.descriptions{n};
  n1 = strfind(desc, ': ');
  if isempty(n1)
    n1 = 1;
  else
    n1 = n1(1) + 2;
  end
  n2 = strfind(desc, 'fit with');
  n2 = n2(1) - 1;
  label = desc(n1:n2);
  
  labels = [labels, {label}]; %#ok<AGROW>
end

labels = unique(labels);
return
