function xScored = ZScore(x, varargin)
% xScored = ZScore(x, dim, flag)
% if x contains no NaNs or Infs, then works the same as the built-in
% function z-score: returns xScored = (x - mean(x)) / std(x)
% otherwise z-scores along the desired dimension (dim) while ignoring the
% presence of non-finite elements
%
% Note: the order of flag was reversed because in my opinion it's not a
% particularly useful value to override
%
% INPUTS
%   x   array of values to be z-scored
%  (OPTIONAL)
%   dim (default = first nontrivial) dimension to perform z-score along
%   flag (default = 0) flag = 0 or 1, normalize the std(x) by:
%                      std(x) = cov(x) / (N - 1 + flag)

if nargin > 3 || nargin < 1
  help ZScore
  error('Incorrect number of arguments to ZScore')
end

% find the appropriate dim
if nargin >= 2
  % dim is specified
  dim = varargin{1};
else
  % dim is unspecified, 
  dim = find(size(x) ~= 1, 1);
  if isempty(dim)
    xScored = x;
    return
  end
end

if size(x, dim) < 2
  % x is trivial along this dimension, just set xScored = x and return
  xScored = x;
  return
end

% find the appropriate normalizing offset for std
if nargin == 3
  flag = varargin{2};
  if flag ~= 0 && flag ~= 1
    error('value of flag must be 0 or 1')
  end
else
  flag = 0;
end

xIsFinite = isfinite(x);
if all(xIsFinite(:))
  % everything is finite, use normal z-score
  
  xMean = mean(x, dim);
  xStd = std(x, flag, dim);
  xStd(xStd == 0) = 1;
  xScored = bsxfun(@minus, x, xMean);
  xScored = bsxfun(@rdivide, xScored, xStd);
else
  % some non-finite x values, z-score by ignoring them
  
  xScored = x;
  % temporarily set the non-finite values to zero
  xScored(~xIsFinite) = 0.0;
  % count the number of finite (use this to normalize xMean, xStd)
  numFinite = sum(xIsFinite, dim);
  xMean = sum(xScored, dim) ./ numFinite;
  
  xScored = bsxfun(@minus, xScored, xMean);
  
  xStd = sqrt(sum(xScored.^2, dim) ./ (numFinite - (1 - flag)));
  xStd(xStd == 0) = 1;
  
  xScored = bsxfun(@rdivide, xScored, xStd);
  
  % restore non-finite values
  xScored(~xIsFinite) = x(~xIsFinite);
end

return
