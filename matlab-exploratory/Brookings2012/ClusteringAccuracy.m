function accuracy = ClusteringAccuracy(clustNums, trueClustNums, varargin)
% accuracy = ClusteringAccuracy(clustNums, trueClustNums, ...
%                                            compareConfusionMat, numRandom)
% Calculate the accuracy in a clustering algorithm operating on
%    known true clusters.
% Accuracy is reported as the adjusted mutual information.
%  INPUTS:
%   -clustNums        list of indices to found clusters
%   -trueClustNums    list of indices to true clusters
%   OPTIONS:
%   -calcAdjustedMi       true
%     computed the adjusted mutual information
%   -calcPValue           true
%     compute the probability of the null hypothesis that this clustering
%     is the same as the one specified by compareConfusionMat
%   -compareConfusionMat  []
%     when computing p-values, compare the cluster results to those
%     specified by compareConfusionMat. If empty, compare to random numbers
%   -numRandom            10000
%     when computing a distribution of clusterings, make this many
%  OUTPUTS:
%   -Accuracy:  structure containing information about accuracy/results of
%               the clustering. Depending on the input options, some
%               fields may not be computed
%      mutualInfo      mutual information between found and true clusters
%      maxMi           mutual information of perfect agreement
%      meanMi          (only if calcAdjustedMi) mean of distribution of
%                      random clusterings
%      adjustedMi      (only if calcAdjustedMi) mutual information scaled
%                      so that random ~ 0 and perfect = 1
%      pValue          (only if calcPValue) probability of the null
%                      hypothesis that this clustering is the same as the
%                      one specified by compareConfusionMat
%      clustNums       remapped cluster numbers (organized so that the
%                      maximum number of points are assigned to the
%                      "correct" group)
%      confusionMat    confusion matrix of remapped clustNums
%      proportionCorrect fraction of points correctly assigned after the
%                      remapping proceedure

if length(trueClustNums) ~= length(clustNums)
  error(['clustered group numbers and true group numbers have ' ...
    'different lengths'])
end

% get the options to control how accuracy is computed
defaultOptions = {...
  'calcAdjustedMi', true, ...
  'calcPValue', true, ...
  'compareConfusionMat', [], ...
  'numRandom', 10000 ...
  };
options = GetOptions(defaultOptions, varargin, true);


% convert the information in clustNums and trueClustNums into a confusion
% matrix, a format that is more convenient for analyzing accuracy
confusionMat = getConfusionMatrix(clustNums, trueClustNums);


% compute the accuracy in terms of mutual information
accuracy = getAccuracy(confusionMat, options);

% remap the cluster assignments (swap rows) so as to achieve the largest
% possible fraction correct (maximize the main diagonal of confusionMat)
[accuracy.confusionMat, accuracy.clustNums] = ...
  remapClusterAssignments(confusionMat, clustNums);

% compute the fraction of points correctly clustered
accuracy.proportionCorrect = ...
  sum(diag(accuracy.confusionMat)) / sum(accuracy.confusionMat(:));
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function mat = getConfusionMatrix(clustList, trueClustList)
% make sure that we're dealing with integers from 1 to numTypes
[cNames, ~, cNums] = unique(clustList);
[trueCNames, ~, trueCNums] = unique(trueClustList);
numC = length(cNames);
numTrueC = length(trueCNames);

mat = zeros(numC, numTrueC);
for ind = 1:numC
  for trueInd = 1:numTrueC
    mat(ind,trueInd) = sum(cNums == ind & trueCNums == trueInd);
  end
end
return




%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function accuracy = getAccuracy(confusionMat, options)
% calculate the adjusted mutual information, and p-value that the
% clustering is no better than random (or the clustering described by the
% given confusion matrix compareConfusionMat.

% calculate mutual information as well as entropy of each separate
% clustering
[mutualInfo, entropyClust, entropyTrueClust] = getMutualInfo(confusionMat);

% this is the maximum mutual information: when the clustering with the most
% entropy tells you *everything* about the other clustering
maxMutualInfo = max(entropyClust, entropyTrueClust);

% organize info computed so far into struct
accuracy = struct( ...
  'mutualInfo', mutualInfo, ...
  'maxMi', maxMutualInfo ...
  );

if ~options.calcAdjustedMi && ~options.calcPValue
  % calculated all that was requested, so return
  return
end


% Adjusted Mutual Info (AMI) or p-value was requested, so we need to
% compute the probability distribution of mutual information.
%   -For AMI, we need to compute the mean value of mutual info for random
%    clusterings
%   -For p-values we need to compute the full distribution associated with
%    compareConfusionMat (assumed to be random if compareConfusionMat is
%    empty)

% fix the number of true clusters:
nTrueClust = sum(confusionMat, 1);

% when forming the distribution of clustering outcomes, measure mutual info
measureFunc = @getMutualInfo;


if options.calcAdjustedMi || isempty(options.compareConfusionMat)
  % need to compute the distribution of mutual info for random clusterings
  
  % generate a random confusion matrix with
  %   1. the correct number of points in each true cluster (column)
  %   2. equal probability of assignment to each found cluster (row)
  compareConfusionMat = repmat(nTrueClust, size(confusionMat, 1), 1);
  
  % compute the distribution of mutual information associated with this matrix
  mi = getRandomClusteringDist(options.numRandom, measureFunc, ...
                               nTrueClust, compareConfusionMat);
  
  if options.calcAdjustedMi
    % compute the mean value of mutual info for random clusterings
    accuracy.meanMi = mean(mi);
    
    % compute AMI
    accuracy.adjustedMi = ...
      (mutualInfo - accuracy.meanMi) / (maxMutualInfo - accuracy.meanMi);
  end
  
  if options.calcPValue && isempty(options.compareConfusionMat)
    % compute the p-value of this cluster being better than random
    
    % if you assume p has a uniform prior on [0,1] and compute the expectation
    % value of p given the distribution of mi observed, this is what you obtain
    accuracy.pValue = (1 + sum(mi >= mutualInfo)) / (1 + length(mi));
  end
end

if options.calcPValue && ~isempty(options.compareConfusionMat)
  % compute the p-value of this cluster being different from another cluster,
  % specified by options.compareConfusionMat
  
  compareConfusionMat = options.compareConfusionMat;
  
  % get the mutual info of the comparision cluster to see which is better
  compareMi = getMutualInfo(compareConfusionMat);
  
  % form the distribution of mutual info for whichever clustering is worse
  % then compute the probability that the better clustering was drawn from the
  % distribution of mutual info of the worse one
  if mutualInfo < compareMi
    % form the distribution of mutual info by drawing the number of true
    % clusters specified by the columns of compareConfusionMat, but the
    % assignement probabilities specified by confusionMat
    
    % if the two clusterings were performed on the same data, then
    % nCompareClust == nTrueClust
    nCompareClust = sum(compareConfusionMat, 1);
    
    mi = getRandomClusteringDist(options.numRandom, measureFunc, ...
                                 nCompareClust, confusionMat);
  
    accuracy.pValue = (1 + sum(mi >= compareMi)) / (1 + length(mi));
  else
    % form the distribution of mutual info by drawing the number of true
    % clusters specified by the columns of confusionMat, but the assignement
    % probabilities specified by compareConfusionMat

    mi = getRandomClusteringDist(options.numRandom, measureFunc, ...
                                 nTrueClust, compareConfusionMat);
  
    accuracy.pValue = (1 + sum(mi >= mutualInfo)) / (1 + length(mi));
  end
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [mutualInfo, varargout] = getMutualInfo(confusionMat)
% compute the mutual information from a confusion matrix
% INPUTS
%   confusionMat      a confusion matrix for a given clustering
% OUTPUTS
%   mutualInfo        mutual information between rows and columns of the matrix
%                     (decrease in entropy by knowing both true and found
%                     clusters)
%  OPTIONAL
%   entropyClust      entropy of the rows (found clusters)
%   entropyTrueClust  entropy of the columns (true clusters)

% turn the integer counts in confusionMat into a matrix of probabilities
probMat = confusionMat ./ sum(confusionMat(:));

% calculate the entropy of the clusters, true clusters, and the joint
% partition:
%  1. found clusters are the rows
pClust = sum(probMat, 2);
entropyClust = getEntropy(pClust);
%  2. true clusters are the columns
pTrueClust = sum(probMat, 1);
entropyTrueClust = getEntropy(pTrueClust);
%  3. the joint entropy is considering each matrix element separately
entropyJoint = getEntropy(probMat(:));

% calculate the mutual information: the decrease in entropy by knowing both
% clusterings:
mutualInfo = entropyClust + entropyTrueClust - entropyJoint;
if nargout > 1
  varargout = {entropyClust, entropyTrueClust};
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function entropy = getEntropy(probList)
% compute the entropy of a set of probabilities (in this case, they are derived
% from the probability of certain outcomes in clustering, as specified by a
% confusion matrix, although the formula for entropy is general

% get only the non-zero probabilities
probList = probList(probList > 0);
% scale to make sure they're probabilities
probList = probList / sum(probList);

% compute the entropy
entropy = -sum(probList .* log(probList));
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function qualityDist = ...
  getRandomClusteringDist(numRandom, measureFunc, nTrueClust, seedProbMat)
% get the distribution of some quality (calculated by measureFunc) given a
%  contingency matrix of probabilities

qualityDist = NaN(1, numRandom);

% make sure seedProbMat is a list of probabilities
for n = 1:size(seedProbMat, 2);
  seedProbMat(:,n) = seedProbMat(:,n) / sum(seedProbMat(:,n));
end

parfor n = 1:numRandom
  confusionMat = generateRandomConfusionMat(nTrueClust, seedProbMat);
  qualityDist(n) = measureFunc(confusionMat); %#ok<PFBNS>
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function confusionMat = generateRandomConfusionMat(nTrueClust, seedProbMat)
% randomly generate a confusion matrix (output of a clustering algorithm)
%  INPUTS
%    nTrueClust    1 x numTrueClusters array of number of points in each
%                  true cluster
%    seedProbMat   numFoundClusters x numTrueClusters matrix whose columns
%                  specify the probability that a point in the
%                  corresponding true cluster will be found to be in the
%                  corresponding row.
%  OUTPUTS
%    confusionMat  numFoundClusters x numTrueClusters matrix specifying the
%                  number of points in true cluster (column) that were
%                  classified in a found cluster (row)

confusionMat = zeros(size(seedProbMat));
while true
  % keep trying until a confusion matrix is successfully generated

  for trueInd = 1:size(seedProbMat, 2)
    % loop for each column (each true cluster)
    
    % assign numAssign points to random clusters
    numAssign = nTrueClust(trueInd);
    
    % assign them weighted by seedProbMat
    pAssign = cumsum(seedProbMat(:,trueInd));
  
    for n = 1:numAssign
      % calculate the random index
      ind = find(rand(1) <= pAssign, 1, 'first');
      % add a point in that joint true cluster / assigned cluster location
      confusionMat(ind, trueInd) = confusionMat(ind, trueInd) + 1;
    end
  end
  
  % nClust is the sum across columns for each row
  nClust = sum(confusionMat, 2);
  
  % clustering algorithms always assign at least one point to each cluster,
  % so we're only successful if each cluster has some non-zero probability
  if any(nClust == 0)
    % zero out the confusion matrix and try again
    confusionMat(:) = 0;
  else
    % successfully generated a valid confusion matrix, return
    break
  end
end

return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [confusionMat, groupNums] = ...
  remapClusterAssignments(confusionMat, groupNums)
% remap the cluster assignments (swap rows) so as to achieve the largest
% possible fraction correct (maximize the main diagonal of confusionMat)

labels = unique(groupNums);

numTrueClust = size(confusionMat, 2);
for n = 1:numTrueClust
  [maxVal, maxRow] = max(confusionMat(:,n));
  if confusionMat(n,n) == maxVal
    continue
  end
  if confusionMat(n,n) + confusionMat(maxRow,maxRow) > ...
      maxVal + confusionMat(n,maxRow)
    % not worth swapping
    continue
  end
  %swap rows
  temp = confusionMat(n,:);
  confusionMat(n,:) = confusionMat(maxRow,:);
  confusionMat(maxRow,:) = temp;
  
  %swap group assignments
  ind_n = (groupNums == labels(n));
  ind_maxRow = (groupNums == labels(maxRow));
  groupNums(ind_n) = labels(maxRow);
  groupNums(ind_maxRow) = labels(n);
end
return
