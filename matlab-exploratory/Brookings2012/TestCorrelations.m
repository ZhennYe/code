function TestCorrelations(varargin)
% TestCorrelations(options)
% Randomly generate some correlated data and use it to test
% FindPropertyCorrelations.m 
%   options  default options can be overloaded by passing ordered key/value
%            pairs or a single structure with key/value elements.
%    numData    40
%       Number of data points (rows)
%    numXProps  5
%       Number of x properties (columns of x)
%    numYProps  6
%       Number of y properties (columns of y)
%    significanceLevel  0.05
%       Threshold for a p-value to be considered significant
%    numShuffledTrials  10000
%       Number of shuffled trials to conduct when assessing p-values
% OUTPUT
%   The output is the display (from FindPropertyCorrelations) of the
%  correlations found (x-y, then x-x, then y-y). This is only to display the
%  full range of capabilities of FindPropertyCorrelations.m ---if you actually
%  wanted to test all possible correlations, you should form xJoint = [x y] so
%  that the p-value adjustment for multiple correlations is correct.
%
%  Assuming that the routines are
%  working properly there are a few results to note:
%    SIGNIFICANCE:
%     -if a property being "fit" is correlated with other properties being
%      tested, it is highly likely that a significant correlation will be
%      detected
%     -if a property being "fit" has *no* correlations, it is highly unlikely
%      that a significant correlation will be detected
%    ASSOCIATION: (which properties correlate)
%     -in significant correlations, the "real" correlations will typically be
%      present, and typically be listed first. However extraneous correlations
%      are likely to be found. This is due to the method of "greedy feature
%      selection" used in FindCorrelations.m. There, quality of fit is
%      expressed as sum of square errors per degree of freedom, which doesn't
%      sufficiently penalize 

defaultOptions = { ...
  'numData', 40, ...
  'numXProps', 5, ...
  'numYProps', 6, ...
  'significanceLevel', 0.05, ...
  'numShuffledTrials', 10000 ...
};

options = GetOptions(defaultOptions, varargin);

% randomly generate some correlated data to test
%   -there should be no x-x correlations
%   -y_1 should correlate with x_N
%   -y_2 should correlated with x_2, x_3, ... x_N
%   -y_3 should correlated with y_4
%   -due to shared correlation with x_N, y_1 should weakly correlate with y_2
[xData, yData] = generateData(options);

FindPropertyCorrelations(xData, yData, options)

FindPropertyCorrelations(xData, [], options)

options.xLabel = 'y';
FindPropertyCorrelations(yData, [], options)
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [xData, yData] = generateData(options)
% generate random data
xData = randn(options.numData, options.numXProps);
yData = randn(options.numData, options.numYProps);

% add in some correlations
%   - y_1 is correlated with x_N
yData(:,1) = 0.1 * yData(:,1) + 0.8 * xData(:,options.numXProps);
%   - y_2 = sum_(j = 2:N) x_j
yData(:,2) = sum(xData(:,2:options.numXProps), 2);
%   - y_3 = 0.2 * randn + 0.8 * y_4
yData(:,3) = 0.2 * yData(:,3) + 0.8 * yData(:,4);

xZ = ZScore(xData, 1, 1);
yZ = ZScore(yData, 1, 1);

% uncomment any of these blocks to see the the pairwise correlations
%xZ' * xZ ./ options.numData
%yZ' * yZ ./ options.numData
%xZ' * yZ ./ options.numData
return
