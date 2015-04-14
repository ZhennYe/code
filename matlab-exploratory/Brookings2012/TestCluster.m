function TestCluster(clustFunc, varargin)
% TestCluster(clustFunc, options)
% Test the clustering function specified by the function handle clustFunc
% by generating one or more synthetic data sets, running the algorithm, and
% comparing its results to the correct results.
%  INPUTS:
%   -clustFunc: handle to cluster function, of form
%               clustFunc(dataMat, numClusters, clustNums)
%               where dataMat is numPoints x numDims matrix of points to be
%                                clustered
%               numClusters is the number of clusters to look for
%               clustNums is the list of correct cluster indices
%               If options must be passed to the cluster function or
%               numClusters/clustNums ignored, define a function handle and
%               pass it to TestCluster
%  OPTIONS
%   -makePlots         true
%       visually summarize results
%   -numTestDataSets   100
%       create this many synthetic data sets and try to cluster them
%   -clusterSeparation 8
%       distance between cluster centroids (standard deviations, before
%       z-scoring data)
%   -minNumClusters    3
%       each synthetic data set must have at least this many clusters
%   -maxNumClusters    6
%       each synthetic data set must have at most this many clusters
%   -minNumDims        4
%       synthetic data points have at least this many dimensions
%       between 0 and (numDims - 2) of the dimensions will be extraneous
%       (e.g., random values regardless of cluster identity)
%   -maxNumDims        4
%       synthetic data points have at most this many dimensions
%   -passAccuracy      90
%       percentage of points that must be correctly grouped for a
%       clustering to be deemed successful
%   -useParallel       true
%       use parfor when possible to speed up computation


% clear some data to allow edited files to be reloaded
clear class function

% get options for testing, ignoring any extra options
defaultOptions = { ...
  'makePlots', true, ...
  'numTestDataSets', 100, ...
  'clusterSeparation', 8, ...
  'minNumClusters', 3, ...
  'maxNumClusters', 6, ...
  'minNumDims', 4, ...
  'maxNumDims', 10, ...
  'passAccuracy', 90, ...
  'useParallel', true ...
  };
options = GetOptions(defaultOptions, varargin);

if options.useParallel
  % start the parallel environment if requested
  parBlock = ParallelBlock();
end

% get the name of the cluster function we're testing
funcName = getFuncName(clustFunc);

if options.numTestDataSets > 1
  % Show progress through trials with a progress bar
  
  updatePopup = true;
  popupName = sprintf('Testing %s', funcName);
  PopupProgress(popupName, options.numTestDataSets, ...
                'errorOnPreexisting', false);
else
  updatePopup = false;
end


for trial = options.numTestDataSets:-1:1
  % loop through trials (in reverse order to make only one allocation)
  
  % randomly choose some of the parameters of the synthetic data set
  numClusters = randi([options.minNumClusters, options.maxNumClusters]);
  numDims = randi([options.minNumDims, options.maxNumDims]);
  numDimsExtraneous = randi([0, numDims - 2]);
  numDimsSep = numDims - numDimsExtraneous;
  numPts = max(45, 15 * numClusters);

  % generate the test data
  [testData, clustNums] = ...
    generateTestData(numClusters, numDims, numDimsSep, numPts, ...
                     options.clusterSeparation);

  % run the clustering algorithm being tested
  clustResults = clustFunc(testData, numClusters, clustNums);

  % get information about the accuracy
  accuracy = ClusteringAccuracy(clustResults.clustInds, clustNums, ...
                                'calcPValue', true);
  
  % add the randomly generated data into the accuracy structure and store
  % it in results
  accuracy.numClusters = numClusters;
  accuracy.numDims = numDims;
  accuracy.numDimsSep = numDimsSep;
  accuracy.numPoints = numPts;
  accuracy.testData = testData;
  accuracy.trueClustNums = clustNums;
  results(trial) = accuracy;
  
  if updatePopup
    % update the progress bar
    PopupProgress(popupName);
  end
end


if options.numTestDataSets == 1
  % display detailed results of the one test data set
  displaySingleTrialResults(results, options, funcName);
else
  % display summary data of many trials
  displayMultiTrialResults(results, options, funcName);
end

% shut down the parallel environment if it wasn't running at the
% beginning
if options.useParallel
  parBlock.endBlock();
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [testData, clustNums] = generateTestData(numClusters, numDims, ...
					       numDimsSep, numPts, separation)
% Generate synthetic data that should be highly separated along some
% dimensions (thus has well defined clusters)
%   -The dimensions that matter are 1:numDimsSep
%   -Points are randomly distributed along the other dimensions regardless
%    of which cluster they belong to

% set an acceptable range of separation between cluster centroids
%sepRange = 2 * [0.9 * separation, 1.1 * separation];
sepRange = [0.9 * separation, 1.1 * separation];

% define the centroids, the first one will be at the origin
centroids = zeros(numClusters, numDims);
%range = numClusters * separation;
testP = zeros(1, numDims);
for n = 2:numClusters
  % add additional centroids, guaranteeing they are appropriately spaced

  while true
    % try to add a new centroid, loop until successful
    
    % create a test point
    %   first locate reasonable bounds to look in
    minVals = min(centroids, [], 1) - separation;
    maxVals = max(centroids, [], 1) + separation;
    range = maxVals(1:numDimsSep) - minVals(1:numDimsSep);
    %   then pick a point randomly (uniformly) from that region
    testP(1:numDimsSep) = range .* rand(1, numDimsSep) + ...
                          minVals(1:numDimsSep);
    
    % measure the distance from testP to the nearest existing centroid
    minD = Inf;
    for k = 1:(n-1)
      dist = norm(testP - centroids(k,:));
      if dist < minD
        minD = dist;
      end
    end
    
    if sepRange(1) < minD && minD < sepRange(2)
      % if the distance is within acceptable bounds, accept the point as a
      % new centroid
      centroids(n,:) = testP;
      % stop looping and move to next centroid
      break
    end
  end
end

% Create data as a gaussian cloud around the centroids
%  1. Randomly assign each point to a centroid, and set it at the location
%     of that centroid
while true
  clustNums = randi(numClusters, [numPts, 1]);
  if length(unique(clustNums)) == numClusters
    % make sure each cluster has at least one point in it (should almost
    % always be the case)
    break
  end
end
%  2. Add Gaussian noise to each point
testData = centroids(clustNums,:) + randn(numPts, numDims);
%  3. Remove information about scale:
testData = zscore(testData);
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function funcName = getFuncName(clustFunc)
funcName = func2str(clustFunc);
ind1 = strfind(funcName, '@') + 1;
if ~isempty(ind1)
  funcName = funcName(ind1:end);
end

ind1 = strfind(funcName, '(');
while ~isempty(ind1)
  ind2 = ind1 + strfind(funcName((ind1+1):end), ')');
  keepInd = [1:(ind1-1), (ind2+1):length(funcName)];
  funcName = funcName(keepInd);
  ind1 = strfind(funcName, '(');
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function displaySingleTrialResults(results, options, funcName)
% display detailed results of the one test data set
  
fprintf( ...
  'Percent correct: %g, Adjusted mutual info: %g, p-value: %g\n', ...
  100 * results.proportionCorrect, results.adjustedMi, results.pValue);
fprintf( ...
  '  Mutual info: %g, Max mutual info: %g, Expected Mutual Info: %g', ...
  results.mutualInfo, results.maxMi, results.meanMi);

if results.proportionCorrect > 0.01 * options.passAccuracy
  fprintf(' [Passed]\n')
else
  fprintf(2, ' [Failed]\n');
end

fprintf('Cluster assignments: (trueCluster x assigned)\n')
for n = 1:size(results.confusionMat, 1)
  fprintf('%2d: ', n)
  fprintf(' %4d', results.confusionMat(n,:))
  fprintf('\n')
end


if options.makePlots
  plotTitle = sprintf('Test %s clustering', funcName);
  h = PlotClassification(results.testData, results.clustNums, ...
                         'trueGroupNums', results.trueClustNums, ...
                         'title', plotTitle);
  set(h, 'WindowStyle', 'docked')
end

return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function displayMultiTrialResults(results, options, funcName)
% display summary data of many trials

percentCorrect = 100 * [results.proportionCorrect];

resultStr = sprintf('Mean percent correct: %g +- %g', ...
                    mean(percentCorrect), std(percentCorrect));
fprintf(resultStr)
percentPassed = 100 * sum(percentCorrect > options.passAccuracy) / ...
                length(percentCorrect);
if all(percentCorrect > options.passAccuracy)
  fprintf(' [100%% Passed]\n')
else
  fprintf(2, ' [%d%% Passed, %d%% Failed]\n', ...
          percentPassed, 100 - percentPassed);
end

if options.makePlots
  % make a histogram of the fraction correct
  plotTitle = sprintf('%s clustering, Fraction Correct', funcName);
  h = NamedFigure(plotTitle);
  numBins = max([10, round(sqrt(length(percentCorrect)))]);
  [nums, binX] = hist(percentCorrect, numBins);
  bar(binX, nums)
  xlim([0 100])
  xlabel('% Correct', 'FontName', 'Arial', 'FontSize', 16)
  ylabel('# Trials', 'FontName', 'Arial', 'FontSize', 16)
  title(plotTitle, 'FontName', 'Arial', 'FontSize', 16)
  text(25, 0.8 * max(nums), resultStr, 'FontName', 'Arial', 'FontSize', 12)
  set(h, 'WindowStyle', 'docked')
  
  % check variation in the number of extraneous dimensions
  numExtraneous = [results.numDims] - [results.numDimsSep];
  rangeExtraneous = max(numExtraneous) - min(numExtraneous);
  if rangeExtraneous > 0
    % make a scatter plot of percent correct vs number extraneous
    % dimensions
    plotTitle = sprintf('%s clustering, Effect of extraneous dims', funcName);

    h = NamedFigure(plotTitle);
    
    clf
    if options.passAccuracy < 100
      % draw a rectangle showing the "pass" zone
      rectPos = [min(numExtraneous), options.passAccuracy, ...
        max(numExtraneous) - min(numExtraneous), ...
        100 - options.passAccuracy];
      rectangle('Position', rectPos, 'EdgeColor', 'none', 'FaceColor', 'g')

      hold on
    end
    
    plot(numExtraneous, percentCorrect, 'ko', 'MarkerSize', 10, ...
      'MarkerFaceColor', 'k')
    hold off
    
    xlabel('# Extraneous dimensions', 'FontName', 'Arial', 'FontSize', 16)
    ylabel('% Correct', 'FontName', 'Arial', 'FontSize', 16)
    title(plotTitle, 'FontName', 'Arial', 'FontSize', 16)
    
    set(h, 'WindowStyle', 'docked')
  end
end
return
