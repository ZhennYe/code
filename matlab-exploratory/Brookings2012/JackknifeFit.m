function [yPredict, varargout] = JackknifeFit(x, y)
% [yPredict, pVal, rSquared] = JackknifeFit(x, y)
% Performs jackknifed linear fit of y to x:  y = A * x + B
% For each index n, yPredict(n) is obtained by
%    1) excluding the corresponding values x(n) and y(n)
%    2) doing a linear fit, yRemain = A * xRemain + B
%    3) yPredict(n) = A * x(n) + B
% [Note, even if x and y have zero means, there's no benefit to NOT fitting
%  the offset, so don't try to add that capability later!]
%  INPUTS:
%   -x: nData x nX-types matrix
%   -y: nData x nY-types matrix
%  OUTPUTS:
%   -yPredict: nData x nY-types matrix of predicted y values.
%               The nth yPredict value is predicted from a linear
%               fit that *excludes* the nth x and y values.
%   OPTIONAL:
%   -pVal:  nY-types array of p-values, calculated from Pearson
%               correlation value between y and yPredict.
%   -rSquared: nY-types array of R^2 values, between y and yPredict.

if nargin ~= 2
  help JackknifeFit
  error('Incorrect number of input arguments.')
end
if nargout > 3
  help JackknifeFit
  error('Too many output arguments.')
end
nData = size(x, 1);
if size(y,1) ~= nData
  error('x and y must have the same number of rows')
end

nYTypes = size(y, 2);
yPredict = NaN(nData, nYTypes);

% find the good x-data
goodX = find(sum(~isfinite(x), 2) == 0);
nGoodX = length(goodX);
% add a column of ones, so as to fit the offset
x = [x, ones(nData,1)];

% do the jackknifed fits
for n = 1:nGoodX
  ind = goodX([1:(n-1),(n+1):nGoodX]);
  m = goodX(n);
  for k = 1:nYTypes
    ind_k = ind(isfinite(y(ind, k)));
    coef = pinv(x(ind_k, :)) * y(ind_k, k);
    yPredict(m, k) = x(m, :) * coef;
  end
end

% do additional calculations, based on requested output
switch nargout
 case 1,
  varargout = {};
  
 case 2,
  pVal = zeros(nYTypes,1);
  for n = 1:nYTypes
    [~, pVal(n)] = Pearson(y(:,n), yPredict(:,n));
  end
  varargout = {pVal};
  
 case 3,
  pVal = zeros(nYTypes,1);
  rSquared = zeros(nYTypes,1);
  for n = 1:nYTypes
    [~, pVal(n)] = Pearson(y(:,n), yPredict(:,n));
    finiteInd = find(isfinite(y(:,n)) & isfinite(yPredict(:,n)));
    
    yDev = y(finiteInd,n);
    yErr = yPredict(finiteInd,n) - yDev;
    rSquared(n) = 1 - cov(yErr) / cov(yDev);
    if isnan(rSquared(n))
      fprintf(2, 'NaN in JackknifeFit.m\n');
      keyboard
    end
  end
  varargout = {pVal, rSquared};
end
return