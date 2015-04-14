function varargout = FindPropertyCorrelations(xData, yData, varargin)
% resultStruct = FindPropertyCorrelations(xData, yData, [options])
% Find correlations in data, and evaluate their significance
% INPUTS
%    xData   numData x numXProperties matrix of floating point values
%            each row corresponds to a unique data point
%            each column corresponds to a measured property
%            if a property isn't measured for one data point, that entry
%            should be set to NaN
%  (OPTIONAL)
%    yData   numData x numYProperties matrix. numData should be the same as
%            for xData, but numYPeroperties can differ from numXProperties
%            if yData is specified:
%               correlations are sought by finding linear combinations of
%               xData that reconstruct yData and linear combinations of
%               yData that reconstruct xData
%            if yData is empty (pass yData = [])
%               correlations are sought by finding linear combinations of x
%               properties that reconstruct each individual x property 
%   options  default options can be overloaded by passing ordered key/value
%            pairs or a single structure with key/value elements.
%     numShuffledTrials  10000
%        if non-zero, compute adjusted p-values by finding correlations in
%        shuffled data. If zero, compute adjusted p-values by Pearson test
%        and correcting with Holm-Bonferroni.
%     significanceLevel  0.05
%        The threshold for a p-value to be considered significant
%     typeLabels         {}
%        Either empty or a numPoints x 1 cell array of strings specifying
%        the "type" of each data point. Correlations can be pooled across
%        type, or broken down by type independently
%     fitPooled          true
%        Find correlations by pooling across type
%     fitSubTypes        false  
%        Find correlations by restricting to each sub-type in turn
%     xLabel            'x'
%        The label that collectively applies to xData
%     yLabel            'y'
%        The label that collectively applies to yData
%     xPropLabels       {}
%        List of labels that apply to each column of xData
%     yPropLabels       {}
%        List of labels that apply to each column of yData
%     colormap           []
%        numTypes x 3 matrix of colors for individual plots. If colormap is
%        empty and numTypes > 1, a colormap will be created automatically using
%        DistinctColormap.m
% OUTPUTS
%    resultStruct   A structure that contains information about the
%                   correlations and their significance. It contains the
%                   following subfields
%       numFits           The number of distinct correlations that will be
%                         tested.
%       shuffledFitError  Present if numShuffledTrials == true
%                         numFits x numShuffledTrials array of fit error
%                         for each fit in each shuffled trial
%       adjustedPValues   numFits x 1 array of adjusted p-values
%       isSignificant     numFits x 1 array of boolean listing the
%                         significance of each found correlation
%       correlations      Structure with the bulk of the fit information
%        label              collective description of the correlations
%        fitErr             numFits x 1 array of fit error
%        rSquared           numFits x 1 array of rSquared for best
%                           correlations
%        descriptions       numFits x 1 cell array of correlation
%                           descriptions
%        indices            numFits x 1 cell array with list of columns
%                           used in each fit
%        propVals           numFits x 1 cell array with the data used for
%                           each fit
%        predictedPropVals  numFits x 1 cell array with the data that would
%                           be predicted by the results of the fit
%        coefs              numFits x 1 cell array of fit coefficients
%        pValues            numFits x 1 array of raw p-values
%        sections           list of which fits were grouped into which
%                           computational batches

if nargin < 2
  help FindPropertyCorrelations
  error('Incorrect number of input arguments.')
end
if nargout > 1
  help FindPropertyCorrelations
  error('Incorrect number of return values.')
end
  
% get options that control how FindPropertyCorrelations will fit data
%   -first set some default options
defaultOptions = { ...
  'numShuffledTrials', 10000, ...
  'xLabel', 'x', 'yLabel', 'y', ...
  'xPropLabels', {}, 'yPropLabels', {}, ...
  'typeLabels', {}, ...
  'significanceLevel', 0.05 ...
  'fitPooled', true, ...
  'fitSubTypes', false, ...
  'colormap', [] ...
  };
options = getCorrelationOptions(defaultOptions, xData, yData, varargin{:});

% Minor preliminary processing
%   -Remove all the rows where XData is all NaN or yData is all NaN
%   -ZScore the data
[xData, yData, options] = preprocessData(xData, yData, options);

%   -Make two structs, one to hold results, and one to hold data
%    necessary for fits
[resultStruct, dataStruct] = organizeDataStructs(xData, yData, options);


% Do the analysis with the selected map properties
isShuffledTrial = false;
resultStruct.correlations = ...
  correlateProperties(xData, yData, dataStruct, options, isShuffledTrial);


% adjust p-values for multiple correlations
if options.numShuffledTrials > 0
  % do all the shuffled trials in this block
  isShuffledTrial = true;
  
  
  % open matlab pool if it isn't already open
  parBlock = ParallelBlock();

  %Make a popup to display progress
  if isempty(yData)
    popupLabel = ...
      sprintf('%s vs %s shuffled trials', options.xLabel, options.xLabel);
  else
    popupLabel = ...
      sprintf('%s vs %s shuffled trials', options.xLabel, options.yLabel);
  end
  PopupProgress(popupLabel, options.numShuffledTrials)
  
  shuffledFitErr = resultStruct.shuffledFitErr;
  parfor n = 1:options.numShuffledTrials
    % get the fit error for each shuffled trial, distributing them in
    % parallel
    shuffledFitErr(:,n) = ...
      correlateProperties(xData, yData, dataStruct, options, ...
                          isShuffledTrial);
    PopupProgress(popupLabel)
  end
  resultStruct.shuffledFitErr = shuffledFitErr;
  
  % close the matlab pool if it is no longer required
  parBlock.endBlock();
  
  % adjust p-values for multiple correlations, using shuffled trials
  resultStruct = shuffleCorrect(resultStruct, options);
else
  % adjust p-values for multiple correlations using Holm-Bonferroni
  resultStruct = holmBonferroniCorrect(resultStruct, options);
end

% display the results
DisplayCorrelationResults(resultStruct, options);

if nargout == 1
  varargout = {resultStruct};
else
  varargout = {};
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function options = getCorrelationOptions(defaultOptions, xData, yData, ...
                                         varargin)
% return an options structure based on defaultOptions, but with user
% overrides

%  check for user overrides
options = GetOptions(defaultOptions, varargin, true);

% ensure that 'xPropLabels' and 'yPropLabels' are appropriately defined
if isempty(options.xPropLabels)
    options.xPropLabels = makePropertyLabels(options.xLabel, ...
                                             size(xData, 2));
end
if isempty(options.yPropLabels)
    options.yPropLabels = makePropertyLabels(options.yLabel, ...
                                             size(yData, 2));
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function labels = makePropertyLabels(baseLabel, numProperties)
% Make some default property labels so that findPropertyCorrelations can
% report back which correlations were significant, etc
% 
% Labels are of the form baseLabel_#
% e.g. if baseLabels = 'x' and numProperties = 3, then
%  labels = {'x_1', 'x_2', 'x_3'}
if numProperties == 0
  labels = {};
else
  labels = cell(1, numProperties);
  for n = 1:numProperties
    labels{n} = sprintf('%s_%d', baseLabel, n);
  end
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [xData, yData, options] = preprocessData(xData, yData, options)
% Remove all the rows where XData is all NaN or yData is all NaN
% ZScore the data

% Keep rows were some xData is finite and some yData is finite
if size(yData, 2) > 0
  % yData is used:
  %  keep rows were some xData is finite and some yData is finite
  keepRows = find( sum(isfinite(xData), 2) > 0 & ...
                   sum(isfinite(yData), 2) > 0 );
  
  yData = ZScore(yData(keepRows,:), 1, 1);
else
  % yData isn't used
  %  keep rows were some xData is finite
  keepRows = find( sum(isfinite(xData), 2) > 0 );
  % make yData a L x 0 matrix (avoids some annoying if-statements later)
  yData = zeros(length(keepRows), 0);
end

xData = ZScore(xData(keepRows,:), 1, 1);

if isfield(options, 'typeLabels') && ~isempty(options.typeLabels)
  options.typeLabels = options.typeLabels(keepRows);
end
if isfield(options, 'IDs') && ~isempty(options.IDs)
  options.IDs = options.IDs(keepRows);
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [resultS, dataS] = organizeDataStructs(xData, yData, options)
% construct two structs, one to hold results, and one to hold data
% necessary for fits
%   resultS: a structure to hold results of the correlation algorithm
%   dataS: a structrure to hold data used by the correlation algorithm

% check for consistency in property data set sizes, and describe the
% overall fit
numDataPoints = size(xData, 1);
if size(yData, 1) ~= numDataPoints
  error('xData and yData have different numbers of data points')
end

if size(yData, 2) > 0
  % finding correlations between two distinct sets of properties
  label = sprintf('Correlations between %s and %s', ...
                          options.xLabel, options.yLabel);
else
  % finding correlations among one set of properties
  label = sprintf('Correlations among %s', options.xLabel);
end

% determine if we're breaking down 
types = unique(options.typeLabels);
numTypes = length(types);
numTypeFits = options.fitPooled + numTypes * options.fitSubTypes;

% Make lists of which rows and columns to use for each fit, and keep track
% of the total number of fits
useRows = cell(numTypeFits, 1);
useXCols = cell(numTypeFits, 1);
useYCols = cell(numTypeFits, 1);
numFits = 0;
fitNum = 1;
if options.fitPooled
  % only do this if we're fitting all the data, pooled across cell type
  useRows{fitNum} = 1:numDataPoints;
  useXCols{fitNum} = find(sum(isfinite(xData), 1) > 0);
  useYCols{fitNum} = find(sum(isfinite(yData), 1) > 0);
  numFits = numFits + length(useXCols{fitNum}) + length(useYCols{fitNum});
  fitNum = fitNum + 1;
end
if options.fitSubTypes
  % only do this if we're breaking down fits by cell type
  for n = 1:numTypes
    subType = types{n};
    useRows{fitNum} = find(strcmp(options.typeLabels, subType));
    useXCols{fitNum} = ...
      find(sum(isfinite(xData(useRows{fitNum},:)), 1) > 0);
    useYCols{fitNum} = ...
      find(sum(isfinite(yData(useRows{fitNum},:)), 1) > 0);
    numFits = ...
      numFits + length(useXCols{fitNum}) + length(useYCols{fitNum});
    fitNum = fitNum + 1;
  end
end

resultS = struct( ...
  'numFits', numFits ...
  );
if options.numShuffledTrials > 0
  resultS.shuffledFitErr = zeros(numFits, options.numShuffledTrials);
end


% Create a struct holding information necesary for computing correlations
% (due to the madness of the MATLAB language, add cell array fields later)
dataS = struct( ...
    'corrLabel', label, ...
    'numFits', numFits, ...
    'numTypes', numTypes ...
  );
dataS.types = types;
dataS.useRows = useRows;
dataS.useXCols = useXCols;
dataS.useYCols = useYCols;
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function correlations = correlateProperties(xData, yData, dataStruct, ...
                                          options, isShuffledTrial)
% Do the correlations, section by section.
% This routine calls different correlation functions based upon whether:
%   yData is empty
%   isShuffledTrial is true/false
%        if true, correlations is just a numFits x 1 array
%        if false, correlations is a structure containing a lot of
%        supplemental information about the results of the correlation

numFits = dataStruct.numFits;

if isShuffledTrial
  correlations = zeros(numFits, 1);
else
  correlations = struct ( ...
    'label', dataStruct.corrLabel, ...
    'fitErr', zeros(numFits, 1), ...
    'rSquared', zeros(numFits, 1), ...
    'pValues', zeros(numFits, 1), ...
    'sections', [] ...
    );
  correlations.descriptions = cell(numFits, 1);
  correlations.indices = cell(numFits, 1);
  correlations.propVals = cell(numFits, 1);
  correlations.predictedPropVals = cell(numFits, 1);
  correlations.coefs = cell(numFits, 1);
end

if isempty(yData)
  if isShuffledTrial
    % find correlations between each column of xData and (shuffled rows of)
    %   the remaining columns of xData
    mainCorrRoutine = @(label, correlations, fitNum) ...
      corrShuffledX(xData, dataStruct, correlations, fitNum);
  else
    % find correlations between each element of xData and the rest of xData
    mainCorrRoutine = @(label, correlations, fitNum) ...
      corrX(xData, options, dataStruct, ...
      label, correlations, fitNum);
  end
else
  if isShuffledTrial
    % find correlations between indpendently shuffled xData and yData
    mainCorrRoutine = @(label, correlations, fitNum) ...
      corrShuffledXY(xData, yData, dataStruct, correlations, fitNum);
  else
    % find correlations between xData and yData
    mainCorrRoutine = @(label, correlations, fitNum) ...
      corrXY(xData, yData, options, dataStruct, ...
      label, correlations, fitNum);
  end
end

%First do the correlation with all cells grouped together
fitNum = 1;
if options.fitPooled
  label = 'Pooled';
  correlations = mainCorrRoutine(label, correlations, fitNum);
  fitNum = fitNum + 1;
end

%Now separate the cells out by type:
if options.fitSubTypes
  for n = 1:dataStruct.numSubTypes
    label = dataStruct.types{n};
    correlations = mainCorrRoutine(label, correlations, fitNum);
    fitNum = fitNum + 1;
  end
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function correlations = corrShuffledX(xData, dataStruct, ...
                                       correlations, fitNum)
% find correlations between each column of xData and (shuffled rows of)
% the remaining columns of xData

useRows = dataStruct.useRows{fitNum};
xCols = dataStruct.useXCols{fitNum};
numDataPoints = length(useRows);
numXProps = length(xCols);

% shuffle the order of the rows (the data point labels)
xRows = useRows(randperm(numDataPoints));
yRows = useRows(randperm(numDataPoints));
% use the valid data points and properties
x = xData(xRows, xCols);
y = xData(yRows, xCols);

%Fit each (shuffled) column with linear combinations of the other columns
% for shuffled trials, just get the fit error
n1 = 1 + (fitNum - 1) * numXProps;
for propNum = 1:numXProps
  propDiff = setdiff(1:numXProps, propNum);
  n = n1 + propNum - 1;
  correlations(n) = FindCorrelations(x(:,propDiff), y(:,propNum));
end
return




%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function correlations = corrX(xData, options, dataStruct, ...
                              label, correlations, fitNum)

useRows = dataStruct.useRows{fitNum};
xCols = dataStruct.useXCols{fitNum};
numDataPoints = length(useRows);
numXProps = length(xCols);

% use the valid data points and properties
x = xData(useRows, xCols);

xDataLabel = options.xLabel;
xFitLabels = cell(numXProps, 1);
for n = 1:numXProps
  xFitLabels{n} = [label, ': ', options.xPropLabels{xCols(n)}];
end

%Fit yData with linear combinations of xData
n1 = 1 + (fitNum - 1) * numXProps;
n2 = n1 + (numXProps - 1);
for propNum = 1:numXProps
  % get some additional information, because we care about e.g., which
  % properties are fit
  propDiff = setdiff(1:numXProps, propNum);
  n = n1 + propNum - 1;

  [correlations.fitErr(n), correlations.rSquared(n), ...
   correlations.indices(n), correlations.propVals(n), ...
   correlations.predictedPropVals(n), correlations.coefs(n), ...
   correlations.pValues(n)] = ...
                             FindCorrelations(x(:,propDiff), x(:,propNum));
                             
  
  correlations.descriptions(n) = ...
    getDescriptions(xFitLabels(propDiff), xFitLabels(propNum), ...
                    correlations.indices(n));
end
section.label = ...
  sprintf('%s: Finding correlations among %s', label, xDataLabel);
section.inds = n1:n2;
section.fitLabels = xFitLabels;
section.paramLabels = xFitLabels;
correlations.sections = [correlations.sections, section];
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function correlations = corrShuffledXY(xData, yData, dataStruct, ...
                                       correlations, fitNum)

useRows = dataStruct.useRows{fitNum};
xCols = dataStruct.useXCols{fitNum};
yCols = dataStruct.useYCols{fitNum};
numDataPoints = length(useRows);
numXProps = length(xCols);
numYProps = length(yCols);

% shuffle the order of the rows (the data point labels)
xRows = useRows(randperm(numDataPoints));
yRows = useRows(randperm(numDataPoints));
% use the valid data points and properties
x = xData(xRows, xCols);
y = yData(yRows, yCols);

%Fit yData with linear combinations of xData
n1 = 1 + (fitNum - 1) * (numXProps + numYProps);
n2 = n1 + numYProps - 1;

% for shuffled trials, just get the fit error
correlations(n1:n2) = FindCorrelations(x, y);

%Fit xData with linear combinations of yData
n1 = n2 + 1;
n2 = n2 + numXProps;
correlations(n1:n2) = FindCorrelations(y, x);
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function correlations = corrXY(xData, yData, options, dataStruct, ...
                              label, correlations, fitNum)
useRows = dataStruct.useRows{fitNum};
xCols = dataStruct.useXCols{fitNum};
yCols = dataStruct.useYCols{fitNum};
numXProps = length(xCols);
numYProps = length(yCols);

% use the valid data points and properties
x = xData(useRows, xCols);
y = yData(useRows, yCols);

xDataLabel = options.xLabel;
yDataLabel = options.yLabel;
xFitLabels = cell(numXProps, 1);
yFitLabels = cell(numYProps, 1);
for n = 1:numXProps
  xFitLabels{n} = [label, ': ', options.xPropLabels{xCols(n)}];
end
for n = 1:numYProps
  yFitLabels{n} = [label, ': ', options.yPropLabels{yCols(n)}];
end

%Fit yData with linear combinations of xData
n1 = 1 + (fitNum - 1) * (numXProps + numYProps);
n2 = n1 + numYProps - 1;
% get some additional information, because we care about e.g., which
% properties are fit
[correlations.fitErr(n1:n2), correlations.rSquared(n1:n2), ...
  correlations.indices(n1:n2), correlations.propVals(n1:n2), ...
  correlations.predictedPropVals(n1:n2), correlations.coefs(n1:n2), ...
  correlations.pValues(n1:n2)] = FindCorrelations(x, y);

correlations.descriptions(n1:n2) = ...
  getDescriptions(xFitLabels, yFitLabels, correlations.indices(n1:n2));
section.label = ...
  sprintf('%s: Fitting %s with %s', label, yDataLabel, xDataLabel);
section.inds = n1:n2;
section.fitLabels = yFitLabels;
section.paramLabels = xFitLabels;
correlations.sections = [correlations.sections, section];


%Fit xData with linear combinations of yData
n1 = n2 + 1;
n2 = n2 + numXProps;
[correlations.fitErr(n1:n2), correlations.rSquared(n1:n2), ...
  correlations.indices(n1:n2), correlations.propVals(n1:n2), ...
  correlations.predictedPropVals(n1:n2), correlations.coefs(n1:n2), ...
  correlations.pVals(n1:n2)] = FindCorrelations(y, x);
correlations.descriptions(n1:n2) = ...
  getDescriptions(yFitLabels, xFitLabels, correlations.indices(n1:n2));
section.label = ...
  sprintf('%s: fitting %s with %s', label, xDataLabel, yDataLabel);
section.inds = n1:n2;
section.fitLabels = xFitLabels;
section.paramLabels = yFitLabels;
correlations.sections = [correlations.sections, section];
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function descriptions = getDescriptions(paramLabels, fitLabels, indices)
% create a text description of the results of the fit

numFit = length(fitLabels);
descriptions = cell(numFit, 1);
for n = 1:numFit
  % loop over each property that was fit and make a description for it
  descriptions{n} = sprintf('%s fit with', fitLabels{n});
  for ind = indices{n}
    descriptions{n} = sprintf('%s %s', descriptions{n}, ...
			      stripSectionLabel(paramLabels{ind}));
  end
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function stripped = stripSectionLabel(label)
n = strfind(label, ': ');
if isempty(n)
  stripped = label;
else
  n = n(1) + 2;
  stripped = label(n:end);
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function resultS = shuffleCorrect(resultS, options)
% Compute the adjusted p-values and significance of a set of correlations
% by comparing them to the results of correlations of shuffled data.
%   -Start by considering every possible fit in the scrambled data.
%   -Loop through the fits being considered, starting with the best (lowest
%    fitErr)
%     -The adjusted p-value is the fraction of correlations in scrambled
%      data that are better than the correlations in the best fit.
%     -If the adjusted p-value is better than the significance level:
%         -The fit is significant
%         -That fit is removed from consideration
%         -Looping continues with the next best fit
%     -Otherwise, that fit and all remaining fits are not significant

sigLevel = options.significanceLevel;
numShuffle = size(resultS.shuffledFitErr, 2);
numFits = resultS.numFits;

% allocate some arrays
isSignificant = false(numFits, 1);  % true/false for each fit
pAdjusted = ones(numFits, 1); % p-values adjusted for multiple correlations

% Sort the fit error from smallest to largest
[sortFitErr, sortInds] = sort(resultS.correlations.fitErr);

% Start by considering every possible fit in the scrambled data.
considerInds = 1:numFits;

for n = 1:numFits
  %Loop through data, starting with the best fit (lowest fitError).
  
  % Get the best fits to the considered shuffled data, and see how many are
  % better
  bestFits = min(resultS.shuffledFitErr(considerInds,:), [], 1);
  numBetter = sum(bestFits < sortFitErr(n));
  
  % Get a p-value on this fit being better than fits to shuffled data
  propNum = sortInds(n);
  pAdjusted(propNum) = (numBetter + 1) / (numShuffle + 1);
  
  if pAdjusted(propNum) < sigLevel
    % The adjusted p-value is small enough, so the fit is % significant.
    isSignificant(propNum) = true;
    
    %Remove the shuffled trials of *this* fit from consideration,
    %  and go on to the next-best fit.
    considerInds = setdiff(considerInds, propNum);
  else
    % The adjusted p-value is NOT small enough so this fit and all
    %  subsequent fits are NOT significant.
    
    for k = (n+1):numFits
      numBetter = sum(bestFits < sortFitErr(k));
      propNum = sortInds(k);
      pAdjusted(propNum) = (numBetter + 1) / (numShuffle + 1);
    end
    break
  end
end

% copy p-values into resultS structure
resultS.pAdjusted = pAdjusted;
resultS.isSignificant = isSignificant;
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function resultS = holmBonferroniCorrect(resultS, options)
% Compute the adjusted p-values and significance of a set of correlations
% by by applying the Holm-Bonferroni correction
%   -Start by considering every possible fit in the scrambled data.
%   -Loop through the fits being considered, starting with the best (lowest
%    pValues)
%     -n = the number of fits being considered
%     -The adjusted p-value = 1 - (1 - p)^n
%     -If the adjusted p-value is better than the significance level:
%         -The fit is significant
%         -That fit is removed from consideration (i.e. n is decreased)
%         -Looping continues with the next best fit
%     -Otherwise, that fit and all remaining fits are not significant

sigLevel = options.significanceLevel;

[sortP, sortInd] = sort(resultS.correlations.pValues, 'descend');

numFits = resultS.numFits;
isSignificant = false(numFits, 1);
pAdjusted = ones(numFits, 1);

for n = numFits:-1:1
  ind_n = sortInd(n);
  pAdjusted(ind_n) = 1.0 - ( 1.0 - sortP(n) )^n;
  if pAdjusted(ind_n) < sigLevel
    isSignificant(ind_n) = true;
  else
    remaining = sortInd((n-1):-1:1);
    pAdjusted(remaining) = ...
      1.0 - ( 1.0 - resultS.correlations.pValues(remaining) ).^n;
    break
  end
end

% copy p-values into resultS structure
resultS.pAdjusted = pAdjusted;
resultS.isSignificant = isSignificant;
return



