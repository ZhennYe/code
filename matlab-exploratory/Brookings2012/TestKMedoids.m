function TestKMedoids(varargin)
% TestKMedoids(options)
% Test the KMedoids clustering function by generating one or more synthetic
% data sets, running the algorithm, and comparing its results to the
% correct results.
%  INPUTS:
%   -clustFunc: handle to cluster function, of form
%               clustFunc(dataMat, numClusters)
%               where dataMat is numPoints x numDims matrix of points to be
%               clustered and numClusters is the number of clusters to look
%               for. If options must be passed to the cluster function or
%               numClusters ignored, define a function handle and pass it
%               to TestCluster
%  options
%   -makePlots         true
%       visually summarize results
%   -numTestDataSets   100
%       create this many synthetic data sets and try to cluster them
%   -numTrials         200
%       run KMedoids algorithm this many times for each synthetic data set,
%       then keep the best outcome as the final result
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


% get options to control the test
defaultOptions = { ...
  'makePlots', true, ...
  'numTestDataSets', 100, ...
  'numTrials', 200, ...
  'clusterSeparation', 8, ...
  'minNumClusters', 3, ...
  'maxNumClusters', 6, ...
  'minNumDims', 4, ...
  'maxNumDims', 10, ...
  'passAccuracy', 90, ...
  'useParallel', true ...
  };
options = GetOptions(defaultOptions, varargin);

% make a structure of options to pass to KMedoids
clusterOptions = struct( ...
  'numTrials', options.numTrials, ...
  'useParallel', options.useParallel, ...
  'displayTrialProgress', options.numTestDataSets == 1 ...
  );

% make the function handle, pass extra options and ignore clustNums
clustFunc = ...
  @(data, numClust, clustNums) KMedoids(data, numClust, clusterOptions);

% form the structure of options for TestCluster)
testOptions = rmfield(options, 'numTrials');

% call the main routine to test different clustering schemes
TestCluster(clustFunc, testOptions);
return
