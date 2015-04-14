function clustStruct = KMedoids(dataMat, k, varargin)
% clustStruct = KMedoids(dataMat, k, options)
% Implements k-medoids++ clustering algorithm (k-medoids with a
%   particular initialization choice).
%  INPUTS:
%    dataMat: numDataPoints x numDataDim matrix of points
%    k:  Number of clusters to find
%   OPTIONS
%    numTrials              200
%       run KMedoids algorithm this many times for each synthetic data set,
%       then keep the best outcome as the final result
%    useParallel            true
%       use parfor when possible to speed up computation
%    displayTrialProgress   true
%       create a progress bar to display the number of trials completed
%    normPower              1
%       power of the norm used to measure distance between points
%    distanceMat            []
%       list of pairwise distances between points. If empty, compute the
%       distances using norm, if supplied, ignore dataMat and cluster using
%       the provided matrix
%  OUTPUTS:
%    clustStruct: Structure with fields specifying results of clustering
%       clustInds:  numPoints x 1 list of which cluster each point is in
%       cost:  average distance from each point to its medoid
%       medoids:  k x 1 list of indices indicating the medoids for each
%                   cluster


if nargin < 2
  help KMedoids
  if nargin == 0
    return
  else
    error('Invalid number of inputs.')
  end
end

defaultOptions = { ...
  'numTrials', 1, ...
  'useParallel', true, ...
  'displayTrialProgress', true, ...
  'normPower', 1, ...
  'distanceMat', [] ...
  };
options = GetOptions(defaultOptions, varargin);
if options.numTrials < 1
  help KMedoids
  error('Invalid number of trials:  %g\n', options.numTrials)
end

if options.useParallel
  % open matlab pool if it isn't already open
  parBlock = ParallelBlock();
end

% get a numPoints x numPoints matrix of distances between points
if isempty(options.distanceMat)
  % compute distMat using norm
  
  distMat = getDistMat(dataMat, options.normPower);
else
  % use supplied distanceMat
  
  distMat = options.distanceMat;
  if ~isempty(dataMat) && size(dataMat, 1) ~= size(distMat, 1)
    error('Number of points in dataMat doesn''t agree with distanceMat')
  elseif size(distMat, 1) ~= size(distMat, 2)
    error('distanceMat must be square')
  end
end

% find the best clustering out of options.numTrials attempts
clustStruct = kMedoidTrials(distMat, k, options);


if options.useParallel
  % close the matlab pool if it is no longer required
  parBlock.endBlock();
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function distMat = getDistMat(dataMat, normPower)
% get a numPoints x numPoints matrix of distances between points

% allocate space for distMat
numPoints = size(dataMat, 1);
distMat = zeros(numPoints, numPoints);

% compute distMat
for n = 1:numPoints
  for m = (n+1):numPoints
    distMat(n,m) = distanceFunc(dataMat(n,:), dataMat(m,:), normPower);
    distMat(m,n) = distMat(n,m);
  end
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function distance = distanceFunc(p1, p2, normPower)
% compute the distance between p1 and p2 using the specified norm

delta = p1 - p2;
finiteInd = isfinite(delta);
if normPower == 1
  distance = norm(delta(finiteInd), normPower) / sum(finiteInd);
else
  distance = norm(delta(finiteInd), normPower) / ...
             sum(finiteInd)^(1.0 / normPower);
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function best = kMedoidTrials(distMat, k, options)
% Do the k_medoids routine repeatedly, varying only the initial set of
% medoids 

% get the list of which point is the initial 1st medoid for each trial
initialMedoids = getInitialMedoids(distMat, options.numTrials);

if options.numTrials == 1
  % only one trial, call doKMedoids and return the result
  
  % do the k-medoids algorithm and return results as a structure
  best = doKMedoids(distMat, k, initialMedoids(1));
  % compute clustering cost as the mean distance to each point's medoid
  best.cost = mean(best.medoidDistances);
else
  % many trials, keep the one with the least cost
  
  % create a struct to store the best results in, initialize to a dummy
  % struct with infinite cost
  best = struct('cost', Inf);
  
  if options.displayTrialProgress
    % create a progress bar
    PopupProgress('KMedoids', options.numTrials)
    updatePopup = true;
  else
    updatePopup = false;
  end
  
  parfor n = 1:options.numTrials
    %Do the trial with the nth version of initialized clusters
    
    % do the k-medoids algorithm and return results as a structure
    this = doKMedoids(distMat, k, initialMedoids(n));
    % compute clustering cost as the mean distance to each point's medoid
    this.cost = mean(this.medoidDistances);
    % update best appropriately
    best = keepBest(this, best);

    if updatePopup
      % update progress bar
      PopupProgress('KMedoids')
    end
  end
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function initialMedoids = getInitialMedoids(distMat, numTrials)
% get the list of which point is the starting 1st medoid for each trial

numPoints = size(distMat, 1);
if numTrials > numPoints
  % more trials than points, pick the starting point randomly
  initialMedoids = randi(numPoints, [1, numTrials]);
else
  % few trials than points, scrample the points' order and work through the
  % scrambled list
  initialMedoids = randperm(numPoints);
  initialMedoids = initialMedoids(1:numTrials);
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function best = keepBest(newResult, best)
% update the "best" structure to reflect the lowest-cost cluster trial

if newResult.cost < best.cost
  best = newResult;
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function clustStruct = doKMedoids(distMat, k, initialMedoid)
% do the k-medoids algorithm and return results as a structure

% get the first medoids randomly, setting initialMedoid as the starting
% medoids #1
medoids = getFirstMedoids(distMat, k, initialMedoid);

% assign points to the cluster whose medoid they are closest to
[medoidDistances, clustInds] = min(distMat(:, medoids), [], 2);


for numItr = 1:size(distMat, 1);
  % iterate until cluster indices converge, or reach max # of iterations
  % (in this case, max # iterations = # points)
  
  % update the medoids to better represent the putative clusters
  medoids = updateMedoids(distMat, medoids, clustInds);

  % save the old cluster points to test for convergence
  oldClustInds = clustInds;
  % assign points to the cluster whose medoid they are closest to
  [medoidDistances, clustInds] = min(distMat(:, medoids), [], 2);
  
  if all(clustInds == oldClustInds);
    % cluster indices coverged, so stop iterating
    break
  end
end

% store results in a convenient structure
clustStruct = struct( ...
  'clustInds', clustInds, ...
  'medoidDistances', medoidDistances, ...
  'medoids', medoids, ...
  'cost', [] ...
  );
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function medoids = getFirstMedoids(distMat, k, firstMedoid)
% Choose starting cluster medoids randomly by the following method:
%  -The first point is passed to this function
%  -Choose each subsequent point randomly from remaining points, with
%   probability proportional to distance from the nearest medoid
% (Note that this method is similar to that used by k-means++)

% set the first medoid
medoids = NaN(k, 1);
medoids(1) = firstMedoid;

% allocate probSelect
numPoints = size(distMat, 1);
probSelect = zeros(numPoints, 1);

for n = 2:k
  % find the nth medoid, starting at n=2
  
  for ind = 1:numPoints
    % compute the probability of selection for each point
    
    if ismember(ind, medoids(1:(n-1)))
      % don't select previous medoids
      probSelect(ind) = 0;
    else
      % for each non-medoids point, the probability of selection is
      % proportional to the distance to the closest medoid
      probSelect(ind) = min(distMat(ind, medoids(1:(n-1))));
    end
  end
  
  % make the probSelect cumulative
  probSelect = cumsum(probSelect);
  
  % choose proprortionally to probability
  chooseInd = find(probSelect >= rand(1,1) * probSelect(end), 1);
  
  % set the nth medoid to be the one chosen randomly
  medoids(n) = chooseInd;
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function medoids = updateMedoids(distMat, medoids, clustInds)
% choose new medoids by selecting the member of each cluster whose distance
% to the rest of the cluster is minimal

k = size(medoids, 1);

for n = 1:k
  %loop through each cluster
  
  %get the data that are in this cluster
  ind = find(clustInds == n);

  %  chose the medoid with the lowest within-cluster distance
  [~, m] = min(sum(distMat(ind, ind), 2));
  medoids(n) = ind(m);
end
return
