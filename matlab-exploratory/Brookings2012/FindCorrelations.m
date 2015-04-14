function [rChiSquared, rSquared, varargout] = ...
  FindCorrelations(x, y, varargin)
% [rChiSquared, rSquared, fitIndices, yCells, yPredict, coefs, pVals] = ...
%    FindCorrelations(x, y, maxLookAhead, maxCombos, fitOffset)
%
%  Try to find correlations of the form y = A * x + B
%  Search through relationships with varying numbers of xs,
%  using f-statistic to justify adding more.
%  The first form will print out values or make plots, while the
%  second form will silently return the results.
%
%  INPUTS:
%    x: Dependent variables, numData x numX matrix
%    y: Independent variables, numData x numY matrix
%   (OPTIONAL)
%     maxLookAhead: number of parameters to consider adding at once
%                   to the current best fit.  Defaults to 3.
%     maxCombos: maximum number of parameters to fit with.
%                Defaults to Inf
%     fitOffset: set to true if your data is not zero-mean
%                Defaults to false
%  OUTPUTS:
%    rChiSquared: numY x 1 array of reduced sum of chi^2 per degree
%                 of freedom for each y fit
%    rSquared:  numY x 1 array of R^2 for each y fit
%   (OPTIONAL)
%    fitIndices: numY x 1 cell array of arrays of x-indices.  The
%                nth cell array gives the x-indices used to fit
%                the nth Y variable.
%    yCells: numY x 1 cell array of individial y traces
%    yPredict: numY x 1 cell array of predicted y traces
%    coefs:   numY x 1 cell array of coefficients from fit
%    pVals:  numY x 1 cell array of p-values obtained by using
%            JackknifeFit and a Pearson correlation test.

defaultOptions = {'maxLookAhead', 2, 'maxCombos', Inf, 'fitOffset', false};
options = GetOptions(defaultOptions, varargin, true);
if nargout >= 7
  options.getPVals = true;
else
  options.getPVals = false;
end

numData = size(x, 1);
if size(y,1) ~= numData
  error('x and y must have the same number of rows')
end
numX = size(x,2);
numY = size(y,2);

rChiSquared = zeros(numY, 1);
rSquared = zeros(numY, 1);
fitIndices = cell(numY, 1);
yCell = cell(numY, 1);
yPredictCell = cell(numY, 1);
coefs = cell(numY, 1);
if options.getPVals
  pVals = zeros(numY, 1);
end

for m = 1:numY
  % loop through each y property and search for the best fit to it with x
  % properties
  
  y_m = y(:,m);
  yCell{m} = y_m;
  allInd = 1:numX;

  % initialize by setting the best fit to "no x properties", and infinite error
  bestInd = [];
  rChiSquared(m) = Inf;
  
  done = false;
  numAdd = 1;
  while ~done
    % try to add numAdd properties that aren't in use, and see if adding any of
    % them improves the fit
  
    %Put the unused indices in missing
    missing = setdiff(allInd, bestInd);
    
    %Figure out all the combinations of missing indices to try adding:
    if numAdd == 1
      combos = missing';
    else
      combos = nchoosek(missing, numAdd);
    end
    numCombos = size(combos, 1);

    newBest = false;
    for n = 1:numCombos
      %Piece together which indices to use for this comobo
      ind = [bestInd, combos(n,:)];
      
      %Now get the reduced sum of squares from the fit.
      [score_n, rSquared_n, coefs_n, yPredict_n] = ...
	      getLinearFit(x(:,ind), y_m, options.fitOffset);
        
      if score_n < rChiSquared(m)
        % this combo improved over the previous best fit
        
	      newBest = true;
	      rChiSquared(m) = score_n;
        rSquared(m) = rSquared_n;
        coefs{m} = coefs_n;
        yPredictCell{m} = yPredict_n;
        fitIndices{m} = ind;
      end
    end
    if newBest
      % one of the combos was an improvement, keep looking unless we're already
      % using all the properties
      
      numAdd = 1;
      bestInd = fitIndices{m};
      if length(fitIndices{m}) == numX
        done = true;
      end
    elseif numAdd == options.maxLookAhead || ...
	         length(bestInd) + numAdd == numX || ...
	         length(bestInd) + numAdd == options.maxCombos
      % no improvement, and we've reached a logical reason to stop looking
      done = true;
    else
      % no improvement, try simultaneously adding more properties next time
      numAdd = numAdd + 1;
    end
  end
  
  if options.getPVals
    % p-values have been requested, estimate them with a Jackknife fit and
    % the Pearson correlation coefficient
    [~, pVals(m)] = JackknifeFit(x(:,bestInd), y_m);
  end
end

varargout = {fitIndices, yCell, yPredictCell, coefs};
if options.getPVals
  varargout = [varargout, {pVals}];
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [rChiSquared, rSquared, varargout] = getLinearFit(x, y, fitOffset)
% [reducedChiSquared, rSquared, coefs, yPredict] = getLinearFit(x, y)
% Obtain a linear fit of x to y, returning measures of fit success and
% results of the fit

numData = size(x, 1);
numXTypes = size(x, 2);
% numYTypes = size(y, 2);  %this equals 1

goodInd = find(sum(~isfinite(x), 2) == 0 & isfinite(y));
numGood = length(goodInd);
if fitOffset
  degreesOfFreedom = numGood - numXTypes - 1;
  x = [x, ones(numData, 1)];
else
  degreesOfFreedom = numGood - numXTypes;
  % if we're not fitting an offset, it's necessary to re-zscore
  x(goodInd,:) = ZScore(x(goodInd,:), 1, 1);
  y(goodInd) = ZScore(y(goodInd), 1, 1);
end

if degreesOfFreedom < 1
  rChiSquared = Inf;
  rSquared = 0;
  if nargout == 0
    varargout = {};
  elseif nargout == 1
    varargout = {NaN(numXTypes, 1)};
  else
    varargout = {NaN(numXTypes, 1), NaN(numData, 1)};
  end
  return
end

yPredict = NaN(numData, 1);

coefs = pinv(x(goodInd,:)) * y(goodInd);
yPredict(goodInd) = x(goodInd,:) * coefs;

errY = yPredict(goodInd) - y(goodInd);
chiSquared = cov(errY) / cov(y(goodInd));
rSquared = 1 - chiSquared;
if rSquared < -1e-12 || chiSquared < -1e-12
  fprintf(2, 'Weird fit results.\n');
  keyboard
end
%rChiSquared = chiSquared / degreesOfFreedom;
rChiSquared = sum(errY.^2) / degreesOfFreedom;
if nargout == 0
  varargout = {};
elseif nargout == 1
  varargout = {coefs};
else
  varargout = {coefs, yPredict};
end
return
