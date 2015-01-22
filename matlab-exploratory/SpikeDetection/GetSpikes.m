function spike = GetSpikes(dT, v, varargin)
% spike = GetSpikes(dT, v, plotSubject, timesOnly, firstOnly)
% Analyzes a single voltage waveform, looking for spikes
%    and bursts, and calculating relevant frequencies.
%
%  INPUT PARAMETERS:
%   -dT is sample time in ms
%   -v is array of voltages in mV
%    OPTIONAL:
%     -plotSubject should be set to true[false] to produce[suppress]
%       plots of waveforms/analysis.  Alternatively, it can be set
%       to a string to aid it titling plots (e.g. 'Exp #71')
%       plotSubject defaults to false
%     -timesOnly: defaults to false.  If true, only compute spike
%       times, no other spike information
%     -lowCutoff: defaults to automatically detected. The threshold for
%       negative derivatives that constitutes a potential spike
%     -highCutoff: defaults to automatically detected. The threshold for
%       positive derivatives that constitutes a potential spike
%     -bracketWidth: defaults to 15ms. A spike must have a large positive
%       derivative followed by large negative within this interval
%     -pFalseSpike: defaults to 0.05. Estimated proability of finding a
%       spurious spike in the whole trace
%     -recursive: defaults to false. if spikes are found, remove them and
%       try to find spikes in the remaining data. Keep doing this until no
%       new spikes are found
%     -debugPlots: defaults to false. When true, make extra plots depicting
%       the spike-finding process
%
%  OUTPUT PARAMETERS:
%   -spike:  a structure with the following fields
%    -spike.times is a plain list of spike times (in ms)
%    -spike.freq is overall spiking frequency (in Hz)
%    -spike.intervals is a list of interspike intervals (in ms)
%    -spike.frequencies is a list of instantaneous frequencies (in Hz)
%    Shape information structures (should be self-descriptive)
%    -spike.maxV, spike.maxDeriv, spike.minDeriv, spike.preMinV,
%     spike.postMinV, spike.preMaxCurve, spike.postMaxCurve
%           Each contains a list of times/voltage points, and if relevant
%           another quantity (such as K for curvatures)
%
%List structures usually will have a name.list element, as well as
%  name.mean, name.stdDev, name.variance, name.coefOfVar
%  (a few are just plain lists)
%If a feature is not detected, relevant frequencies are set to
%  zero, and relevant lists are empty
%

if nargin < 2
  help GetSpikes
  error('Invalid number of arguments.')
end
if length(dT) > 1
  % user passed in array of time, rather than dT
  if length(dT) ~= length(v)
    error('Time and Voltage arrays have different length!')
  end
  dT = (dT(end) - dT(1)) / (length(dT) - 1);
end

if size(v,1) > 1
  
  if size(v,2) > 1
    error('Voltage must be a single array, not a matrix')
  else
    v = v';
  end
end

% set the default options
defaultOptions = { ...
  'plotSubject', false, ...
  'timesOnly', false, ...
  'firstOnly', false, ...
  'lowCutoff', NaN, ...
  'highCutoff', NaN, ...
  'bracketWidth', 5.0, ...
  'pFalseSpike', 1.0e-4, ...
  'recursive', false, ...
  'removeOutliers', true, ...
  'debugPlots', false ...
};
% get the options overrides from varargin
options = GetOptions(defaultOptions, varargin);

%First get the spike times
spike = getSpikeTimesThreshold(dT, v, options);
if options.recursive
  oldSpikeTimes = [];
  while length(oldSpikeTimes) < length(spike.times)
    oldSpikeTimes = spike.times;
    spike = getSpikeTimesThreshold(dT, v, options, spike);
  end
end

%Next get the overall spike frequency
spike.freq = getSpikeFrequency(spike.times, dT * (length(v) - 1));

callstack = dbstack;
if needPlot(options, callstack)
  hSpikes = PlotGetSpikes(dT, v, spike, options);
  
  % link relevant time axis together
  if options.debugPlots
    aSpikes = get(hSpikes, 'CurrentAxes');
    derivsTitle = makeTitle('dV/dT vs. t', options);
    aDerivs = get(findobj('name', derivsTitle),'CurrentAxes');
    kTitle = makeTitle('Curvature', options);
    aK = get(findobj('name', kTitle), 'CurrentAxes');
    aHandles = [aSpikes, aDerivs, aK];
    linkaxes(aHandles, 'x');
  end
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function spike = getSpikeTimesThreshold(dT, v, options, oldSpike)
% Finds spikes by looking for points where derivative is large
% (positive) followed quickly by a large (negative) derivative.

if nargin < 4
  oldSpike = [];
end

numV = length(v);

maxTimeWidth = options.bracketWidth;  %ms

maxIndDiff = maxTimeWidth / dT;
dV = diff(v);
absDV = abs(dV);
digitizationNoise = min(absDV(absDV > 0));
maxNyquistFrac = 0.25;
minNyquistFrac = min(4.0 / maxIndDiff, maxNyquistFrac);
nyquistFrac = maxNyquistFrac + ...
  (minNyquistFrac - maxNyquistFrac) * tanh(digitizationNoise / dT / 0.2);

if dT < .005
  warning('MATLAB:SmallDT', ...
          'Very small dT (%g). Note dT should be in ms.', dT)
end

if options.timesOnly
  deriv = WienerDeriv(v, dT, 1, 'nyquistFraction', nyquistFrac);
  deriv2 = [];
else
  [deriv, deriv2] = WienerDeriv(v, dT, 1, 2, 'nyquistFraction', nyquistFrac);
  %[deriv, deriv2] = WienerDeriv(v, dT, 1, 2, 'nyquistFraction', 0.25);
end

if isnan(options.lowCutoff) || isnan(options.highCutoff)
  [lowCutoff, highCutoff] = ...
    getAutoCutoffs(dT, deriv, nyquistFrac, options, oldSpike);
  if ~isnan(options.lowCutoff)
    lowCutoff = options.lowCutoff;
  elseif ~isnan(options.highCutoff)
    highCutoff = options.highCutoff;
  end
else
 lowCutoff = options.lowCutoff;
 highCutoff = options.highCutoff;
end

fprintf('GetSpikes.m: low/high cutoff: %g/%g\n', lowCutoff, highCutoff)

n1List = [];
n2List = [];

n1Stop = numV - maxIndDiff;
if highCutoff - lowCutoff < .1  %no spikes so skip detection
  n1 = n1Stop;
else
  n1 = 1;
end


while n1 < n1Stop
  if deriv(n1) < highCutoff
    n1 = n1 + 1;
  else  %Found potential beginning of a spike, try to bracket a spike
    n2 = n1 + 1;
    bracketSuccess = false;
    n2Stop = n1 + maxIndDiff;
    while n2 < n2Stop
      if deriv(n2) > lowCutoff
        if deriv(n2) > highCutoff
          %If we've gone back above cutoff, reset n1
          n2Stop = min(n2, n1Stop) + maxIndDiff;
        end
        n2 = n2 + 1;
      else
        bracketSuccess = true;
        break
      end
    end
    if ~bracketSuccess
      n1 = n2 + 1;
      continue;
    end

    if n2 == numV || deriv(n2 + 1) > highCutoff || n2 - n1 < 2
      %probably just spurious
      n1 = n2 + 1;
      continue
    end

    %We've bracketed a spike between n1 and n2
    
    %We want to get some spike shape info, so extend n1 and n2
    %until we cross deriv = 0
    while n1 > 1 && deriv(n1) > 0
      n1 = n1 - 1;
    end
    n1List = [n1List, n1]; %#ok<AGROW>
    
    while n2 < numV && deriv(n2) < 0
      n2 = n2 + 1;
    end
    n2List = [n2List, n2]; %#ok<AGROW>
    if options.firstOnly
      break
    end
    n1 = n2 + 1;
  end
end


%now spikes are all bracketed between n1List and n2List

%  Get spike shape
spike = getSpikeShape(n1List, n2List, dT, v, deriv, deriv2, ...
                      digitizationNoise, options);

%  Calculate spike intervals and frequencies
if ~options.timesOnly
  if isempty(spike.times)
    spike.intervals = [];
    spike.frequencies = [];  
  else
    spike.intervals = spike.times(2:end) - spike.times(1:(end-1));
    spike.frequencies = 1000 ./ spike.intervals;
  end
end

%  Make plots if requested
if needPlot(options) && options.debugPlots
  plotGetSpikeTimes(dT, v, deriv, lowCutoff, highCutoff, options);
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [lowCutoff, highCutoff] = getAutoCutoffs(dT, deriv, ...
                                            nyquistFrac, options, oldSpike)
% Get cutoffs for significant spiking


if ~isempty(oldSpike)
  % first remove detected spikes from the list of voltage derivatives, then
  % sort into increasing order
  for n = length(oldSpike.n1List):-1:1
    n1 = oldSpike.n1List(n);
    n2 = oldSpike.n2List(n);
    deriv(n1:n2) = [];
  end
end
% sort the voltage derivative into a list of increasing order
sortDeriv = sort(deriv);

% next compute how rare/extreme derivatives cutoffs need to be to set a
% false detection probability

% number of *effective* trace points in a bracketed spike
nBracket = nyquistFrac * options.bracketWidth / dT;
% length of trace
len = length(sortDeriv);
logOdds = 4 * log(1 - options.pFalseSpike) / len / nBracket;

% this is how rare a derivative has to be (either positive or negative) to
% achieve the given false-detection probability
minRareness = sqrt(-logOdds);

% compute approximate 1/2-sigma levels for positive and negative
% derivatives, based on presumably nearly-gaussian small derivatives near
% the median derivative
checkSigma = 0.5;
medianInd = round(0.5 * len);
medianDV = sortDeriv(medianInd);
%sigmaFact = 0.5 * erf(1.0/sqrt(2));
sigmaFact = erf(checkSigma / sqrt(2.0));
sigmaPosInd = round((1.0 + sigmaFact) * 0.5 * len);
sigmaNegInd = round((1.0 - sigmaFact) * 0.5 * len);

sigmaPos = (sortDeriv(sigmaPosInd) - medianDV) / checkSigma;
sigmaNeg = (medianDV - sortDeriv(sigmaNegInd)) / checkSigma;

% estimate number of sigma needed to achieve minRareness
numSigma = sqrt(2) * erfcinv(2 * minRareness);

% compute cutoffs by moving them appropriate number of sigma away from
% median
lowCutoff = medianDV - numSigma * sigmaNeg;
highCutoff = medianDV + numSigma * sigmaPos;

if options.debugPlots
  titleStr = makeTitle('Spike Thresholds', options);
  
  h = NamedFigure(titleStr);
  set(h, 'WindowStyle', 'docked')
  clf
  [n, x] = hist(sortDeriv, 1000);
  n = n ./ max(n);
  bar(x, n);
  hold on
  plot([lowCutoff, lowCutoff], [0, 1], 'r')
  plot([highCutoff, highCutoff], [0, 1], 'g')
  hold off
  xlabel('Derivative (mV/ms)')
  ylabel('Relative Frequency')
  title(RealUnderscores(titleStr))
  legend('Derivatives', 'Low threshold', 'High threshold')
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function spike = getSpikeShape(n1List, n2List, dT, v, deriv, deriv2, ...
                               digitizationNoise, options)
numSpikes = length(n1List);
spike.n1List = n1List;
spike.n2List = n2List;

spike.times = nan(1, numSpikes);
badSpikes = false(1, numSpikes);

if ~options.timesOnly
  % compute curvature
  
  K = deriv2 .* (1 + deriv.^2).^-1.5;
  %K = deriv2;
  if options.debugPlots
    titleStr = makeTitle('Curvature', options);
    h = NamedFigure(titleStr);
    set(h, 'WindowStyle', 'docked')
    plot((0.001 * dT) * (0:(length(K)-1)), K)
    xlabel('Time (s)')
    ylabel('Curvature')
    title(RealUnderscores(titleStr))
  end
  
  spike.maxV.v = nan(1, numSpikes);
  spike.maxV.t = nan(1, numSpikes);
  spike.maxV.ind = nan(1, numSpikes);
  spike.maxDeriv.v = nan(1, numSpikes);
  spike.maxDeriv.dV = nan(1, numSpikes);
  spike.maxDeriv.t = nan(1, numSpikes);
  spike.maxDeriv.ind = nan(1, numSpikes);
  spike.minDeriv.v = nan(1, numSpikes);
  spike.minDeriv.dV = nan(1, numSpikes);
  spike.minDeriv.t = nan(1, numSpikes);
  spike.minDeriv.ind = nan(1, numSpikes);
  spike.preMinV.v = nan(1, numSpikes);
  spike.preMinV.t = nan(1, numSpikes);
  spike.preMinV.ind = nan(1, numSpikes);
  spike.postMinV.v = nan(1, numSpikes);
  spike.postMinV.t = nan(1, numSpikes);
  spike.postMinV.ind = nan(1, numSpikes);
  spike.preMaxCurve.v = nan(1, numSpikes);
  spike.preMaxCurve.K = nan(1, numSpikes);
  spike.preMaxCurve.t = nan(1, numSpikes);
  spike.preMaxCurve.ind = nan(1, numSpikes);
  spike.postMaxCurve.v = nan(1, numSpikes);
  spike.postMaxCurve.K = nan(1, numSpikes);
  spike.postMaxCurve.t = nan(1, numSpikes);
  spike.postMaxCurve.ind = nan(1, numSpikes);  
end
if numSpikes == 0
  return
end


for m = 1:numSpikes
  n1 = n1List(m);
  n2 = n2List(m);

  %Find the moment and voltage of maximum depolarization
  [maxV, tMaxV, nMaxV] = getExtremum(v, dT, n1, n2, 'max', false, true);
  
  if isnan(tMaxV)
    badSpikes(m) = true;
    continue
  end
  
  spike.times(m) = tMaxV;
  if options.timesOnly
    continue
  end
  
  %Find the max derivative
  [maxDV, tMaxDV, nMaxDV] = getExtremum(deriv, dT, n1, nMaxV, 'max', true);
  vMaxDV = v(nMaxDV);
  %Find the min derivative
  [minDV, tMinDV, nMinDV] = getExtremum(deriv, dT, nMaxV, n2, 'min', true);
  vMinDV = v(nMinDV);
  
  %Find the max curvature near the spike
  nLook = nMaxDV;
  kLook = K(nLook);
  while nLook > n1
    kOld = kLook;
    nLook = nLook - 1;
    kLook = K(nLook);
    if kLook < kOld
      break
    end
  end
  [preMaxK, tPreMaxK, nPreMaxK] = getExtremum(K, dT, nLook, nLook + 2, ...
					      'max', true);
  vPreMaxK = v(nPreMaxK);
  
  nLook = nMinDV;
  kLook = K(nLook);
  while nLook < n2
    kOld = kLook;
    nLook = nLook + 1;
    kLook = K(nLook);
    if kLook < kOld
      break
    end
  end
  [postMaxK, tPostMaxK, nPostMaxK] = getExtremum(K, dT, nLook - 2, ...
             nLook, 'max', true);
  vPostMaxK = v(nPostMaxK);

  %Find minimum voltage before and after spike
  while n1 > 1 && v(n1-1) <= v(n1)
    n1 = n1 - 1;
  end
  while n2 < length(v) && v(n2+1) <= v(n2)
    n2 = n2 + 1;
  end
  [preMinV, tPreMin, nPreMin] = getExtremum(v, dT, n1, n1+3, 'min', true);
  [postMinV, tPostMin, nPostMin] = ...
    getExtremum(v, dT, n2-3, n2, 'min', true);
  
  height = min(maxV - vMaxDV, maxV - vMinDV);
  if height < 2 * digitizationNoise
    % this spike is bad
    badSpikes(m) = true;
    continue
  end
  
  spike.maxV.v(m) = maxV;
  spike.maxV.t(m) = tMaxV;
  spike.maxV.ind(m) = nMaxV;
  spike.maxDeriv.v(m) = vMaxDV;
  spike.maxDeriv.dV(m) = maxDV;
  spike.maxDeriv.t(m) = tMaxDV;
  spike.maxDeriv.ind(m) = nMaxDV;
  spike.minDeriv.v(m) = vMinDV;
  spike.minDeriv.dV(m) = minDV;
  spike.minDeriv.t(m) = tMinDV;
  spike.minDeriv.ind(m) = nMinDV;
  spike.preMinV.v(m) = preMinV;
  spike.preMinV.t(m) = tPreMin;
  spike.preMinV.ind(m) = nPreMin;
  spike.postMinV.v(m) = postMinV;
  spike.postMinV.t(m) = tPostMin;
  spike.postMinV.ind(m) = nPostMin;
  spike.preMaxCurve.v(m) = vPreMaxK;
  spike.preMaxCurve.K(m) = preMaxK;
  spike.preMaxCurve.t(m) = tPreMaxK;
  spike.preMaxCurve.ind(m) = nPreMaxK;
  spike.postMaxCurve.v(m) = vPostMaxK;
  spike.postMaxCurve.K(m) = postMaxK;
  spike.postMaxCurve.t(m) = tPostMaxK;
  spike.postMaxCurve.ind(m) = nPostMaxK;
end

if options.removeOutliers
  spikeDV = spike.maxDeriv.dV(~badSpikes);
  medianDV = median(spikeDV);
  threshold = min(0.5 * medianDV, medianDV - 3 * std(spikeDV));
  badSpikes = badSpikes | (spike.maxDeriv.dV < threshold);
end

if any(badSpikes)
  % remove bad spikes from spike struct
  
  goodSpikes = ~badSpikes;
  
  fNames1 = fieldnames(spike);
  for n1 = 1:length(fNames1)
    name1 = fNames1{n1};
    try
      fNames2 = fieldnames(spike.(name1));
    catch %#ok<CTCH>
      checkList = spike.(name1);
      spike.(name1) = checkList(goodSpikes);
      continue
    end
    for n2 = 1:length(fNames2)
      name2 = fNames2{n2};
      checkList = spike.(name1).(name2);
      spike.(name1).(name2) = checkList(goodSpikes);
    end
  end
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [maxV, tMax, nMax] = getExtremum(v, dT, n1, n2, extremumStr, ...
                                          simple, forceRealMax)
% from a bracketed extremum, find the actual extreme time and value
if nargin < 7
  forceRealMax = false;
  if nargin < 6
    simple = false;
  end
end

if strcmpi(extremumStr, 'min')
  [maxV, nMax] = min(v(n1:n2));
else
  [maxV, nMax] = max(v(n1:n2));
end
nMax = nMax + n1 - 1;

if simple || nMax == 1 || nMax == length(v)
  tMax = dT * (nMax - 1);
  return
end

%Refine by modeling trace as parabola
n1 = nMax - 1;
n2 = nMax;
n3 = nMax + 1;
t2 = dT * n1;
t3 = dT * n2;
t1 = t2 - dT;

if v(n1) == v(n2)
  if v(n2) == v(n3)
    maxV = v(n2);
    tMax = dT * (n2 - 1);
    return
  else
    tMax = (t1 + t2) / 2;
    coeff = (v(n2) - v(n3)) / ((t2 - tMax)^2 - (t3 - tMax)^2);
  end
elseif v(n2) == v(n3)
  tMax = (t2 + t3) / 2;
  coeff = (v(n2) - v(n1)) / ((t2 - tMax)^2 - (t1 - tMax)^2);
else
  val1 = (v(n2) - v(n1)) / (v(n2) - v(n3));

  b = 2 * (t2 - t1 + val1 * (t3 - t2));
  c = val1 * (t2*t2 - t3*t3) + t1*t1 - t2*t2;

  tMax = -c / b;
  % check for sanity on this extremum time
  if tMax <= t1
    if forceRealMax
      tMax = NaN;
    else
      tMax = t1;
    end
    return
  elseif tMax >= t3
    if forceRealMax
      tMax = NaN;
    else
      tMax = t3;
    end
    return
  end

  
  coeff = (v(n2) - v(n1)) / ((t2 - tMax)^2 - (t1 - tMax)^2);
  %arbitrary which formula to use:
  %coeff = (v(n3) - v(n1)) / ((t(n3) - tMax)^2 - (t(n1) - tMax)^2);
end

maxV = v(n2) - coeff * (t2 - tMax)^2;
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function freq = getSpikeFrequency(times, tFinal)
if isempty(times) || tFinal == 0
  freq = 0;
  return
end

tHalf = .5 * tFinal;
if isempty(find(times > tHalf, 1))
  %Check if there are no events in the second half of the experiment
  %  if so, presumably it just took a LONG time to settle down, so
  %  label the cell as NOT spiking
  freq = 0;
  return
end

numEvents = length(times);
if numEvents == 1
  freq = 1000 * numEvents / tFinal;
else
  freq = 1000 * (numEvents - 1) / (times(end) - times(1));
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function plotVar = needPlot(options, callStack)
if ischar(options.plotSubject)
  plotVar = true;
else
  plotVar = options.plotSubject;
end
if plotVar && nargin == 2 && length(callStack) >= 2
  plotVar = ~strcmp(callStack(2).name, 'AnalyzeWaveform');
end
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function h = plotGetSpikeTimes(dT, v, deriv, lowCutoff, highCutoff, ...
                               options)
% Plot the derivatives and thresholds, showing how the affect spike
% detection
titleStr = makeTitle('dV/dT vs. t', options);

h = NamedFigure(titleStr);
set(h, 'WindowStyle', 'docked');
hold off

numV = length(v);
dTSeconds = 0.001 * dT;
tFinal = dTSeconds * (numV - 1);
plot(0:dTSeconds:tFinal, deriv, 'b-')
hold on
plot([0, tFinal], [lowCutoff, lowCutoff], 'r-')
plot([0, tFinal], [highCutoff, highCutoff], 'g-')
xlabel('Time (s)', 'FontSize', 18)
ylabel('dV/dT (mV/ms)', 'FontSize', 18)
title(RealUnderscores(titleStr), 'FontSize', 18)
hold off
return



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function titleStr = makeTitle(titleBase, options)
% set the full title for a figure based on base title and plotSubject
if ischar(options.plotSubject)
  titleStr = [options.plotSubject, ': ', titleBase];
else
  titleStr = titleBase;
end
return